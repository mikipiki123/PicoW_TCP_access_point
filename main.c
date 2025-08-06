#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/netif.h"
#include "lwip/ip4_addr.h"

#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#include "localtcp.h"

// -- example --
char* res (char* buffer) {
    char* newbuf = (char*) calloc(1024,1);
    strcpy(newbuf, "message from response: ");
    strcat(newbuf, buffer);
    strcat(newbuf, "\n");
    return newbuf;
} //-----


int main() {
    stdio_init_all();

    const char *ap_name = "picow_test_miki";
    const char *password = "password";


    TCP_LOCAL *tcp_local = tcp_initial(ap_name,password);
    tcp_local->fn = res;

    int err = tcp_open(tcp_local);

    if (err != NULL) {
        DEBUG_printf("ERROR VALUE: %d \n",err);
    }

    tcp_shut(tcp_local);

    printf("Test complete\n");

    return 0;

}

