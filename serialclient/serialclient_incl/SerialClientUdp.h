#ifndef SERIALCLIENTUDP_H
#define SERIALCLIENTUDP_H
#include "board_info.h"
/* 串口客户端与远端主机建立UDP连接 */
struct udp_pcb * serialClientUdpConnect(char *host, uint16_t port,uint8_t serial_no);
void processSerialClientUdpConnection(char *host, uint16_t port,uint8_t serial_index);
uint32_t handleSerialClientUdpOutput(uint8_t serial_no);
/*  */
#endif