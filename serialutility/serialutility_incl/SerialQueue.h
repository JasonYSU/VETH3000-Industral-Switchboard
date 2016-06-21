#ifndef SERIAL_QUEUE_H
#define SERIAL_QUEUE_H
//#include<stdio.h>
//#include<stdlib.h>
//#include<string.h>
#include "board_info.h"
#define MAX_BUF_LENGTH 1500


struct queue_node{
	//char remote_ip[20]; //要连接的IP地址
	//uint16_t remote_port; //要连接的端口号
	//uint8_t serial_no;   //对应的串口号,可以作为主键确定所在的队列
	//uint8_t content_buf[MAX_BUF_LENGTH];  //存储要发送或者接收的数据
	uint8_t *content_buf;  //存储要发送或者接收的数据,改成指针的形式，根据content_length动态分配
	uint32_t content_length;
	struct queue_node *next;
};
/* 队列中每一个节点中存放的信息 */

struct serial_queue{
	struct queue_node *head;  //队列的头部
	struct queue_node *tail;  //队列的尾部
	//char queue_id;         //标识所属的队列，和serial_no的值相同
};
/* 每一个队列包括的信息 */

/* 串口服务器结构体数组中数据结构 */
struct server_queue_array{
	struct serial_queue *queue;  //队列指针
	uint8_t serial_no;           //队列属于哪个串口
	uint8_t empty_queue;        //标识该队列是否为空,只有为空时才能存储新的队列
};

/* 服务器作为客户端时，结构体数组中存放的信息 */
struct client_queue_array{
	struct serial_queue *queue;
	char remote_ip[20]; //要连接的IP地址
	uint16_t remote_port; //要连接的端口号
	uint8_t serial_no;     //队列对应的串口号
	uint8_t empty_queue;  //标识该队列是否为空,只有为空时才能存储新的队列
};

/* 用于存放串口服务器的所有队列,结构体中设置一个空队列位，用于判断该队列当前是否为空
** 数组的下标对应串口号 0 - 1，1 - 2，2 - 3,3 - 4。在取完队列的数据以后应该把对应数组中的标识设置为1.
*/
extern struct server_queue_array server_array_queue[MAX_QUEUE_NUM];

/* 用于存储串口服务器当作客户端时的所有队列,目前一个串口只能对应一个远程主机
**数组的下标对应串口号
*/
extern struct client_queue_array client_array_queue[MAX_QUEUE_NUM];


/* 初始化队列 */
void initQueue(struct serial_queue *queue);

/* 向队列中添加节点 */
struct serial_queue *addQueueNode(struct serial_queue *queue,struct queue_node *queue_node);

/* 从队列中得到数据 */
char *getQueueData(struct serial_queue *queue,uint32_t *data_length,uint32_t current_total_length,uint32_t max_length);

/* 删除所有的队列节点 */
struct serial_queue *removeAllQueueNode(uint8_t serial_no,struct serial_queue *queue);

#endif