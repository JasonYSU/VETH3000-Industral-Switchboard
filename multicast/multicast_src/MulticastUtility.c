/*******************************************************
File name: MulticastUtility.c
Description: 组播功能的实现
Author: dujiancheng
Version: v1.0
TODO：组播模块目前没有完成实现，只处于测试阶段，功能不完整，需要进行后续的开发
********************************************************/
#include "MulticastUtility.h"
#include "igmp.h"
struct udp_pcb *multicast_client_udp = NULL;
/**
 * Function: multicastServerRecv
 * Description: 组播服务器接收到数据后的回调函数
 * @param arg 回调时传递回来的参数
 * @param pcb 当前连接的PCB
 * @param p   存放接收到数据的BUF
 * @param addr 发送数据的IP地址
 * @param port 发送数据的端口号
**/
void multicastServerRecv(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port){
	printf("recv remote multicast data\n\r");
}
/**
 * Function: createMulticastListen
 * Description: 创建组播监听端
**/
void createMulticastListen(){
	struct udp_pcb *multicast_udp;
	ip_addr_t multicast_addr;
	err_t err_status;
	IP4_ADDR(&multicast_addr,224,1,1,210);
	multicast_udp = udp_new();
	if( !multicast_udp){
		printf("create multicast udp failed\n\r");
		return;
	}
	err_status = udp_bind(multicast_udp,&multicast_addr,6666);
	if(err_status != ERR_OK){
		printf("bind multicast udp failed\n\r");
		return;
	}
	igmp_joingroup(IP_ADDR_ANY,&multicast_addr);
	igmp_joingroup(&veth_netif.ip_addr,&multicast_addr);
	udp_recv(multicast_udp,multicastServerRecv,NULL);
}
/**
 * Function: createMulticastListen
 * Description: 创建组播的客户端模式
**/
void createMulticastClient(){
    int retValue = 0;
	err_t err_status;
    uint8_t i;
	struct pbuf *udp_data_packet;
	ip_addr_t ipaddr;//定义IP类型变量
	udp_data_packet = pbuf_alloc(PBUF_RAW,20,PBUF_RAM);
	if(!udp_data_packet){
		printf("serial udp memory allocate\n\r");
	}
	IP4_ADDR(&ipaddr,224,1,1,210);
	if(multicast_client_udp == NULL){
		multicast_client_udp = udp_new();
		if( !multicast_client_udp){
			printf("create udp pcb failed\n\r");
			return;
		}
		/* 先绑定udp链接 */
		err_status = udp_bind(multicast_client_udp,IP_ADDR_ANY,9675);    //绑定任意地址和1233端口
		if(err_status != ERR_OK){  //绑定失败
			printf("serial client udp bind failed\n\r");
			return ;
		}
		err_status = udp_connect(multicast_client_udp,&ipaddr,6666);   //与远程NTP服务器建立连接
		if(err_status != ERR_OK){  //创建连接失败
			printf("serial client udp create connect failed\n\r");
			return;
		}
		udp_recv(multicast_client_udp,multicastClientRecv,NULL);
	}
	
	memset(udp_data_packet->payload,0,udp_data_packet->len);
	memcpy(udp_data_packet->payload,"hello server",13);
	err_status = udp_send(multicast_client_udp,udp_data_packet);	
}
/**
 * Function: multicastClientRecv
 * Description: 组播客户端接收到数据后的回调函数
 * @param arg 回调时传递回来的参数
 * @param pcb 当前连接的PCB
 * @param p   存放接收到数据的BUF
 * @param addr 发送数据的IP地址
 * @param port 发送数据的端口号
**/
void multicastClientRecv(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port){
	printf("udp client recv data \n\r");
}