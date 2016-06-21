/*******************************************************
File name: LogUtility.c
Description: 实现并提供了操作日志系统的相关的接口函数
Author: dujiancheng
Version: v1.0
********************************************************/
#include"LogUtility.h"
#include "system_info.h"
#include<time.h>
#define YEAR_SECOND (365 * 24 * 3600)  //定义一年的总秒数
#define MONTH_SECOND ()           //定义一月的总秒数
//#include<time.h>
/*  */
/**
 * Function: saveSystemLogTime
 * Description:获取系统当前时间保存时间到日志结构体中
 * @param log_time  系统时间结构体指针
 * @return 1 函数正常执行
**/
static uint8_t saveSystemLogTime(SYSTEM_TIME *log_time){
	struct tm *log_tm;
	log_time->system_sec = system_time.system_sec;
	log_tm = localtime(&log_time->system_sec);
	log_time->year = log_tm->tm_year + 1900;
	log_time->month = log_tm->tm_mon + 1;
	log_time->day = log_tm->tm_mday;
	log_time->hour = log_tm->tm_hour;
	log_time->minute = log_tm->tm_min;
	log_time->second = log_tm->tm_sec;
	log_time->week = log_tm->tm_wday;
//	strcpy((char*)log_time->weekday,(char*)system_time.weekday);
	getTimeWday((char*)log_time->weekday,log_time->week);
	return 1;
}
/*  */
/**
 * Function: saveSystemLogTime
 * Description:生成新的日志项，对外的接口函数，调用该函数会向日志系统中添加一条日志信息，函数内部会自动为日志
 *			   添加当前的系统时间
 * @param log_level  添加的日志等级，目前的等级分为INFO，ERROR和WARING
 * @param user       日志所属于的用户
 * @param log_content 添加的日志内容
 * @return 1 函数正常执行
**/
uint8_t addLogEntry(uint8_t log_level,char *user,char*log_content){
	SYSTEM_LOG *temp;
	char *system_time = NULL;
	uint32_t use_memory_size = 0; //新添加的日志项所需要的内存大小
	uint32_t need_memory_size = 0; //需要的日志系统释放的内存大小
	/* 先为新增加的记录分配空间，以此来判断需要释放日志系统的空间 */
	temp = (SYSTEM_LOG *)malloc(sizeof(SYSTEM_LOG));
	if(temp == NULL){
		printf("malloc failed:addLogEntry_temp\n\r");
		return 0;
	}
	temp->log_content = (char*)malloc(strlen(log_content) + 1);
	if( !temp->log_content){
		printf("malloc failed:addLogEntry_temp->log_content\n\r");
		return 0;
	}
	memset(temp->log_content,0,strlen(log_content) + 1);
	strcpy(temp->log_content,log_content);
    /* 设置日志的内容 */

	log_num++;
	temp->log_id = log_num;
	/* 设置日志的ID号 */

	temp->log_level = log_level;
	/* 设置日志的等级 */

	system_time = calcSystemTime();   //获取系统的时间
//	memset(temp->system_time,0,sizeof(temp->system_time));
//	strcpy(temp->system_time,system_time);
	free(system_time);
	/* 保存系统时间到日志时间结构体中 */
	saveSystemLogTime(&temp->log_time);
	system_time = NULL;
	/* 设置日志的系统时间 */

	memset(temp->user,0,sizeof(temp->user));
	strcpy((char*)temp->user,user);
	/* 设置日志所属的用户 */
	
	use_memory_size = sizeof(*temp) + strlen(temp->log_content) + 1;
	//printf("need memory size is %d\n",use_memory_size);
	/* 计算本条日志所需要的内存空间 */

	/* 先判断日志系统是否还有剩余的空间可用 */
	if(log_use_total_memory + use_memory_size >= MAX_LOG_MEMORY){  //没有可用的空间，释放部分空间,预判断
		//printf("system log avaliable memory not enough for log %d,total size is %d\n",temp->log_id,log_use_total_memory);
		need_memory_size = log_use_total_memory - (MAX_LOG_MEMORY - use_memory_size);
		freeLogSystemMemory(&log_root,need_memory_size);
	}
	addLogNode(&log_root,temp);
	log_use_total_memory += use_memory_size;
	//printf("current log system use memory size is %d\n",log_use_total_memory);
	return 0;
}
/*  */
/**
 * Function: deleteAllLogInfo
 * Description:清空日志系统中所有的内容，对外的接口函数，函数调用日志系统内部函数emptyLogSystem来执行清除操作
**/
uint8_t deleteAllLogInfo(){
	return emptyLogSystem(&log_root);
}
/**
 * Function: printSystemLog
 * Description:打印目前日志系统的所有日志项，可以作为测试方法使用
**/
void printSystemLog(){
	#if 0
	SYSTEM_LOG *temp;
	char log_type[10];
	char log_time[32];
	temp = log_root;
	printf("-------------------------------------system log--------------------------------\n\r");
	printf("ID   LEVEL        TIME                 CONTENT                            USER\n\r");
	while(temp){
		switch(temp->log_level){
			case 0:
				strcpy(log_type,"info");
				break;
			case 1:
				strcpy(log_type,"warning");
				break;
			case 2:
				strcpy(log_type,"error");
				break;
			default:
				strcpy(log_type,"other");
		}
		memset(log_time,0,sizeof(log_time));
		formatSystemTime(log_time,&temp->log_time);
		//strftime(log_time,32,"%Y-%m-%d %H:%M:%S %A",&temp->log_time);
		printf("%d   %8s   %8s   %8s   %8s\n\r",temp->log_id,log_type,log_time,temp->log_content,temp->user);
		temp = temp->next;
	}
	printf("----------------------------------------end----------------------------------------\n\r");
	#endif
}
/**
 * Function: formatSystemTime
 * Description:格式化系统日期时间，把存储的系统格式的日期格式化可读的日期形式
 * @param str 存放格式化后的日期时间
 * @param system_time 被格式化的系统时间
 * @TODO 目前格式化只能格式成YY-MM-DD HH-mm-SS W的格式，以后可以考虑增加自定义格式的形式进行扩展
**/
uint8_t formatSystemTime(char*str,SYSTEM_TIME *system_time){
	sprintf(str,"%d-%02d-%02d %02d:%02d:%02d %s",system_time->year,system_time->month,system_time->day,
			system_time->hour,system_time->minute,system_time->second,system_time->weekday);
	return 1;
}
/**
 * Function: updateSystemLogTime
 * Description:修改日志的时间
 * @param start_time 系统的启动时间
**/
uint8_t updateSystemLogTime(time_t start_time){
	SYSTEM_LOG *temp;
	temp = log_root;
	while(temp){
		modifySystemLogTime(temp,start_time);
		temp = temp->next;
	}
	return 1;
}
/*  */
/**
 * Function: modifySystemLogTime
 * Description:更新日志系统中的使用默认的系统时间的日志项，该函数被updateSystemLogTime函数调用
 * @param temp 指向某一个日志项的指针
 * @param start_time 系统的启动时间
**/
uint8_t modifySystemLogTime(SYSTEM_LOG *temp,time_t start_time){
	time_t total_sec = 0;
	time_t log_time_sec = 0;  //日志生成时的对应的时间秒数
	struct tm *log_tm;
	/* 根据生成日志的时间秒数和系统启动时间来计算日志生成日志的偏移量 */
	//log_time_sec = total_sec +  start_time;
	log_time_sec = temp->log_time.system_sec +  start_time;
	log_tm = localtime(&log_time_sec);
	//printf("update log time is %s\n\r",ctime(&log_time_sec));
	temp->log_time.year = log_tm->tm_year + 1900;
	temp->log_time.month = log_tm->tm_mon + 1;
	temp->log_time.day = log_tm->tm_mday;
	temp->log_time.hour = log_tm->tm_hour;
	temp->log_time.minute = log_tm->tm_min;
	temp->log_time.second = log_tm->tm_sec;
	temp->log_time.week = log_tm->tm_wday;
	getTimeWday((char*)temp->log_time.weekday,temp->log_time.week);
	return 1;
}
/**
 * Function: getSystemLogEntry
 * Description:返回大于id的number数目的日志项，并把这些日志项格式化后发送给浏览器
 * @param id 最小的日志ID
 * @param number 查找的日志总数目
 * @param flag 请求的标志 为1说明请求下一页数据，不包含起始项,为 0说明是刷新请求应该包含起始项
 * @return 格式化以后要发给浏览器的数据
**/
uint8_t *getSystemLogEntry(int id,int number,int flag){
	int i;
	int found_num = 0;  //找到的总条数
	int first_id;      
	int last_id;
	static char all_data[4096];  //定义4K的缓冲区
	char single_entry[256];   
	char header[10];
	char first_last[10];
	char log_time[32];
	char level[10];
	SYSTEM_LOG *temp;
	memset(all_data,0,sizeof(all_data));
	memset(header,0,sizeof(header));
	memset(first_last,0,sizeof(first_last));
	sprintf(header,"%d/%d|",number,id);
	strcat(all_data,header);
	for(temp = log_root; temp; temp = temp->next){
		memset(single_entry,0,sizeof(single_entry));
		//flag为1说明请求下一页数据，不包含起始项,为 0说明是刷新请求应该包含起始项
		if(flag ?(temp->log_id <= id) : (temp->log_id < id)){  //当前ID小于起始ID 继续查找
			continue;
		}
		if(found_num == 0){  //找到第一个项
			first_id = temp->log_id;
		}
		memset(log_time,0,sizeof(log_time));
		formatSystemTime(log_time,&temp->log_time);
		switch(temp->log_level){
			case 0:
				strcpy(level,"信息");
				break;
			case 1:
				strcpy(level,"警告");
				break;
			case 2:
				strcpy(level,"错误");
				break;
		}
		sprintf(single_entry,"%s/%d/%s/%s/%s|",level,temp->log_id,log_time,temp->log_content,temp->user);
		strcat(all_data,single_entry);
		found_num++;
		last_id = temp->log_id;  //确定是否是最后一个选项
		if(found_num == number){    //找到了所有的选项
			break;
		}
	}
	if(found_num == 0 ){   //没有找到符合条件的选项
		sprintf(single_entry,"NoEntries/-/-/-|");
		strcat(all_data,single_entry);
		first_id = 1;
		last_id = log_num;
	}
	first_id = (first_id == 1) ? 1 : 0; //判断第一项的前面是否还有数据
	last_id = (last_id == log_num) ? 1 : 0; //判断最后一项的后面是否还有数据
	sprintf(first_last,"%d/%d|",first_id,last_id); //
	strcat(all_data,first_last);
	return (uint8_t*)all_data;
}
/* 返回小于id的number数目的日志条数 */
/**
 * Function: getPreSystemLogEntry
 * Description:得到目前ID的前number日志项
 * @param id 要查找的最小的ID号
 * @param number 查找的日志总数目
 * @return 格式化以后要发给浏览器的数据
**/
uint8_t *getPreSystemLogEntry(int id,int number){
	int begin_id = 0;
	int end_id = 0;
	int found_num = 0;
	static char all_data[4096];
	char single_entry[256];
	char header[10];
	char first_last[10];
	char log_time[32];
	char level[10];
	SYSTEM_LOG *temp;
	memset(all_data,0,sizeof(all_data));
	memset(header,0,sizeof(header));
	memset(first_last,0,sizeof(first_last));
	sprintf(header,"%d/%d|",number,id);
	strcat(all_data,header);
	end_id = id;
	begin_id = end_id - number;
	if(begin_id <= 0){  //前面的项数总和不到number
		begin_id = 1;
	}
	for(temp = log_root; temp; temp = temp->next){
		memset(single_entry,0,sizeof(single_entry));
		if(temp->log_id >= begin_id && temp->log_id < end_id){   //找到了符合条件的日志
			memset(log_time,0,sizeof(log_time));
			formatSystemTime(log_time,&temp->log_time);
			switch(temp->log_level){
				case 0:
					strcpy(level,"信息");
					break;
				case 1:
					strcpy(level,"警告");
					break;
				case 2:
					strcpy(level,"错误");
					break;
			}
			sprintf(single_entry,"%s/%d/%s/%s/%s|",level,temp->log_id,log_time,temp->log_content,temp->user);
			strcat(all_data,single_entry);
			found_num++;
			}
		if(found_num == number){    //找到了所有的选项
			break;
		}
	}
	if(found_num == 0 ){   //没有找到符合条件的项
		sprintf(single_entry,"NoEntries/-/-/-|");
		strcat(all_data,single_entry);
		begin_id = 1;
		end_id = log_num;
	}
	begin_id = (begin_id == 1) ? 1 : 0; //判断第一项的前面是否还有数据
	end_id = (end_id == log_num) ? 1 : 0; //判断最后一项的后面是否还有数据
	sprintf(first_last,"%d/%d|",begin_id,end_id); //
	strcat(all_data,first_last);
	return (uint8_t*)all_data;
}

