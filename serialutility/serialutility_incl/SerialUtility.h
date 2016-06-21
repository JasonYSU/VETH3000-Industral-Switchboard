#ifndef SERIAL_UTILITY_H
#define SERIAL_UTILITY_H
#include "SerialQueue.h"
#define SERVER_QUEUE 0
#define CLIENT_QUEUE 1
#define SERIAL_QUEUE_MAX_MEMORY 1024 * 1024   //串口队列能使用的最大内存为1M

extern uint32_t serial_use_memory[MAX_SERIAL_NUM];
extern uint32_t serial_server_queue_total[4];
void initAllQueueArray();
/* 向队列数组中添加节点到队列中
** 
*/
void addDataToServerQueue(uint8_t *data,uint32_t data_len,uint8_t serial_no);

/* 客户端队列 */
void addDataToClientQueue(uint8_t *data,uint32_t data_len,uint8_t serial_no);

/* 从串口服务器队列中得到数据,需要一个char指针来接受返回值，返回值使用完之后
**要使用free函数释放空间。data_length为得到的数据长度
**
**
**
*/
char* getServerSerialData(uint8_t serial_no,uint32_t *data_length,uint32_t current_total_length,uint32_t max_length);

/* 从串口客户端队列中得到数据，注释参考上面的注释*/
char* getClientSerialData(uint8_t serial_no,uint32_t *data_length,uint32_t current_total_length,uint32_t max_length);

/* 得到当前对串口的配置 */
char *getSerialConfig(uint8_t serial_no);
int isSerialListenPort(uint16_t port);
int modifySerialConfig(char *form_content);
/* 删除配置的远端的项 */
int deleteSerialRemoteEntry(char *remote_ip,uint16_t remote_port,uint8_t serial_no);
/* 判断配置信息是否已经存在 */
int isExistRemoteConfig(char *remote_ip,uint16_t remote_port,uint8_t serial_no);
/* 添加配置到board结构体中 */
int addRemoteConfig(char *remote_ip,uint16_t remote_port,uint8_t serial_no);
/* 修改串口的远端配置,一个串口对应一个远程配置 */
int modifyRemoteConfig(char *remote_ip,uint16_t remote_port,uint8_t use_udp,uint8_t serial_no);
/* 修改本地的串口参数配置 */
int modifyLocalSerialConfig(uint8_t serial_no,uint16_t serial_mode_1_2,uint16_t serial_mode_3_4,uint32_t serial_bound_rate,
							uint8_t serial_data_bit,uint8_t serial_parity,float serial_stop_bit,
							uint16_t serial_listen_port);
							
/* 把客户端对应的数据读取到指定的buf中,返回值为实际读到的长度*/
int readClientQueueToBuf(char *buf,int length,uint8_t serial_no);
/* 串口相关的内存释放函数 */
void freeSerialMemory(void *ptr);
/* 检查是否是本地的串口之间的转发操作 */
uint8_t checkLocalSerialTransmit(char *remote_ip);
/* 本地串口之间的数据转发,根据串口监听的端口号实现本地串口数据之间的转发 */
uint8_t transmitLocalSerial(uint16_t listen_port);
#endif