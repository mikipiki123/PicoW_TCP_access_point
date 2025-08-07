//
// Created by Michael Berner on 13/07/2025.
//

#include "lwip/ip_addr.h"


#include "dhcpserver/dhcpserver.h"
#include "dnsserver/dnsserver.h"


#ifndef LOCALTCP_H
#define LOCALTCP_H

#define TCP_PORT 9999

typedef struct TCP_SERVER_T_ {
    struct tcp_pcb *server_pcb;
    bool complete;
    ip_addr_t gw;
} TCP_SERVER_T;

typedef struct TCP_CONNECT_STATE_T_ {
    struct tcp_pcb *pcb;
    int sent_len;
    char headers[128];
    char result[256];
    int header_len;
    int result_len;
    ip_addr_t *gw;
} TCP_CONNECT_STATE_T;

typedef struct {
    TCP_SERVER_T *state;
    TCP_CONNECT_STATE_T *con_state;

    dhcp_server_t dhcp_server;
    dns_server_t dns_server;

    char* (*fn)(char* buffer);

}TCP_LOCAL;


//initialize TCP and AP features
TCP_LOCAL *ap_tcp_init(const char* ap_name,const char* password);

//open Access point
int ap_tcp_open(TCP_LOCAL *tcp_local);

//closing all TCP Initializations
void ap_tcp_shutdown(TCP_LOCAL *tcp_local);


#endif //LOCALTCP_H
