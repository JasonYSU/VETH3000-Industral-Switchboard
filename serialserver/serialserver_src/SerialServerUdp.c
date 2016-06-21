/*******************************************************
File name: SerialServerUdp.c
Description: 串口服务器的服务器模式UDP方式的实现
Author: dujiancheng
Version: v1.0
********************************************************/
#include "SerialServerUdp.h"
#include "SerialUtility.h"
#include "udp.h"
struct udp_serial_server serial_server[MAX_SERIAL_NUM];
uint32_t udp_serv_recv[4];
static char send_data(char *content,uint32_t data_len) {
    /* 在发送udp数据前 重置数据发送的指针 */
    //uip_send(content,data_len);
    //printf("udp send data is\n\r %s\n\r",uip_sappdata);
    return 1;
}
/**
 * Function: serialServerRecv
 * Description: 串口服务器接收到数据后的回调函数
 * @param arg 回调时传递回来的参数
 * @param pcb 当前连接的PCB
 * @param p   存放接收到数据的BUF
 * @param addr 发送数据的IP地址
 * @param port 发送数据的端口号
 * @param err  回调时传递回来的错误码
**/
void serialServerRecv(void *arg, struct udp_pcb *pcb, struct pbuf *p,
    ip_addr_t *addr, u16_t port){
	uint8_t *recv_buf;
	struct pbuf *temp;
    err_t error_status;
	int recv_data_length;
    int loc = 0;
	int i;
    if(p == NULL) {
        printf("recv data is empty\n\r");
    } else {
        recv_buf = (uint8_t*)malloc(p->tot_len);
        memset(recv_buf,0,p->tot_len);
		recv_data_length = p->tot_len;
        if(p->tot_len == p->len){  //p节点的后面没有buf数据
			memcpy(recv_buf,p->payload,p->tot_len);
		}else{
			for(temp = p; temp ; temp = temp->next) {
				memcpy(&recv_buf[loc],temp->payload,temp->len);
				loc += temp->len;
			}
		}
        
    }

    if(p != NULL) {
        pbuf_free(p);
    }
	for(i = 0; i < MAX_SERIAL_NUM; i++){
		if(board.serial_config[i].serial_listen_port == pcb->local_port){
			break;
		}
	}
	udp_serv_recv[i] += recv_data_length;
	addDataToServerQueue(recv_buf,recv_data_length,i);
	free(recv_buf);
	recv_buf = NULL;
}
/**
 * Function: addUdpSerialListenPort
 * Description: 创建UDP的监听连接
 * @param port_no 监听的端口号
 * @param serial_no 监听连接对应的串口号
 * @return err_t 类型的状态码
**/
err_t addUdpSerialListenPort(uint16_t port_no,uint8_t serial_no) {
    int i;
	err_t err_status;
	struct udp_pcb *server_udp;
	server_udp = udp_new();
	if( !server_udp){
		printf("create udp pcb failed\n\r");
		return ERR_VAL;
	}
	/* 先绑定udp链接 */
	err_status = udp_bind(server_udp,IP_ADDR_ANY,port_no); 
	if(err_status != ERR_OK){  //绑定失败
		printf("ntp udp bind failed\n\r");
		return ERR_VAL;
	}
	serial_server[serial_no].serial_no = serial_no;
	serial_server[serial_no].udp_pcb = server_udp;
	udp_recv(server_udp,serialServerRecv,&serial_no);
}
/**
 * Function: unlistenUdpSerialPort
 * Description: 取消UDP对应的监听连接
 * @param port_no 监听的端口号
 * @param serial_no 监听连接对应的串口号
**/
void unlistenUdpSerialPort(uint16_t port_no,uint8_t serial_no) {
    int i;
	struct udp_pcb *ipcb;
	for (ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb->next){
		if(ipcb->local_port == port_no){   //找到对应的连接，删除
			udp_remove(ipcb);
			serial_server[serial_no].udp_pcb = NULL;
		}
	}
}
