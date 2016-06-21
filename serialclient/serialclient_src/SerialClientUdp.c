/*******************************************************
File name: SerialClientUdp.c
Description: 串口客户端UDP模式的实现
Author: dujiancheng
Version: v1.0
********************************************************/
#include "SerialClientUdp.h"
#include "SerialClient.h"
#include "SerialUtility.h"
#include "ip_config.h"
#include "udp.h"
#include "pbuf.h"
#define UDP_MSS 1024
#define SERIAL_UDP_PORT 20512
struct serial_client serial_udp_client;
REMOTE_UDP_SOCKET remote_serial_udp_socket[MAX_SERIAL_REMOTE_NUM];

static void serialClientRecv(void *arg, struct udp_pcb *pcb, struct pbuf *p,ip_addr_t *addr, u16_t port){
	
}
/**
 * Function: senddata
 * Description: 串口客户端UDP发送数据的接口函数
 * @param conn 当前UDP的连接
 * @param serial_no UDP连接对应的串口号
 * @param data_length 要发送的数据长度
**/
uint8_t senddata(struct udp_pcb *conn,uint8_t serial_no,uint32_t data_length) {
    uint32_t send_length = 0;
	struct pbuf *udp_data_packet;
	uint32_t loc = 0;
	err_t err_status;
	do {
		
		if(data_length >= UDP_MSS) {
            send_length = UDP_MSS;
        } else {
            send_length = data_length;
        }
		udp_data_packet = pbuf_alloc(PBUF_RAW,send_length,PBUF_RAM);
		if(!udp_data_packet){
			printf("serial udp memory allocate\n\r");
			return 0;
		}
        memset(udp_data_packet->payload,0,send_length);
		memcpy(udp_data_packet->payload,&serial_client.client_queue_data_buf[serial_no][loc],send_length);
		err_status = udp_send(conn,udp_data_packet);
		loc += send_length;
        data_length -= send_length;
		if(udp_data_packet){
			pbuf_free(udp_data_packet);
			udp_data_packet = NULL;
		}
    } while(data_length > 0);
	if(serial_client.client_queue_data_buf[serial_no]){
		free(serial_client.client_queue_data_buf[serial_no]);
		serial_client.client_queue_data_buf[serial_no] = NULL;
	}
	return 1;
}
/**
 * Function: serialClientUdpConnect
 * Description: 串口客户端创建UDP连接的接口函数
 * @param host 连接的远程主机IP地址
 * @param port 远程主机的端口号
 * @param serial_no 该连接对应的串口号
 * @return 连接成功返回连接的PCB
 *         创建连接失败返回NULL
**/
struct udp_pcb * serialClientUdpConnect(char *host, uint16_t port,uint8_t serial_no) {
	struct udp_pcb *serial_udp;
    int retValue = 0;
	err_t err_status;
    uint8_t i;
	uint8_t format_ip[4];
    ip_addr_t ipaddr;//定义IP类型变量
	//printf("remote ip is %s,port is %d\n\r",host,port);
	getFormatAddress(format_ip,host);
	IP4_ADDR(&ipaddr,format_ip[0],format_ip[1],format_ip[2],format_ip[3]);
	serial_udp = udp_new();
	if( !serial_udp){
		printf("create udp pcb failed\n\r");
		return NULL;
	}
	/* 先绑定udp链接 */
	err_status = udp_bind(serial_udp,IP_ADDR_ANY,SERIAL_UDP_PORT + serial_no);    //绑定任意地址和1233端口
	if(err_status != ERR_OK){  //绑定失败
		printf("serial client udp bind failed\n\r");
		return NULL;
	}
	err_status = udp_connect(serial_udp,&ipaddr,port);   //与远程NTP服务器建立连接
	if(err_status != ERR_OK){  //创建连接失败
		printf("serial client udp create connect failed\n\r");
		return NULL;
	}
    return serial_udp;
}
/**
 * Function: processSerialClientUdpConnection
 * Description: 串口客户端建立UDP连接的接口函数，需要建立UDP连接时该函数将被调用，函数通过调用serialClientUdpConnect
 *				函数来创建UDP连接，之后调用数据发送函数。最后关闭UDP连接
 * @param host 连接的远程主机IP地址
 * @param port 远程主机的端口号
 * @param serial_index 该连接对应的串口号
**/
void processSerialClientUdpConnection(char *host, uint16_t port,uint8_t serial_index) {
	struct udp_pcb *serial_udp;
	int send_length;
	serial_udp = serialClientUdpConnect(host,port,serial_index);
	send_length = handleSerialClientUdpOutput(serial_index);
	if(send_length > 0){
		senddata(serial_udp,serial_index,send_length);
		//printf("serial data send over\n\r");
	}
	
	udp_remove(serial_udp);
}
/**
 * Function: handleSerialClientUdpOutput
 * Description: 生成UDP要发送的数据
 * @param serial_no 串口号
 * @return 生成的数据长度
**/
uint32_t handleSerialClientUdpOutput(uint8_t serial_no) {
    int length = generateSerialClientSendData(serial_no);   //调用数据生成函数
	return length;
}