#ifndef SERIAL_CLIENT_H
#define SERIAL_CLIENT_H
#include "board_info.h"
#include "tcp.h"
#define TCPCONNECT 0
#define UDPCONNECT 1
#define MAX_SERIAL_CLIENT_LENGTH 1024
#define CLIENT_DATA_BUF_LENGTH  15 * 1024  //15KB
#define SERIAL_CHECK_TIME       20        //定义检测串口数据的时间间隔
struct serial_send_data{
	unsigned char *data;
	int len;
};
/* 串口结构体，用于存放一些串口连接的一些信息 */
struct serial_info{
	uint8_t serial_no;       //对应的串口号
	struct tcp_pcb *pcb;     //串口对应连接的PCB信息
	uint32_t send_data_total_length;   //串口发送数据的总长度
	uint32_t ackownaged_length;        //被对方确认的数据长度
	uint8_t is_send_over;   //串口是否有未完成的数据交互
};
struct serial_client{
	uint8_t *client_queue_data_buf[MAX_SERIAL_NUM];   //每一个串口对象对应一个发送buf
	struct serial_info serial_obj[MAX_SERIAL_NUM];    //存放所有的串口对象
};

//extern char client_queue_data_buf[CLIENT_DATA_BUF_LENGTH];

typedef struct serial_client SerialClientState;
void serialClientConnected(void);
void serialClientTimeout(void);
void serialClientAborted(void);
void serialClientClosed(void);
void serialClientInit(void);
int initSerialClient();
void initConnection(void);
/* 当客户端和服务器连接建立以后的回调函数 */
err_t processSerialClientConnection(void *arg, struct tcp_pcb *tpcb, err_t err);
/* 当客户端收到服务器发送的数据时调用的回调函数 */
err_t recvRemotePacket(void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err);

/* 根据远程主机的IP地址和端口号与远程主机建立连接
*/
unsigned char serialClientConnect(char *host, u16_t port,uint8_t serial_index);
/* 关闭打开的连接 */
void serialClientClose(void);  
void serialClientAppcall(void);
int readSerialData(uint8_t serial_no);//模拟读取串口的数据,每次读取1500长度的数据添加到队列里面

//int generateSerialClientSendData(struct serial_client *client_state);
int generateSerialClientSendData(uint8_t serial_index);
/* 存储建立连接的远端的串口服务器的信息,type是指存储的是UDP还是TCP socket
** 0代表是TCP 1代表的是UDP
 */
#if 0
int recordRemoteSerialServerSocket(uip_ipaddr_t remote_ip,uint16_t remote_port,uint16_t local_port,uint8_t type);
/* 是否是对应的远程串口服务器的响应 */
int isRemoteSerialServerResponse(uip_ipaddr_t remote_ip,uint16_t remote_port,uint8_t type);
/* 判断该远程的串口服务器信息是否存在,防止重新建立连接的过程发生 */
int isExistRemoteSerialServerSocket(uip_ipaddr_t remote_ip,uint16_t remote_port,uint8_t type);
#endif
#endif