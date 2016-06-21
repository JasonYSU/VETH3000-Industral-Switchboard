/*******************************************************
File name: SerialClient.c
Description: 串口服务器的客户端模式的实现
Author: dujiancheng
Version: v1.0
********************************************************/
#include "SerialClient.h"
#include "ip_config.h"
#include "tcp.h"
#include "tcp_impl.h"
#include "SerialUtility.h"
#include<stdio.h>
#include<string.h>
#define SERIALCLIENT_TIMEOUT 100

#define SERIALCLIENT_STATE_STATUSLINE 0
//#define SERIALCLIENT_STATE_HEADERS    1
#define SERIALCLIENT_STATE_DATA       2
#define SERIALCLIENT_STATE_CLOSE      3
#define SERIALCLIENT_STATE_SEND_END   4
#define SERIAL_CLIENT_PORT            7000

struct serial_client serial_client;
static int current_serial_no;
/**
 * Function: initSerialClient
 * Description: 初始化串口客户端连接
**/
int initSerialClient(){
	int i;
	for(i = 0; i < MAX_SERIAL_NUM; i++){
		serial_client.serial_obj[i].pcb = NULL;
		serial_client.serial_obj[i].serial_no = 200;
	}
}
/**
 * Function: macthSerialPCB
 * Description: 从所有的串口客户端连接中中找到和当前一样的PCB,返回对应的串口号
 * @param src_pcb 当前连接的PCB
 * @return 连接存在返回该连接对应的串口号
 *		   -1 连接不存在
**/
static int macthSerialPCB(struct tcp_pcb *src_pcb){
	int i;
	struct tcp_pcb *serial_pcb = NULL;
	
	for(i = 0; i < MAX_SERIAL_NUM;i++){
		serial_pcb = serial_client.serial_obj[i].pcb;
		if(!serial_pcb){
			continue;
		}
		if ((serial_pcb->local_port == src_pcb->local_port) &&
                    (serial_pcb->remote_port ==  src_pcb->remote_port) &&
                    ip_addr_cmp(&serial_pcb->local_ip, &src_pcb->local_ip) &&
                    ip_addr_cmp(&serial_pcb->remote_ip,&src_pcb->remote_ip)) {
                /* 在所有的串口结构中找到了匹配的PCB */
                return serial_client.serial_obj[i].serial_no;
         }
	}
	//没有找到相匹配的PCB，有错误发生
	return -1;
}
/**
 * Function: serialPoll
 * Description: 当串口客户端连接处于轮询状态时的回调函数
 * @param arg 回调时传递回来的参数
 * @param tpcb 当前连接的PCB
 * @TODO 该函数目前没有做实际的工作
**/
err_t serialPoll(void *arg, struct tcp_pcb *tpcb) {
    if(tpcb->state == SYN_SENT) { //发送SYN失败
        //printf("send SYN again\n\r");
    }
    //printf("serial poll\n\r");
}
/**
 * Function: processSerialClientConnection
 * Description: 当serial客户端和服务器的连接建立完成后的回调函数
 * @param arg 回调时传递回来的参数
 * @param tpcb 当前连接的PCB
 * @param err  回调时传递回来的错误码
 * @return err_t 类型的状态码
**/
err_t processSerialClientConnection(void *arg, struct tcp_pcb *tpcb, err_t err) {
    int serial_index;
	int len = 0;
	err_t error_status;
    if(err != ERR_OK) { //连接有误
        return ERR_OK;
    }
	serial_index = macthSerialPCB(tpcb);
	if(serial_index < 0){  //该连接没有对应的串口
		return ERR_VAL;
	}
	/* 读取该串口队列上的数据 */
	len = generateSerialClientSendData(serial_index);
	serial_client.serial_obj[serial_index].send_data_total_length = len;
	serial_client.serial_obj[serial_index].ackownaged_length = 0;
    error_status = tcp_write(tpcb,serial_client.client_queue_data_buf[serial_index],len,1);
    if(error_status != ERR_OK) { //发送数据失败
        printf("data send failed,error status is %d\n\r",error_status);
        return ERR_MEM;
    }
    tcp_output(tpcb);
	serial_client.serial_obj[serial_index].is_send_over = 0;
	if(serial_client.client_queue_data_buf[serial_index]){  //有空间没有被释放
		free(serial_client.client_queue_data_buf[serial_index]);
		serial_client.client_queue_data_buf[serial_index] = NULL;
	}
    return ERR_OK;
}
/**
 * Function: processSerialClientConnection
 * Description: 发送的数据被远程主机确认后的回调函数
 * @param arg 回调时传递回来的参数
 * @param tpcb 当前连接的PCB
 * @param len  被确认的数据的长度
 * @return err_t 类型的状态码
**/
err_t dataAcknowaged(void *arg, struct tcp_pcb *tpcb,u16_t len){
	int serial_index;
	if(len == 0){ //没有数据被确认
		return ERR_OK;
	}
	serial_index = macthSerialPCB(tpcb);
	if(serial_index < 0){  //该连接没有对应的串口
		return ERR_VAL;
	}
	/* 没有需要确认的数据，直接返回 */
	if(serial_client.serial_obj[serial_index].send_data_total_length == 0){
		return ERR_OK;
	}
	if(len != serial_client.serial_obj[serial_index].send_data_total_length ){
		serial_client.serial_obj[serial_index].ackownaged_length += len;
		//printf("has outstanding data;len is %d\n\r",serial_client.serial_obj[serial_index].send_data_total_length - len);
	}else{
		//printf("all data has ackonwaged,len is %d\n\r",serial_client.serial_obj[serial_index].ackownaged_length);
		serial_client.serial_obj[serial_index].ackownaged_length = 0;
		serial_client.serial_obj[serial_index].send_data_total_length = 0;
		serial_client.serial_obj[serial_index].is_send_over = 1; //置发送结束标志为true
	}
	
}
/**
 * Function: recvRemotePacket
 * Description: 当客户端收到服务器发送的数据时调用的回调函数
 * @param arg 回调时传递回来的参数
 * @param tpcb 当前连接的PCB
 * @param p  存放服务器发送过来的数据
 * @param err 回调时传递回来的错误码
 * @return err_t 类型的状态码
**/
err_t recvRemotePacket(void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err) {
    struct pbuf *temp;
    char *data;
    err_t error_status;
    int loc = 0;
    if ((err != ERR_OK) || (p == NULL)) {
        if(p != NULL) {
            tcp_recved(tpcb, p->tot_len);
            pbuf_free(p);
        }
        return ERR_OK;
    }
    if(p == NULL) {
        printf("recv data is empty\n\r");
    } else {
        data = (char*)malloc(p->tot_len);
        memset(data,0,p->tot_len);
        memset(data,0,p->tot_len);
        for(temp = p; temp ; temp = temp->next) {
            memcpy(&data[loc],temp->payload,temp->len);
            loc += temp->len;
        }
    }
    tcp_recved(tpcb, p->tot_len);

    if(p != NULL) {
        pbuf_free(p);
    }

    error_status = tcp_write(tpcb,data,p->len,1);
    if(error_status != ERR_OK) { //发送数据失败
        printf("data send failed\n\r");
        return ERR_MEM;
    }
	tcp_output(tpcb);
    free(data);
    data = NULL;
}
/**
 * Function: checkConnectionExist
 * Description: 检测要建立的连接是否已经存在，如果存在则直接返回存在的连接
 * @param local_ip 客户端的IP地址
 * @param local_port 客户端的端口号
 * @param remote_ip  连接的远程主机的IP地址
 * @param remote_port 远程主机的端口号
 * @return 连接存在返回连接的PCB
 *         连接不存在返回NULL
**/
static struct tcp_pcb* checkConnectionExist(ip_addr_t *local_ip,u16_t local_port,ip_addr_t remote_ip,u16_t remote_port) {
    struct tcp_pcb *cpcb;
    int i;
    ip_addr_t temp_ip;
    if (ip_addr_isany(local_ip)) {
        /* no local IP address set, yet. */
        struct netif *netif = ip_route(local_ip);
        if (netif == NULL) {
            return NULL;
        }
        /* Use the netif's IP address as local address. */
        ip_addr_copy(temp_ip, netif->ip_addr);
    } else {
        ip_addr_copy(temp_ip, *local_ip);
    }

    /* Don't check listen- and bound-PCBs, check active- and TIME-WAIT PCBs. */
    for (i = 0; i < NUM_TCP_PCB_LISTS; i++) {
        for(cpcb = *tcp_pcb_lists[i]; cpcb != NULL; cpcb = cpcb->next) {
            if (cpcb->local_port == local_port) {
                if((cpcb->remote_port == remote_port) &&
                    ip_addr_cmp(&cpcb->local_ip, &temp_ip) &&
                    ip_addr_cmp(&cpcb->remote_ip,&remote_ip)) {
                return cpcb;
            }
			}
        }
    }
    return NULL;
}
/**
 * Function: serialClientConnect
 * Description: 串口客户端创建连接的接口函数，创建连接之前先判断相应的连接是否已经存在，如果存在则直接发送数据
 *				否则创建一个新的连接
 * @param host 要建立连接的远程主机号
 * @param port 要建立连接的远程主机端口号
 * @param serial_index  连接对应的串口号
 * @return 0 连接处于SYN状态等待即可或者连接不存在创建新连接成功，连接处于SYN状态，或者连接已经存在直接发送数据
 *         1 创建连接失败
**/
unsigned char serialClientConnect(char *host, u16_t port,uint8_t serial_index) {
    ip_addr_t ipaddr;//定义IP类型变量
    err_t error_status;
    struct tcp_seg *rseg;
    u32_t iss;
    struct tcp_pcb *serial_client_pcb = NULL;
    char temp_ip[40];
    uint8_t format_ip[4];
    memset(temp_ip,0,sizeof(temp_ip));
    strcpy(temp_ip,host);
    getFormatAddress(format_ip,temp_ip);
    IP4_ADDR(&ipaddr,format_ip[0],format_ip[1],format_ip[2],format_ip[3]);
    serial_client_pcb = checkConnectionExist(IP_ADDR_ANY,SERIAL_CLIENT_PORT + port,ipaddr,port);
    /* 先检测连接池中是否存在对应的连接，如果有则不需要建立连接直接发送数据即可 */
    if(serial_client_pcb != NULL) {
        if(serial_client_pcb->state == SYN_SENT){   //连接处于SYN状态，等待连接的建立，直接返回
			return 0;
		}else if(serial_client_pcb->state != ESTABLISHED ) { //PCB状态不为连接状态，则视为连接不可用，删除连接从新建立
            serial_client.serial_obj[serial_index].pcb = NULL;
			tcp_close(serial_client_pcb);
            serial_client_pcb = NULL;
        } else {
            serial_client_pcb->state = ESTABLISHED;
			
            processSerialClientConnection(NULL,serial_client_pcb,ERR_OK);
            return 0;
        }
    }
    serial_client_pcb = tcp_new();
    if( !serial_client_pcb) {
        printf("create serial client PCB failed\n\r");
        return 1;
    } 
    /*绑定本地IP为任意IP地址，设置本地端口为0是为了让协议栈自动分配一个可用的端口*/
    error_status = tcp_bind(serial_client_pcb,IP_ADDR_ANY,SERIAL_CLIENT_PORT + port);
    if(error_status != ERR_OK) { //绑定失败
        printf("bind local IP address failed,status is %d\n\r",error_status);
        return 1;
    }
    error_status = tcp_connect(serial_client_pcb,&ipaddr,port,processSerialClientConnection);
    if(error_status != ERR_OK) { //创建连接失败，由于内存不足导致不能加入到SYN队列中
        printf("create connection failed,can't put SYN queue\n\r");
        return 1;
    } else {
        serial_client.serial_obj[serial_index].pcb = serial_client_pcb;
		serial_client.serial_obj[serial_index].is_send_over = 1;
		serial_client.serial_obj[serial_index].serial_no = serial_index;
		serial_client.serial_obj[serial_index].ackownaged_length = 0;
		serial_client.serial_obj[serial_index].send_data_total_length = 0;
    }
    tcp_recv(serial_client_pcb,recvRemotePacket);  //设定当收到服务器发来的数据时的回调函数
	tcp_sent(serial_client_pcb,dataAcknowaged);   //数据被远程主机确认后的回调函数
    tcp_poll(serial_client_pcb,serialPoll,2);
    return 0;
}
/**
 * Function: generateSerialClientSendData
 * Description: 生成客户端要发送的数据
 * @param serial_index 连接对应的串口号
 * @return 生成的数据长度
**/
int generateSerialClientSendData(uint8_t serial_index){
	uint32_t read_length = 0;
    serial_client.client_queue_data_buf[serial_index] = (uint8_t*)malloc(CLIENT_DATA_BUF_LENGTH * sizeof(char));
	if( serial_client.client_queue_data_buf[serial_index] == NULL) {
        printf("get client queue data malloc failed\n\r");
        return 0;
    }
    memset( serial_client.client_queue_data_buf[serial_index],0,sizeof(CLIENT_DATA_BUF_LENGTH));
    read_length = readClientQueueToBuf((char*) serial_client.client_queue_data_buf[serial_index],CLIENT_DATA_BUF_LENGTH,serial_index);//读取指定长度的数据到缓冲区中
    return read_length;
}
