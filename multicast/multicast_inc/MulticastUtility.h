#ifndef MULTICAST_H
#define MULTICAST_H
#include "board_info.h"
#include "udp.h"
void createMulticastListen();
void multicastServerRecv(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port);
void createMulticastClient();
void multicastClientRecv(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port);
#endif