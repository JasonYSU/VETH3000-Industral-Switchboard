#ifndef SERIAL_SERVER_H
#define SERIAL_SERVER_H
#include "board_info.h"
#include "tcp.h"
#define MAX_LENGTH_RECV 10 * 1500 //定义串口服务器最大的接收数据长度
#define STATE_WAITING 0
#define STATE_OUTPUT  1
#define SERVER_SERIAL_TIME 60 * 2   //定义时间戳为2s

typedef struct serial_server{
	uint32_t timer;
	char inputbuf[MAX_LENGTH_RECV];
	char state;
	int len;
	unsigned short count;
}SerialServerState;
extern uint32_t recv_total_data[4];
/* 串口服务器接收数据的时间间隔用于判断是否收到了完整的数据
** 如果在规定的时间内没有数据的接收,则视为数据为完整的数据
**/
extern int serial_server_time_out[MAX_SERIAL_NUM]; //每个串口队列一个计时器
extern uint32_t serial_server_total_num;
//extern uint16_t serial_to_port[MAX_SERIAL_NUM];

/* 添加serial server监听的连接,对每一个串口创建一个监听连接 */
err_t addSerialServerListen(uint16_t port,int prio);

/* 删除存在的串口监听的连接,在串口对应的监听端口号发生改变时被调用 */
err_t deleteSerialServerListen(uint16_t port);

/* 初始化默认的串口到端口的映射表 */
void initDefaultSerialPort();

/* 更新串口到端口的映射,通过用户的配置来调用 */
void updateSerialPort();

/* 判断是否是串口服务器的请求 */
int isSerialServerRequest(uint16_t request_port);

void handle_serial_connection(struct serial_server *serial);

char handle_serial_input(struct serial_server *serial);
#endif