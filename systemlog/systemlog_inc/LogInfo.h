#ifndef LOG_INFO_H
#define LOG_INFO_H
#include "board_info.h"
//#include<time.h>
#define MAX_LOG_MEMORY 1024 * 1024  //定义目前日志的最大内存使用量是1M
#define INFO  0    //日志的类型为信息
#define WARNING  1   //日志的类型为警告
#define ERROR   2    //日志的类型为错误

typedef struct system_log{
	uint32_t log_id;    //日志的ID
	uint8_t log_level;  //日志的等级
	uint8_t user[10];   //日志对应的登陆用户
	//char system_time[30];  //日志生成的时间
	SYSTEM_TIME log_time;   //日志生成的日期，结构体表示
	char *log_content;     //日志的内容，需要动态开辟
	struct system_log *next;
}SYSTEM_LOG;
extern uint32_t log_num;    //当前日志的总数
extern uint32_t log_use_total_memory;    //日志系统使用的总的内存
extern struct system_log *log_root;      //日志系统的根节点
 
/* 添加日志节点到日志系统中，采用尾部插入法，生成的日志一次往后插入到日志系统中
   插入之前应该先判断日志系统是否还有可用的空间
*/
struct system_log* addLogNode(struct system_log **root,struct system_log *node);
/* 删除日志的节点，删除从头部开始删除，头部的日志是最老的日志 */
struct system_log* deleteLogNode(struct system_log **root);
/* 清空当前的日志系统所有日志信息 */
uint8_t emptyLogSystem(struct system_log **root);

/* 管理系统日志使用的内存，释放老的日志占用的空间，根据第二个参数来删除root中的数据*/
uint32_t freeLogSystemMemory(struct system_log **root,uint32_t need_memory);

#endif