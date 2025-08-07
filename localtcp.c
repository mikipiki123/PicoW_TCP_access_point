//
// Created by Michael Berner on 13/07/2025.
//
#include <string.h>

#include "localtcp.h"

#include <stdio.h>

#include "lwip/netif.h"
#include "lwip/ip4_addr.h"

#include "lwip/tcp.h"

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "lwip/pbuf.h"

#include "pico/time.h"


//tcp_set
#define TIME_PASSED_SEC 60
#define POLL_TIME_S 5

absolute_time_t start;
absolute_time_t end;


void tcp_server_close(TCP_LOCAL *tcp_local) {
    if (tcp_local->state->server_pcb) {
        tcp_arg(tcp_local->state->server_pcb, NULL);
        tcp_close(tcp_local->state->server_pcb);
        tcp_local->state->server_pcb = NULL;
    }
}


err_t tcp_close_client_connection(TCP_CONNECT_STATE_T *con_state, struct tcp_pcb *client_pcb, err_t close_err) {
    if (client_pcb) {
        assert(con_state && con_state->pcb == client_pcb);

        printf("Closing connection\n");
        char* cl = "Closing connection..\n";
        tcp_write(client_pcb, cl, strlen(cl),0);
        tcp_output(client_pcb);

        tcp_arg(client_pcb, NULL);
        tcp_poll(client_pcb, NULL, 0);
        tcp_sent(client_pcb, NULL);
        tcp_recv(client_pcb, NULL);
        tcp_err(client_pcb, NULL);
        err_t err = tcp_close(client_pcb);
        if (err != ERR_OK) {
            printf("close failed %d, calling abort\n", err);
            tcp_abort(client_pcb);
            close_err = ERR_ABRT;
        }
        if (con_state) {
            free(con_state);
        }
    }
    return close_err;
}

err_t tcp_server_sent(void *arg, struct tcp_pcb *pcb, u16_t len) {
    TCP_LOCAL *tcp_local = (TCP_LOCAL*)arg;
    printf("tcp_server_sent %u\n", len);
    tcp_local->con_state->sent_len += len;
    return ERR_OK;
}



err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    TCP_LOCAL *tcp_local = (TCP_LOCAL*)arg;

    if (!p) {
        printf("\nconnection closed\n");
        return tcp_close_client_connection(tcp_local->con_state, pcb, ERR_OK);
    }
    assert(con_state && con_state->pcb == pcb);
    if (p->tot_len > 0) {
        printf("tcp_server_recv %d err %d\n", p->tot_len, err);
#if 0
        for (struct pbuf *q = p; q != NULL; q = q->next) {
            printf("in: %.*s\n", q->len, q->payload);
        }
#endif


        start = get_absolute_time();

        // Copy the request into the buffer
        char buffer[1024];

        u32_t numbytes = pbuf_copy_partial(p, buffer, p->tot_len, 0);

        buffer[numbytes - 1] = '\0'; //overwrite '\n'


        char* response = tcp_local->fn(buffer);
        printf("%s\n", response);


        if (strcmp(buffer, "close") == 0) { //closing connection with client + ends TCP loop.
            tcp_local->state->complete = true;
            return tcp_close_client_connection(tcp_local->con_state, pcb, ERR_OK);
        }


        err = tcp_write(pcb, response, strlen(response),0);
        tcp_output(pcb);
        if (err != ERR_OK) {
            printf("\ntcp_server_recv failed, why??\n");
        }

        tcp_recved(pcb, p->tot_len);

    }
    pbuf_free(p);
    return ERR_OK;
}

err_t tcp_server_poll(void *arg, struct tcp_pcb *pcb) {
    TCP_LOCAL *tcp_local = (TCP_LOCAL*)arg;

    printf("tcp_server_poll_fn\n");

    //close client connection if not received message for more than minute
    end = get_absolute_time();

    int64_t sec = absolute_time_diff_us(start, end) / 1000000;
    printf("seconds: %lld\n", sec);

    if (sec > TIME_PASSED_SEC) {
        printf("timed_out..\n");
        return tcp_close_client_connection(tcp_local->con_state, pcb, ERR_OK);
    }

    return ERR_OK;
}

void tcp_server_err(void *arg, err_t err) {
    TCP_LOCAL *tcp_local = (TCP_LOCAL*)arg;
    if (err != ERR_ABRT) {
        printf("tcp_client_err_fn %d\n", err);
        tcp_close_client_connection(tcp_local->con_state, tcp_local->con_state->pcb, err);
    }
}

err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
    TCP_LOCAL *tcp_local = (TCP_LOCAL*)arg;

    if (err != ERR_OK || client_pcb == NULL) {
        printf("failure in accept\n");
        return ERR_VAL;
    }
    printf("client connected\n");

    start = get_absolute_time();

    // Create the state for the connection
    tcp_local->con_state = (TCP_CONNECT_STATE_T *)calloc(1,sizeof(TCP_CONNECT_STATE_T));
    if (!tcp_local->con_state) {
        printf("failed to allocate connect state\n");
        return ERR_MEM;
    }
    tcp_local->con_state->pcb = client_pcb; // for checking
    tcp_local->con_state->gw = &tcp_local->state->gw;

    // setup connection to client
    tcp_arg(client_pcb, tcp_local);
    tcp_sent(client_pcb, tcp_server_sent);
    tcp_recv(client_pcb, tcp_server_recv);
    tcp_poll(client_pcb, tcp_server_poll, POLL_TIME_S);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

bool tcp_server_open(TCP_LOCAL *tcp_local) {

    printf("starting server on port %d\n", TCP_PORT);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb) {
        printf("failed to create pcb\n");
        return false;
    }

    err_t err = tcp_bind(pcb, IP_ANY_TYPE, TCP_PORT);
    if (err) {
        printf("failed to bind to port %d\n",TCP_PORT);
        return false;
    }

    tcp_local->state->server_pcb = tcp_listen_with_backlog(pcb, 1);
    if (!tcp_local->state->server_pcb) {
        printf("failed to listen\n");
        if (pcb) {
            tcp_close(pcb);
        }
        return false;
    }

    tcp_arg(tcp_local->state->server_pcb, tcp_local);
    tcp_accept(tcp_local->state->server_pcb, tcp_server_accept);

    // printf("Try connecting.. (press 'd' to disable access point)\n");
    return true;
}

// void key_pressed_func(void *param) {
//     assert(param);
//     TCP_SERVER_T *state = (TCP_SERVER_T*)param;
//     int key = getchar_timeout_us(0); // get any pending key press but don't wait
//
//     if (key == 'k' || key == 'K') {
//         printf("got notification from KEYBOARD!\n");
//     }
//
//     if (key == 'd' || key == 'D') {
//         cyw43_arch_lwip_begin();
//         cyw43_arch_disable_ap_mode();
//         cyw43_arch_lwip_end();
//         state->complete = true;
//     }
// }

TCP_LOCAL *ap_tcp_init(const char* ap_name,const char* password) {

    sleep_ms(5000);

    TCP_LOCAL *tcp_local = (TCP_LOCAL *)calloc(1, sizeof(TCP_LOCAL));

    if (!tcp_local) {
        printf("failed to allocate tcp_local\n");
        return NULL;
    }

    tcp_local->state = (TCP_SERVER_T *)calloc(1, sizeof(TCP_SERVER_T));

    if (!tcp_local->state) {
        printf("failed to allocate state\n");
        return NULL;
    }

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return NULL;
    }

    // Get notified if the user presses a key
    // stdio_set_chars_available_callback(key_pressed_func, tcp_local->state);


    cyw43_arch_enable_ap_mode(ap_name, password, CYW43_AUTH_WPA2_AES_PSK);
    printf("Connecting to '%s'\n", ap_name);

#if LWIP_IPV6
#define IP(x) ((x).u_addr.ip4)
#else
#define IP(x) (x)
#endif
    //
    ip4_addr_t mask;
    IP(tcp_local->state->gw).addr = PP_HTONL(CYW43_DEFAULT_IP_AP_ADDRESS);
    IP(mask).addr = PP_HTONL(CYW43_DEFAULT_IP_MASK);
    //
    // #undef IP
    //

    // Start the dhcp server
    dhcp_server_init(&tcp_local->dhcp_server, &tcp_local->state->gw, &mask);

    // Start the dns server
    dns_server_init(&tcp_local->dns_server, &tcp_local->state->gw);

    return tcp_local;

}

int ap_tcp_open(TCP_LOCAL *tcp_local) {

    if (tcp_local->fn == NULL) {
        perror("missing callback function\n");
        return -1;
    }

    if (!tcp_server_open(tcp_local)) {
        printf("failed to open server\n");
        return -1;
    }

    tcp_local->state->complete = false;
    printf("Starting server on port %d\n", TCP_PORT);

    while(!tcp_local->state->complete) {
        sleep_ms(1000);
    }
    return 0;
}

void ap_tcp_shutdown(TCP_LOCAL *tcp_local) {
    tcp_server_close(tcp_local);
    dns_server_deinit(&tcp_local->dns_server);
    dhcp_server_deinit(&tcp_local->dhcp_server);
    cyw43_arch_deinit();
}

