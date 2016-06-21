#ifndef LOG_UTILITY_H
#define LOG_UTILITY_H
#include "LogInfo.h"
/* 生成新的日志项 */
uint8_t addLogEntry(uint8_t log_level,char *user,char*log_content);
/* 清空日志系统中所有的内容 */
uint8_t deleteAllLogInfo();

/* 打印当前日志信息 */
void printSystemLog();
/*格式化系统时间把系统时间结构体转换成字符串形式
** 转换的格式为 %YY-%MM-%DD %H:%m%s %w,把结构存储在str中
 */
uint8_t formatSystemTime(char*str,SYSTEM_TIME *system_time);

/* 更新日志系统中的使用默认的系统时间的日志项 */
uint8_t updateSystemLogTime(time_t start_time);
/* 修改日志时间内部调用 */
uint8_t modifySystemLogTime(SYSTEM_LOG *temp,time_t start_time);
/* 返回大于id的number数目的日志条数 */
uint8_t *getSystemLogEntry(int id,int number,int flag);
/* 返回小于id的number数目的日志条数 */
uint8_t *getPreSystemLogEntry(int id,int number);
#endif