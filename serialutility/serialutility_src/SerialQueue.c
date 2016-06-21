/*******************************************************
File name: SerialQueue.c
Description: 串口服务器队列数据结构的实现
Author: dujiancheng
Version: v1.0
********************************************************/
#include "SerialQueue.h"
#include "SerialUtility.h"
struct server_queue_array server_array_queue[MAX_QUEUE_NUM];
struct client_queue_array client_array_queue[MAX_QUEUE_NUM];

/**
 * Function: initQueue
 * Description: 初始化一个队列，把队列的头指针和尾指针赋值为空
 * @param queue  指向队列结构体的指针，队列结构体中包括指向队列结点的头和尾的指针
**/
void initQueue(struct serial_queue *queue){
	queue->head = NULL;
	queue->tail = NULL;
}

/**
 * Function: addQueueNode
 * Description: 添加节点到队列中
 * @param queue  指向队列结构体的指针，队列结构体中包括指向队列结点的头和尾的指针
 * @param queue_node  添加的队列结点数据
 * @return 指向新队列的指针
**/
struct serial_queue *addQueueNode(struct serial_queue *queue,struct queue_node *queue_node){
	if(queue->head == NULL){  //空队列,初始化队列头和尾以及对应的串口号
		queue->head = queue->tail = queue_node;
	}else{
		queue->tail->next = queue_node;
		queue->tail = queue_node;
	}
	queue->tail->next = NULL;  //队列尾部next指针为空
	return queue;
}

/**
 * Function: getQueueData
 * Description: 从队列中得到数据,删除队列中的数据,如果应经读取的数据总长度 + 剩余的数据长度大于最大的数据长度
 *				则不读取数据返回NULL
 * @param queue  指向队列结构体的指针，队列结构体中包括指向队列结点的头和尾的指针
 * @param data_length  存放读取的实际长度
 * @param current_total_length 目前已经读取的总长度
 * @param max_length 需要读取的最大长度
 * @return 读取到的队列里面的数据
 * @ATTENTION 返回值使用的空间是动态生成的，调用者需要手动释放返回值空间
**/
char *getQueueData(struct serial_queue *queue,uint32_t *data_length,uint32_t current_total_length,uint32_t max_length){
	struct queue_node *temp = NULL;
	char *content = NULL;
	if(queue->head == NULL){
		return NULL;
	}else{
		if(current_total_length + queue->head->content_length > max_length){
			return NULL;
		}
		/*  应经读取的总长度 + 将要读取的数据长度 > 最大的总长度 则认为数据读取结束 */
		content = (char*)malloc(queue->head->content_length);
		if( !content){
			printf("get queue data malloc failed\n\r");
			return NULL;
		}
		memset(content,0,queue->head->content_length);
		memcpy(content,queue->head->content_buf,queue->head->content_length);
		*data_length = queue->head->content_length;
		temp = queue->head;
		queue->head = queue->head->next;
		if(queue->head == NULL){  //队列中只有一个数据
			queue->tail = queue->head;
		}
		free(temp->content_buf);
		temp->content_buf = NULL;
		free(temp);
		temp = NULL;
	}
	return content;
	
}
/*  */
/**
 * Function: removeAllQueueNode
 * Description: 删除所有的队列节点
 * @param serial_no 队列所属于的串口号
 * @param queue  指向队列结构体的指针，队列结构体中包括指向队列结点的头和尾的指针
 * @return 删除后的队列指针
**/
struct serial_queue *removeAllQueueNode(uint8_t serial_no,struct serial_queue *queue){
	struct queue_node *p = NULL;
	p = queue->head;
	while(p){
		queue->head = queue->head->next;
		/* 删除节点，修改对应的内存使用量 */
		serial_use_memory[serial_no] -= p->content_length;
		free(p->content_buf);
		free(p);
		p = queue->head;
	}
	queue->tail = queue->head;
	return queue;
}

