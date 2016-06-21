#ifndef MAC_TABLE_H
#define MAC_TABLE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
void initMacTableEntry(void);
/* 得到动态mac地址表项,flag用于决定mac比较时候是大于还是大于等于 */
char* getDyanMacTable(int start_vid,char *start_macAddr,int total_entry,int flag);
/* 根据start_macAddr得到上一页 的数据
** 参数 start_macAddr是要查找的mac地址最大值，显示该mac地址之前的数据
** total_entry 是要查询的数据总数
 */
char* getPrePageDynamicMac(int start_vid,char *start_macAddr,int total_entry);

/* 得到符合下一页的总项数 */
#endif
