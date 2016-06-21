#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
#include "board_info.h"
char *generateIPConfigData();
char *generateSystemStatusInfo();
char *generateSystemCompileDate(char*result);
char* calcSystemTime();
uint8_t calcSystemRunTime(SYSTEM_RUN_TIME *run_time);  //计算系统运行的时间
uint8_t getTimeWday(char* str,int day);  //计算某一天对应的星期
#endif