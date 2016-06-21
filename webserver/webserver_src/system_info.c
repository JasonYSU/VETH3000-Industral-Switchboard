/*******************************************************
File name: system_info.c
Description: 提供和系统相关的接口函数，用于获取系统时间以及编译时间，所以该文件应该每次都要被编译
Author: dujiancheng
Version: v1.0
********************************************************/
#include "system_info.h"
#include "board_info.h"
#include "mac_link.h"
SYSTEM_TIME system_time;
uint32_t time_update_interval;  //时间更新经过的秒数
uint8_t is_recover_factory = 0;      //记录系统是否进行了恢复出厂设置操作
time_t system_start_time;

 /**
 * Function: calcSystemTime
 * Description: 计算系统当前的时间根据当前系统的总的时间秒数进行计算
 * @return 时间格式的字符串
 * @ATTENTION 该函数返回值使用的动态开辟的空间，调用者需要手动释放该空间
**/
 char* calcSystemTime(){
	char *result;
	time_t current_sec = 0;
	struct tm *system_tm;
	uint32_t time_interval;
	//uint32_t year,month,day,hour,minute,second,week;
	result = (char*)malloc(200 * sizeof(char));
	if(result == NULL){
		printf("calc system time malloc failed\n\r");
		return NULL;
	}
	time_interval = sCount - time_update_interval;
	system_time.system_sec += time_interval;
	system_tm = localtime(&system_time.system_sec);
	getTimeWday((char*)system_time.weekday,system_tm->tm_wday);
	time_update_interval = sCount;  //重新计算时间间隔
	sprintf(result,"%02d-%02d-%02d %02d:%02d:%02d %s",(system_tm->tm_year + 1900),(system_tm->tm_mon + 1),
			system_tm->tm_mday,system_tm->tm_hour,system_tm->tm_min,system_tm->tm_sec,system_time.weekday);
	return result;
}
/**
 * Function: generateIPConfigData
 * Description: 生成IP地址配置相关的信息
 * @return 格式化之后要返回给浏览器解析的字符串
**/
char *generateIPConfigData(){
	static char ip_config[100];
	sprintf(ip_config,"%d,%d.%d.%d.%d,%d.%d.%d.%d,%d.%d.%d.%d,,%d,%d.%d.%d.%d,%d.%d.%d.%d,%d.%d.%d.%d,,%d,%d,%d.%d.%d.%d,%d",
		0,board.ip_config.ip_addr[0],board.ip_config.ip_addr[1],board.ip_config.ip_addr[2],board.ip_config.ip_addr[3],
		board.ip_config.net_mask[0],board.ip_config.net_mask[1],board.ip_config.net_mask[2],board.ip_config.net_mask[3],
		board.ip_config.route_addr[0],board.ip_config.route_addr[1],board.ip_config.route_addr[2],board.ip_config.route_addr[3],
		1,
		board.ip_config.ip_addr[0],board.ip_config.ip_addr[1],board.ip_config.ip_addr[2],board.ip_config.ip_addr[3],
		board.ip_config.net_mask[0],board.ip_config.net_mask[1],board.ip_config.net_mask[2],board.ip_config.net_mask[3],
		board.ip_config.route_addr[0],board.ip_config.route_addr[1],board.ip_config.route_addr[2],board.ip_config.route_addr[3],
		1,0,board.ntp_server_addr[0],board.ntp_server_addr[1],board.ntp_server_addr[2],board.ntp_server_addr[3],board.is_config_ntp_server
		);
	//printf("generate system config is %s\n\r",ip_config);
	return ip_config;
}
/**
 * Function: generateSystemStatusInfo
 * Description: 生成系统的的相关信息，信息包括软件版本号，系统时间等等相关内容
 * @return 格式化之后要返回给浏览器解析的字符串
**/
char *generateSystemStatusInfo(){
	static char sys_info[400];
	unsigned char mac_str[25];
	//int day,hour,minute,second;
	SYSTEM_RUN_TIME run_time;
	int minute_total,hour_total;
	char software_info[100];  //软件信息
	char system_compile_date[30];
	char *sys_time = NULL;// = "2014-11-17T00:28:30+00:00";
	calcSystemRunTime(&run_time);
	memset(mac_str,0,sizeof(mac_str));
	getMacString(mac_str,board.sets_info.The_MAC);
	memset(software_info,0,sizeof(software_info));
	sprintf(software_info,"%s %s %s",
				board.sets_info.The_Info.The_type,
				board.sets_info.The_Info.The_Version,
				board.sets_info.The_Info.The_Soft);
	memset(sys_info,0,sizeof(sys_info));
	sys_time = calcSystemTime();
	generateSystemCompileDate(system_compile_date);
	/* 先判断系统时间是否是默认的时间也就是系统时间有没有和NTP服务器进行同步*/
	if( !memcmp(sys_time,"1970",4)){  //比较年份是否是默认的
		printf("System time not sycn\n\r");
		sprintf(sys_info,"%s/ %02dD%02d:%02d:%02d/%s/%s/%s/%d",	
					mac_str,run_time.day,run_time.hour,run_time.minute,run_time.second,sys_time,software_info,system_compile_date,0);
	}else{
		sprintf(sys_info,"%s/ %02d天%02d:%02d:%02d/%s/%s/%s/%d",	
					mac_str,run_time.day,run_time.hour,run_time.minute,run_time.second,sys_time,software_info,system_compile_date,
					(request_failed_num >= NTP_REQUEST_FAILED_MAX_NUM) ? 0:1);
	}
	/* 发送数据的最后一位是用于判断和NTP服务器的连接是否畅通 */
	//printf("generate system info is\n\r%s\n\r",sys_info);
	free(sys_time);
	sys_time = NULL;
	return sys_info;
}
/**
 * Function: initSystemTime
 * Description: 初始化默认的系统时间
**/
void initSystemTime(){
	system_time.year = 1970;
	system_time.month = 1;
	system_time.day = 1;
	system_time.hour = 0;
	system_time.minute = 0;
	system_time.second = 0;
	system_time.week = 4;
	system_time.system_sec = 0;
	strcpy((char*)system_time.weekday,"星期四"); 
}
/**
 * Function: generateSystemCompileDate
 * Description: 生成系统的编译时间
 * @param result 存放生成的信息
 * @return 生成的信息
**/
char *generateSystemCompileDate(char *result){
	int i;
	char *month[]={"","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	char compile_date[12];
	char compile_month[4];
	char compile_year[6];
	char compile_day[3];
	char final_month[3];
	char compile_hour[3];
	char compile_minute[3];
	char compile_second[3];
	//static char result[30];
	memset(compile_date,0,sizeof(compile_date));
	memset(compile_month,0,sizeof(compile_month));
	memset(compile_year,0,sizeof(compile_year));
	memset(compile_day,0,sizeof(compile_day));
	memset(compile_hour,0,sizeof(compile_hour));
	memset(compile_minute,0,sizeof(compile_minute));
	memset(compile_second,0,sizeof(compile_second));
	//memset(result,0,sizeof(result));
	strcpy(compile_date,__DATE__);
	strncpy(compile_month,compile_date,3);
	(compile_date[4] != ' ')?strncpy(compile_day,&compile_date[4],2):strncpy(compile_day,&compile_date[5],1);
	strncpy(compile_year,&compile_date[7],4);
	for(i = 1;i < 13;i++){
		if( !strcmp(compile_month,month[i])){
			break;
		}
	}
	strncpy(compile_hour,__TIME__,2);
	strncpy(compile_minute,&(__TIME__[3]),2);
	strncpy(compile_second,&(__TIME__[6]),2);
	sprintf(result,"%s%d%s%s%s%s",compile_year,i,compile_day,compile_hour,compile_minute,compile_second);
	//printf("program compile date is %s\n\r",result);
	return result;
}
/**
 * Function: calcSystemRunTime
 * Description: 计算系统运行的时间
 * @param run_time 系统运行的时间结构体指针
 * @return 1 程序正常结束
**/
uint8_t calcSystemRunTime(SYSTEM_RUN_TIME *run_time){
	int minute_total,hour_total;
	run_time->second = sCount % 60; //对计时器取余得到秒数
	
	minute_total = sCount / 60;  //得到总的分钟数
	run_time->minute = minute_total % 60;  //对总的分钟取余得到分钟数
	
	hour_total = minute_total / 60; //得到总的小时数
	run_time->hour = hour_total % 24;     //得到小时数
	
	run_time->day = hour_total / 24; //得到天数
	return 1;
}
/* 计算某一天对应的星期几 */
/**
 * Function: getTimeWday
 * Description: 根据每周的第几天得到对应的星期几的汉字形式
 * @param src 存放星期几的中文形式的字符串
 * @return 1 程序正常结束
**/
uint8_t getTimeWday(char* str,int day){
		switch(day){
		case 0:
			strcpy(str,"星期日");
			break;
		case 1:
			strcpy(str,"星期一");
			break;
		case 2:
			strcpy(str,"星期二");
			break;
		case 3:
			strcpy(str,"星期三");
			break;
		case 4:
			strcpy(str,"星期四");
			break;
		case 5:
			strcpy(str,"星期五");
			break;
		case 6:
			strcpy(str,"星期六");
			break;
	}
	return 1;
}