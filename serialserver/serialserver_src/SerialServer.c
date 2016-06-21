/*******************************************************
File name: SerialServer.c
Description: 串口服务器的服务器模式的实现
Author: dujiancheng
Version: v1.0
********************************************************/
#include "SerialServer.h"
#include "SerialUtility.h"
#include "tcp.h"
#include "serialServerUdp.h"

/*串口服务器是通过监听的端口号和串口建立对应关系
**根据连接的端口号来决定要往哪个串口发送数据
**需要一个全局的数组来保存监听的端口号和串口的映射
**可以使用数组的下标对应串口号,数组里面存放对应的端口号
**/
//uint16_t serial_to_port[MAX_SERIAL_NUM];   //端口到串口的映射表
uint32_t serial_server_total_num;
uint32_t recv_total_length = 0;
uint32_t recv_total_data[4];
static uint8_t isExistZero(uint8_t *buf){
	int i;
	int is_all_zero = 1;

		for(i = 0; i < 50 ; i++){
			printf("%02x ",buf[i]);
		}
		printf("\n\r");
	//}
}
/* 串口服务器发送的数据被远程主机成功接收后的回调函数 */
static err_t serialServerSent(void *arg, struct tcp_pcb *pcb, u16_t len){

}
/**
 * Function: serialServerRecv
 * Description: 串口服务器接收到数据后的回调函数
 * @param arg 回调时传递回来的参数
 * @param pcb 当前连接的PCB
 * @param p   存放接收到数据的BUF
 * @param err  回调时传递回来的错误码
 * @return err_t 类型的状态码
**/
static err_t serialServerRecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err){
	uint8_t *recv_buf;
	struct pbuf *temp;
    err_t error_status;
	int recv_data_length;
    int loc = 0;
	int i;
    if ((err != ERR_OK) || (p == NULL)) {
        if(p != NULL) {
            tcp_recved(pcb, p->tot_len);
            pbuf_free(p);
        }
        return ERR_OK;
    }
    if(p == NULL) {
        printf("recv data is empty\n\r");
    } else {
        recv_buf = (uint8_t*)malloc(p->tot_len);
		//recv_total_length += p->tot_len;
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
    tcp_recved(pcb, p->tot_len);
	
    if(p != NULL) {
        pbuf_free(p);
    }
	for(i = 0; i < MAX_SERIAL_NUM; i++){
		if(board.serial_config[i].serial_listen_port == pcb->local_port){
			break;
		}
	}
	recv_total_data[i] += recv_data_length;
	addDataToServerQueue(recv_buf,recv_data_length,i);
	free(recv_buf);
	recv_buf = NULL;
}
/**
 * Function: serialServerRecv
 * Description: 串口服务器和远端主机成功建立连接后的回调函数
 * @param arg 回调时传递回来的参数
 * @param newpcb 当前连接的PCB
 * @param err  回调时传递回来的错误码
 * @return err_t 类型的状态码
**/
static err_t processSerialServerAccept(void *arg, struct tcp_pcb *newpcb, err_t err){
	struct tcp_pcb_listen *lpcb = (struct tcp_pcb_listen*)arg;

	if( err != ERR_OK){  //连接有误
		printf("serial server connection wrong\n\r");
		return ERR_MEM;
	}
	tcp_accepted(lpcb);
	//printf("connect client sucessful\n\r");
	//newpcb->flags &= ~TF_ACK_DELAY;
	tcp_setprio(newpcb,lpcb->prio);
	tcp_arg(newpcb, NULL);
	tcp_poll(newpcb, NULL, 2);
	tcp_recv(newpcb, serialServerRecv);
    tcp_sent(newpcb, serialServerSent);
	return ERR_OK;
}
/**
 * Function: deleteSerialServerListen
 * Description: 删除端口对应的串口服务器的监听连接
 * @param port 要删除的端口号
 * @return err_t 类型的状态码
**/
err_t deleteSerialServerListen(uint16_t port){
	struct tcp_pcb *cpcb;
	int i;
	/* 遍历所有的PCB查找相应的监听PCB */
	for (i = 0; i < NUM_TCP_PCB_LISTS; i++) {
        for(cpcb = *tcp_pcb_lists[i]; cpcb != NULL; cpcb = cpcb->next) {
            if (cpcb->local_port == port) {
                tcp_close(cpcb);
                return ERR_OK;
            }
        }
    }
}

/**
 * Function: addSerialServerListen
 * Description: 为本地的串口创建监听的连接，每一个串口对应一个监听连接
 * @param port 监听的端口号
 * @param prio 监听连接的优先级
 * @return err_t 类型的状态码
**/
err_t addSerialServerListen(uint16_t port,int prio){
	struct tcp_pcb *pcb;
    err_t err;
	pcb = tcp_new();
	if( !pcb){  //新建PCB失败
		printf("Allocate serial server PCB failed\n\r");
		return ERR_MEM;
	}
	tcp_setprio(pcb,prio);
	err = tcp_bind(pcb,IP_ADDR_ANY,port);
	if(err != ERR_OK){
		printf("bind port failed\n\r");
		return ERR_MEM;
	}
	pcb = tcp_listen(pcb);
	if( !pcb){  //监听连接失败
		printf("listen serial server PCB failed\n\r");
		return ERR_MEM;
	}
	tcp_arg(pcb, pcb);
	tcp_accept(pcb,processSerialServerAccept);
	return ERR_OK;
}
/**
 * Function: initDefaultSerialPort
 * Description: 初始化串口服务器的服务端的监听连接
**/
void initDefaultSerialPort(){
	int i;
	/* 添加端口到监听队列 */
	for(i = 0; i < MAX_SERIAL_NUM; i++){
		if(board.serial_config[i].serial_listen_port != 0){
			addSerialServerListen(board.serial_config[i].serial_listen_port,i + 1);//添加TCP的监听端口
			addUdpSerialListenPort(board.serial_config[i].serial_listen_port,i);  //添加UDP的监听端口
		}
	}
	
}
