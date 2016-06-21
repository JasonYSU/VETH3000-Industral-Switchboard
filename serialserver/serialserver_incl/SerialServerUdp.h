#ifndef SERIALSERVERUDP_H
#define SERIALSERVERUDP_H
#include "board_info.h"
#include "udp.h"
#define MAX_LENGTH_RECV 10 * 1500 //定义串口服务器最大的接收数据长度
typedef struct udp_serial_server{
	uint8_t serial_no;  //串口号
	struct udp_pcb *udp_pcb;
}UDPSerialServerState;
extern struct udp_serial_server serial_server[MAX_SERIAL_NUM];
void serialUDPServerAppcall(void);
/* 添加UDP串口服务器的监听端口 */
err_t addUdpSerialListenPort(uint16_t port_no,uint8_t serial_no);
void unlistenUdpSerialPort(uint16_t port_no,uint8_t serial_no);
int isUdpSerialServerRequest(uint16_t request_port);
void handleUdpSerialConnection(struct udp_serial_server *udp_serial_server);
extern uint32_t udp_serv_recv[4];
#endif