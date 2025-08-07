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
    strcpy(newbuf, "message from response callback: ");
    strcat(newbuf, buffer);
    strcat(newbuf, "\n");
    return newbuf;
} //-----


int main() {
    stdio_init_all();

    const char *ap_name = "picow_test";
    const char *password = "password";


    TCP_LOCAL *tcp_local = ap_tcp_init(ap_name,password);
    tcp_local->fn = res;

    int err = ap_tcp_open(tcp_local);

    if (err != NULL) {
        printf("Failed to open TCP Access Point.\n");
    }

    ap_tcp_shutdown(tcp_local);

    printf("Test completed\n");

    return 0;

}

