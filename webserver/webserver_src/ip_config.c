/*******************************************************
File name: ip_config.c
Description: 处理IP相关的内容，用户修改IP地址时改文件的相关方法会被调用
Author: dujiancheng
Version: v1.0
********************************************************/
#include"ip_config.h"
#include "board_info.h"
#include "netif.h"
#include "request.h"
#include "LogUtility.h"
#include "Session.h"
/**
 * Function: setHostAddress
 * Description: 设置服务器的IP地址
 * 
**/
int setHostAddress(struct netif *netif,VETH_IP_T *ip){
	ip_addr_t ipaddr;
	IP4_ADDR(&ipaddr, ip->ip_addr[0],ip->ip_addr[1],ip->ip_addr[2],ip->ip_addr[3]);
	//uip_sethostaddr(ipaddr);
	netif_set_ipaddr(netif, &ipaddr);
	IP4_ADDR(&ipaddr, ip->net_mask[0],ip->net_mask[1],ip->net_mask[2],ip->net_mask[3]);
    netif_set_netmask(netif, &ipaddr);
	IP4_ADDR(&ipaddr, ip->route_addr[0],ip->route_addr[1],ip->route_addr[2],ip->route_addr[3]);
    netif_set_gw(netif, &ipaddr);
	
	return 1;
}
/**
 * Function: modifyIPConfig
 * Description: 获取用户提交的IP地址配置，NTP服务器地址配置，解析出相应的参数，根据参数进行相关的配置
 * 
**/
int modifyIPConfig(char *url){
	char *ip_addr = NULL;
	char *net_mask = NULL;
	char *gw = NULL;
	char *ntp_server = NULL;
	char *is_config_ntp;
	int i;
	uint8_t get_ip_addr[4];
	uint8_t mask_addr[4];
	uint8_t ip_gw[4];
	uint8_t get_ntp_server[4];
	char message[96];
	struct netif *temp;
	//int begin_time;
	ip_addr = getParameter(url,"ip_conf_addr");
	net_mask = getParameter(url,"ip_conf_mask");
	gw = getParameter(url,"ip_conf_gw");
	ntp_server = getParameter(url,"ntp_server_addr");
	is_config_ntp = getParameter(url,"isConfigNTP");
	memset(get_ip_addr,0,sizeof(get_ip_addr));
	memset(mask_addr,0,sizeof(mask_addr));
	memset(ip_gw,0,sizeof(ip_gw));
	memset(get_ntp_server,0,sizeof(get_ntp_server));
	getFormatAddress(get_ip_addr ,ip_addr); //得到IP地址的数组
	getFormatAddress(mask_addr,net_mask);
	getFormatAddress(ip_gw,gw);
	//printf("ntp server addr is %s\n\r",ntp_server);
	if( !strcmp(ntp_server,"")){
		memset(board.ntp_server_addr,0,sizeof(board.ntp_server_addr));
		//printf("ntp server is NULL\n\r");
	}else{
		getFormatAddress(get_ntp_server,ntp_server);
	}
	if( memcmp(board.ip_config.ip_addr,get_ip_addr,sizeof(get_ip_addr)) || 
		memcmp(board.ip_config.net_mask,mask_addr,sizeof(mask_addr)) ||
		memcmp(board.ip_config.route_addr,ip_gw,sizeof(ip_gw))){   //用户有修改的时候才会设置
		WatchDogTimerReset();
		memcpy(board.ip_config.ip_addr,get_ip_addr,sizeof(get_ip_addr));
		memcpy(board.ip_config.net_mask,mask_addr,sizeof(mask_addr)); 
		memcpy(board.ip_config.route_addr,ip_gw,sizeof(ip_gw));
		//begin_time = msCount;
		for(temp = netif_list; temp;temp = temp->next){
			setHostAddress(temp,&board.ip_config);
		}
		//printf("set ip time is %d ms\n\r",msCount - begin_time);
		WatchDogTimerReset();
		//begin_time = msCount;
		if(updateInvariate(&board,sizeof(board))){
			//printf("new ip address has store flash\n\r");
		}else{
			//printf("store new ip failed\n\r");
		}
		memset(message,0,sizeof(message));
		sprintf(message,"IP相关信息被修改，IP：%s  子网掩码：%s 网关：%s",ip_addr,net_mask,gw);
		addLogEntry(INFO,"vorx",message);   //升级失败生成警告日志
		printSystemLog();
	}
//	printf("board server is %d.%d.%d.%d;submit ntp addr is %d.%d.%d.%d",board.ntp_server_addr[0],board.ntp_server_addr[1],
//			board.ntp_server_addr[2],board.ntp_server_addr[3],get_ntp_server[0],get_ntp_server[1],get_ntp_server[2],get_ntp_server[3]);
	if(board.is_config_ntp_server != atoi(is_config_ntp)){
		if(board.is_config_ntp_server == 1){  //之前的状态为开启状态
			addLogEntry(INFO,"vorx","关闭了NTP服务器的配置");   //升级失败生成警告日志
			printSystemLog();
		}else{
			addLogEntry(INFO,"vorx","开启了NTP服务器的配置");   //升级失败生成警告日志
			printSystemLog();
		}
	}
	if(atoi(is_config_ntp) == 1 && ntp_server && memcmp(board.ntp_server_addr,get_ntp_server,sizeof(get_ntp_server))){  //ntp服务器地址被更改了，重新赋值
		memcpy(board.ntp_server_addr,get_ntp_server,sizeof(get_ntp_server));
		/* 启用了并且修改了ntp服务器的地址，生成相关日志 */
		memset(message,0,sizeof(message));
		sprintf(message,"NTP服务器地址修改为：%s",ntp_server);
		addLogEntry(INFO,"vorx",message);   //升级失败生成警告日志
		printSystemLog();
	}
	board.is_config_ntp_server = atoi(is_config_ntp);
	//printf("update flash time is %d ms\n\r",msCount - begin_time);
	freeMemory(ip_addr);
	freeMemory(net_mask);
	freeMemory(gw);
	freeMemory(ntp_server);
	freeMemory(is_config_ntp);
}
/**
 * Function: getFormatAddress
 * Description: 对IP地址进行格式化，把IP地址字符串转换成相对应的整形数组
 * @param array 存放格式化之后的的IP地址，整形数组
 * @param str   被格式化的IP地址字符串，字符串用点十进制表示
 * @return  返回值为1 表示格式化完成
**/
int getFormatAddress(uint8_t *array,char *str){
	char seps[] = ".";
	char *token;
	char temp[20];
	int i = 0;
	strcpy(temp,str);  //保护源字符串
	for(token = strtok(temp,seps);token;token = strtok(NULL,seps)){
		array[i] = atoi(token);
		i++;
	}
	return 1;
}
