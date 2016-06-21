/*******************************************************
File name: web_main.c
Description: web服务器的main文件，循环接收和处理网络数据包、串口相关的操作、各种计时器的检测。
			 对其他某块进行管理和调度
Author: dujiancheng
Version: v1.0
********************************************************/
#include "mac_link.h"
#include "veth_common.h"
#include "board_info.h"
#include "ports.h"
#include "mac_link.h"
#include "vlan.h"
#include "httpd.h"
#include "ip_config.h"
#include "SerialUtility.h"
#include "system_info.h"
#include "system_maintain.h"
#include "veth_RSTP.h"
#include "ntp_client.h"
#include "LogUtility.h"
#include "veth_serial.h"
#include "SerialClientUdp.h"
#include "etharp.h"
#include "init.h"
#include "httpd.h"
#include "ip_config.h"
#include "mem.h"
#include "timers.h"
#include "timer.h"
#include "tcp_impl.h"
#include "ip_frag.h"
#include "Session.h"
#include "board_info.h"
#include "snmp_structs.h"
#include "serialClient.h"
#include "SerialServer.h"
#include "MulticastUtility.h"
#include "igmp.h"
#include "SerialServerUdp.h"

#define PORT_CONFIG_INDEX  66  //网页上的最大端口数的个位在data_lib_config_js数组中对应的下标

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */
#define SERIAL_DATA_TIMEOUT     800
BOARD board= {{"vorx_veth2000"}};
BOARD board_temp; //存储之前Flash中的数据，外部变量

unsigned char isFirstRequest;    //判断是否是第一次对mac地址配置信息的请求
unsigned char isFirstRequestPorts;  //判断是否是第一次对端口配置信息的请求
unsigned char isFirstRequestVlanPorts;  //判断是否是第一次对VLAN端口配置信息的请求

unsigned char isRecoverMacConfig; //是否需要恢复mac配置
uint32_t auto_ntp_refresh_timeout = 0;  //自动向NTP服务器发送更新时间的定时器
int vlan_time_out;
uint8_t is_reboot;  //系统是否重新启动
uint32_t ntp_request_timeout;   //ntp周期性请求的时间
int write_flash_time;           //定时更新Flash的定时器
uint32_t recond_system_reboot_time_out;   
uint32_t lwip_time_out;
uint32_t sigalarm_time_out;
uint8_t pre_port_link_status[PORTSNUM];  //记录之前的端口的链接状态，用于记录端口link日志
uint32_t snmp_system_inc_timeout;        //snmp更新时间计时器
uint32_t serial_client_timeout;   //串口客户端发送数据的时间间隔
int isConfigRemoteHost = 0;       //判读远程主机是否配置
uint32_t  serial_data_timeout=0;   //
uint32_t igmp_time_out = 0;       
uint32_t print_timeout = 0;
uint8_t  bp_targ = 0;

/**
 * Function: processSerialClientRemoteData
 * Description: 静态函数，周期性检测串口是否有数据要向远程主机发送，如果有数据发送则调用相关函数处理串口数据
 * 
**/
static void processSerialClientRemoteData() {
    int i,j;
	uint8_t is_local_transmit = 0;
    if(msCount - serial_data_timeout > SERIAL_DATA_TIMEOUT) {
		for(i = 0; i < MAX_SERIAL_NUM; i++) {
            if( !client_array_queue[i].empty_queue && client_array_queue[i].queue->head != NULL) { //串口队列有数据
                /* 判断相应的串口是否配置了远程的主机 */
                for(j = 0; j < MAX_SERIAL_REMOTE_NUM; j++) {
                    if(board.serial_remote_host[j].serial_no == client_array_queue[i].serial_no
                            && board.serial_remote_host[j].remote_ip
                            && board.serial_remote_host[j].remote_port) {
                        isConfigRemoteHost = 1;
                        break;
                    }
                }//结束对远端配置的判断
                if(!isConfigRemoteHost) { //对应的串口远端信息没有配置
                    printf("serial %d not remote config,queue will empty\n\r",client_array_queue[i].serial_no);
                    removeAllQueueNode(client_array_queue[i].serial_no,client_array_queue[i].queue); //删除队列
                } else {  //配置了对应的远程信息,向远程发送一定长度的数据
                    /* 代码98-102行实现了本机环回的串口数据发送，该部分代码没有进行压力测试，
					   在低压力下测试没有问题，如果该部分代码在高压下出现问题，可进一步优化
					*/
                    is_local_transmit = checkLocalSerialTransmit(board.serial_remote_host[j].remote_ip);
					if(is_local_transmit == 1){  //进行本地串口之间的转发操作
						transmitLocalSerial(board.serial_remote_host[j].remote_port);
						return;
					}
					
					//建立和某一个串口对应的远程连接
					if(board.serial_remote_host[client_array_queue[i].serial_no].is_use_udp == 1) { //配置使用UDP协议
                        /* 调用UDP处理函数处理串口中的数据 */
                        processSerialClientUdpConnection(board.serial_remote_host[j].remote_ip,board.serial_remote_host[j].remote_port,client_array_queue[i].serial_no);
                    } else {
						 //printf("serial %d client will send data\n\r",client_array_queue[i].serial_no);
                         serialClientConnect(board.serial_remote_host[j].remote_ip,board.serial_remote_host[j].remote_port,client_array_queue[i].serial_no);
                    }
                }
                WatchDogTimerReset();
            }
            isConfigRemoteHost = 0;
        }
        serial_data_timeout = msCount;
    }
}

/**
 * Function:    checkFlashUpdate
 * Description: 静态函数，周期性检测board结构的数据有没有变化，如果有变化则需要更新Flash中的数据
 * 
**/
static void checkFlashUpdate() {
    if(sCount - write_flash_time >= 2 * 10) { //向Flash写数据,测试的时候使用的是1s进行打印信息
        write_flash_time = sCount;
        WatchDogTimerReset();
		//createMulticastClient();  //测试使用，之后应该删除
        memset(&board_temp,0,sizeof(board_temp));
        getInvariate(&board_temp,sizeof(board_temp)); //得到之前flash中的数据
        if(calculateChecksum(&board_temp,sizeof(board_temp),18)
                != calculateChecksum(&board,sizeof(board),18)) { //判断两个board的检验和是否相同
            //updateInvariate(&board,sizeof(board));
            if(updateInvariate(&board,sizeof(board))) { //向Flash中写入数据
                //printf("update flash sucessful\n\r");
            } else {
               // printf("update flash failed\n\r");
            }
        } else {
           // printf("not need update flash\n\r");
        }
    }
}
/**
 * Function: ntpServerRequest
 * Description: 静态函数，周期性与NTP服务器进行通信，更新当前系统时间
 * 
**/
static void ntpServerRequest(){
	char message[96];
	if(board.is_config_ntp_server && board.ntp_server_addr[0] != 0){  //开启了NTP服务器模式并且NTP服务器地址不为0
		if(sCount - auto_ntp_refresh_timeout >= ntp_request_timeout){  //假定5s发送一次ntp请求
			connectNTPServer(); //与NTP服务器建立连接
			auto_ntp_refresh_timeout = sCount;
			if( !is_reboot && is_need_record_time_log == 0 && request_failed_num >= NTP_REQUEST_FAILED_MAX_NUM){  //请求已经超时，网络不通,需要记录超时记录
				is_need_record_time_log = 1;  //设置需要记录标志，当时间被同步时记录信息
				memset(message,0,sizeof(message));
				sprintf(message,"与时间服务器通信失败，系统时间未能同步");
				addLogEntry(WARNING,"vorx",message);   //时间未能同步警告日志
				printSystemLog();
			}
		}
	}
}
/**
 * Function: ntpServerRequest
 * Description: 静态函数，判断与NTP服务器通信是否超时，进而可以判断与NTP服务器的连接是否有效
 * 
**/

static void isNTPServerRequestTimeout(){
	if(is_open_ntp_request_timer && 
		sCount - ntp_send_request_time >= NTP_REQUEST_TIME_OUT){ //ntp请求超时可能网络不通，加快对ntp服务器的请求，确定是否是网络不通
		ntp_request_timeout = QUICK_NTP_REQUEST;
	}
}

/**
 * Function: modifyMaxPortConfig
 * Description: 静态函数，修改web页面中配置的最大端口数，最大端口数的配置存放在config.js文件中
 * 				在系统启动时该方法被调用
 * @return 修改是否成功
**/
static uint8_t modifyMaxPortConfig(){
	/* config.js里面端口数的定义是
    ** portsNum = 12 - 2;
	*/
	if(ports_num == 10){  //端口的最大值是10
		data__lib_config_js[PORT_CONFIG_INDEX] = 0x32;
	}else if(ports_num == 8){
		data__lib_config_js[PORT_CONFIG_INDEX] = 0x30;
	}
	return 1;
}
/**
 * Function: initTimeout
 * Description: 静态函数，用于初始化系统使用的各种定时器的值
 * 
**/
static uint8_t initTimeout(){
	auto_ntp_refresh_timeout = sCount;
	lwip_time_out = msCount;
    sigalarm_time_out = msCount;
	write_flash_time = sCount;
	ntp_request_timeout = QUICK_NTP_REQUEST;
    is_open_delay_config_ports = 0;
	delay_config_ports_time = sCount;  //初始化延迟端口配置计时器
	band_width_refresh_time = msCount; //开始记录刷新带宽利用时间
	is_reboot = 1;  //当程序执行此处时系统重新运行过
	recond_system_reboot_time_out = sCount;
	snmp_system_inc_timeout = msCount;
	serial_client_timeout = msCount;
	return 0;
}
/**
 * Function: addNetifList
 * Description: 静态函数，添加netif到netif列表中去
 * 
**/
static void addNetifList(struct netif *netif,VETH_IP_T *ip){
	ip_addr_t ipaddr,net_mask,gw_addr;
	int i;
	struct netif *temp_netif;
	IP4_ADDR(&ipaddr, ip->ip_addr[0],ip->ip_addr[1],ip->ip_addr[2],ip->ip_addr[3]);
	IP4_ADDR(&net_mask, ip->net_mask[0],ip->net_mask[1],ip->net_mask[2],ip->net_mask[3]);
	IP4_ADDR(&gw_addr, ip->route_addr[0],ip->route_addr[1],ip->route_addr[2],ip->route_addr[3]);
	//for(i = 0; i < ports_num; i++){
	//	temp_netif = (struct netif *)malloc(sizeof(struct netif));
	//	temp_netif = netif_add(temp_netif,&ipaddr,&net_mask,&gw_addr,"normal",ethernetif_init,ethernet_input);
	//}
	netif_add(netif,&ipaddr,&net_mask,&gw_addr,"normal",ethernetif_init,ethernet_input);
}
/**
 * Function: initLWIPConfig
 * Description: 静态函数，对lwip协议栈的各个某块进行初始化
 * 
**/
static void initLWIPConfig(){
	WatchDogTimerReset();
    mem_init();   //初始化lwip内存管理
    lwip_init();
    addNetifList(&veth_netif,&board.ip_config);
    httpd_init();
    netif_set_default(&veth_netif);
    netif_set_up(&veth_netif);
}

/**
 * Function: initSwitchChipConfig
 * Description: 静态函数，初始化交换芯片的各种配置以及相关的状态变量的配置
 * 
**/
static void initSwitchChipConfig(){
	isFirstRequestPorts = TRUE;  //第一次请求ports配置
	isFirstRequest = TRUE;       //第一次对MAC地址的请求
    isFirstRequestVlanPorts = TRUE;
    isRecoverMacConfig = TRUE;
	is_file_download = 0;
	
	initPorts();   //初始化端口配置
	initFirstVlan();
	/* 延迟15s之后再进行mac链表的初始化 */
	Delays(15);    //延迟15s之后再去读mac地址
	//printf("delay end\n\r");
	initRoot();    //初始化MAC地址root,该初始化应该在VALN初始化之后进行
	initPreBandWidth();  //初始化带宽利用率数组
	updatePreBandWidth();   //更新收发字节数
    /* 用于端口流量统计和带宽利用率曲线显示使用 */
	
	/* 初始化保存所有配置的状态 */
	memset(save_all_config_result,0,sizeof(save_all_config_result));
	strcpy((char*)save_all_config_result,"no_save");
	memset(login_status,0,sizeof(login_status));
	strcpy((char*)login_status,"no_error");
	memset(pre_compile_date,0,sizeof(pre_compile_date));
	memcpy(pre_compile_date,board.compile_date,sizeof(board.compile_date));
}

/* flag用于区分是否记录当前的时间值 */
/**
 * Function: modifyNetifLastChangeTime
 * Description: 静态函数，修改端口的last change time用于snmp功能
 * @param netif_index netif在netif 列表中对应的索引
 * @param flag  用于区分是否需要记录当前时间值
**/
static void modifyNetifLastChangeTime(int netif_index,uint8_t flag){
	struct netif *netif;
	snmp_ifindextonetif(netif_index,&netif);
	if(flag == 1){
		snmp_get_sysuptime(&netif->ts);
	}else{
		netif->ts = 0;
	}
}

/**
 * Function: getPortsLinkStatus
 * Description: 静态函数，获取端口的link状态,如果端口的状态发生改变，修改端口的last change time
 * 
**/
static uint8_t getPortsLinkStatus(uint8_t *link_array){
	uint8_t port_link;
	int i;
	for(i = 0; i < ports_num; i++){
		port_link = Get_link_status(i);
		link_array[i] = port_link;
		if(is_reboot && port_link){  //系统第一次运行时修改端口状态更改的时间
			modifyNetifLastChangeTime(i + 1,1);
		}
	}
	return 0;
}

/**
 * Function: generatePortsLinkLog
 * Description: 静态函数，生成端口相关的日志信息
 * 
**/
static uint8_t generatePortsLinkLog(uint8_t *current_link,uint8_t len){
	int i;
	char message[96];
	uint8_t port_type;
	for(i = 0; i < len; i++){   //判断端口link状态是否发生了变化，如果发生了变化则生成日志
		if(current_link[i] != pre_port_link_status[i]){  //两次状态不一样,生成日志信息，保存现在的状态
			port_type = getPortType(i);
			memset(message,0,sizeof(message));
			/* 判断是link down 还是link up */
			if(current_link[i]){  //端口由断开状态变为连接状态
				sprintf(message,"%s %d由断开状态变为连接状态",port_type ? "光口":"电口",(i + 1));
				modifyNetifLastChangeTime(i + 1,1);
			}else{
				sprintf(message,"%s %d由连接状态变为断开状态",port_type ? "光口":"电口",(i + 1));
				modifyNetifLastChangeTime(i + 1,0);
			}
			
			addLogEntry(INFO,"vorx",message);
			/* 保存现在的状态 */
			pre_port_link_status[i] = current_link[i];
			printSystemLog();
		}
	}
}

/**
 * Function: checkPortsStatus
 * Description: 静态函数，检查端口状态是否发生了变化，如果发生变化，生成日志信息 
 * 
**/
static void checkPortsStatus(){
	uint8 current_port_link_status[PORTSNUM];
	getPortsLinkStatus(current_port_link_status);
	generatePortsLinkLog(current_port_link_status,ports_num);
}
/*  */
/**
 * Function: recordSystemRebootLog
 * Description: 静态函数，记录系统重启和网络升级的相关日志 
 * 
**/
static void recordSystemRebootLog(){
	char message[96];
	char *current_system_time = NULL;
	current_system_time = calcSystemTime();
	
	/* 先检测是否进行了网络升级 */
	if(board.is_network_update == 1 && network_update_sucess == 2){  //用户进行了网络升级操作,并且升级后网络异常
		generateSystemUpdateStatus();   //获取用户网络升级后的状态
//		board.is_network_update = 0;  //设置标志，表明本次网络升级结果已经处理完成
//		updateInvariate(&board,sizeof(board));   //更新Flash的值
	}
	if(!memcmp(current_system_time,"1970",4)){  //未能获取网络时间
		memset(message,0,sizeof(message));
		sprintf(message,"系统时间未能同步，日志将使用默认时间 %s",current_system_time);
		addLogEntry(WARNING,"vorx",message);   //时间未能同步警告日志
		printSystemLog();
		is_need_record_time_log = 1;   //需要记录时间恢复时的日志信息
	}else{
		memset(message,0,sizeof(message));
		sprintf(message,"系统时间同步完成，当前系统时间是 %s",current_system_time);
		addLogEntry(WARNING,"vorx",message);   //时间未能同步警告日志
		printSystemLog();
		is_need_record_time_log = 0;  //同步完成不需要记录时间日志
	}
	if(network_update_sucess != 2){  //先记录网络升级的信息,说明用户进行了网络升级
		if(network_update_sucess == 1){  //网络升级成功
			memset(message,0,sizeof(message));
			sprintf(message,"网络升级成功，系统版本为%s",board.compile_date);
			addLogEntry(INFO,"vorx",message);   //升级失败生成警告日志
			printSystemLog();
		}else{
			memset(message,0,sizeof(message));
			sprintf(message,"网络升级失败或者使用相同的镜像，系统版本为%s",board.compile_date);
			addLogEntry(WARNING,"vorx",message);   //升级失败生成警告日志
			printSystemLog();
		}
		board.is_network_update = 0;  //设置标志，表明本次网络升级结果已经处理完成
		updateInvariate(&board,sizeof(board));   //更新Flash的值
	}
	if(is_recover_factory == 1){  //说明系统进行了初始化操作
		memset(message,0,sizeof(message));
		sprintf(message,"系统成功恢复出厂设置");
		addLogEntry(INFO,"vorx",message);   //升级失败生成警告日志
		printSystemLog();
	}
	if(is_reboot){   //系统重新启动记录重启信息
		addLogEntry(INFO,"vorx","系统重启成功");   //升级失败生成警告日志
		printSystemLog();
		is_reboot = 0;  //设置重启标志为0
	}
	
	free(current_system_time);
	current_system_time = NULL;
}


/**
 * Function: recordSystemRebootLog
 * Description: 静态函数，删除过期的session
 * 
**/
static void deleteSessionTimeout(struct session_node *node){
	deleteSession(node->session_key);  //先删除session hash table数据
	deleteSessionNode(&session_root,node);//删除session 链表数据
}

/**
 * Function: recordSystemRebootLog
 * Description: 静态函数，判断是否有过期的session，如果有则删除该session
 * 
**/
static void isHasSessionTimeout(){
	struct session_node *temp;
	temp = session_root;
	while(temp){
		if(isSessionTimeout(session_root,temp)){  //session过期，删除对应的session  
			printf("have session timeout\n\r");
			deleteSessionTimeout(temp);
		}
		temp = temp->next;
	}

}
/**
 * Function: recordSystemRebootLog
 * Description: 静态函数，周期性的修改snmp的时钟时间
 * 
**/
static void modifySnmpSystemClick(){
	/* 每10ms增加一次up time的值 */
	if(msCount - snmp_system_inc_timeout >= 10){
		snmp_system_inc_timeout = msCount;
		snmp_inc_sysuptime();
	}
}
#if 0
/* 定时检测串口是否有数据要发送，测试客户端模式时用的数据 */
static void checkSerialData(){
	if(sCount - serial_client_timeout >= SERIAL_CHECK_TIME){
		//serialClientConnect("192.168.20.210",8080,1);
		serial_client_timeout = sCount;
	}
}
#endif 
int
web_main(void) {
	int i;
    VETH_TYPE_T  device;
	getSetType(&device);   //获取设备信息
    ports_num = device.max_portNo;
	initLWIPConfig();
	initSwitchChipConfig();
	modifyMaxPortConfig();  //修改网页中config.js对应的端口数量的配置
	connectNTPServer(); //与NTP服务器建立连接
    memset(is_modify_port,0,sizeof(is_modify_port)); //关闭延迟配置端口模式
	initTimeout();
	memset(pre_port_link_status,0,sizeof(pre_port_link_status));//置port link状态为0
	/* 获取当前的port的link状态 */
	getPortsLinkStatus(pre_port_link_status);
	initSerialInfo();
    initAllQueueArray();
	initDefaultSerialPort();  //初始化serial server监听的端口号的服务器监听连接
	//createMulticastListen();
	igmp_time_out = msCount;
    timer_init();
	timer_set_interval(TIMER_EVT_ETHARPTMR, ARP_TMR_INTERVAL / 10);
	timer_set_interval(TIMER_EVT_TCPTMR, TCP_TMR_INTERVAL / 2 );
#if IP_REASSEMBLY
	timer_set_interval(TIMER_EVT_IPREASSTMR, IP_TMR_INTERVAL / 10);
#endif
	memset(recv_total_data,0,sizeof(recv_total_data));
	memset(udp_serv_recv,0,sizeof(udp_serv_recv));
	print_timeout = sCount;
	initSerialClient();
//	printf("remote host config \n\r");
	for(i = 0; i < MAX_SERIAL_REMOTE_NUM; i++){
		if(board.serial_remote_host[i].remote_port){
			//printf("seq %d serial %d remote host is %s,port is %d\n\r",i,board.serial_remote_host[i].serial_no,
			//		board.serial_remote_host[i].remote_ip,board.serial_remote_host[i].remote_port);
		}
	}
	PT_INIT(&cli_pt);
	PT_INIT(&rstp_pt);
    while(1) {
        WatchDogTimerReset();
        Blink_LED_RUN();
		if(1 == CLI_BUTTON) {
            //CLI(&Uart1Handle,&board.sets_info,CLI_TIMEOUT);
            PT_SCHEDULE(CLI(&cli_pt,&Uart1Handle,&board.sets_info,CLI_TIMEOUT));
        }
		/* 代码测试串口服务器接收数据的打印，调试时使用 */
//		if(sCount - print_timeout >= 10){
//			printf("------TCP serial server recv data-------\n\r");
//			for(i = 0; i < 4; i++){
//				printf("serial %d recv data is %d\n\r",i + 1,recv_total_data[i]);
//			}
//			printf("--------------------------------------------\n\r");
//			
//			printf("*********UDP serial server recv data*******\n\r");
//			for(i = 0; i < 4; i++){
//				printf("serial %d recv data is %d\n\r",i + 1,udp_serv_recv[i]);
//			}
//			printf("*******************************************\n\r");
//			printf("client queue serial no\n\r");
//			for(i = 0; i < MAX_SERIAL_NUM; i++){
//				printf("client %d queue serial no is %d\n\r",i,client_array_queue[i].serial_no);
//			}
//			print_timeout = sCount;
//		}
        ethernetif_input(&veth_netif);
        if(msCount - sigalarm_time_out >= 5) {
            sigalarm_handler(0);
            sigalarm_time_out = msCount;
        }
        if(timer_testclr_evt(TIMER_EVT_TCPTMR)) {
            tcp_tmr();
        }
#if IP_REASSEMBLY
        if(timer_testclr_evt(TIMER_EVT_IPREASSTMR)) {
            ip_reass_tmr();
        }
#endif
        if(timer_testclr_evt(TIMER_EVT_ETHARPTMR)) {
            etharp_tmr();
        }
		if( is_open_delay_config_ports == 1 && sCount - delay_config_ports_time >= PORT_LEDAY_TIME){   //检测是否需要配置芯片的端口信息
			is_open_delay_config_ports = 0;  //关闭延迟配置
			modifySwitchChipPortsConfig();
			delay_config_ports_time = sCount;
		}
		if(msCount - igmp_time_out >= IGMP_TMR_INTERVAL){   //每100ms调用一次
			igmp_time_out = msCount;
			igmp_tmr();
		}
		ntpServerRequest();
		isNTPServerRequestTimeout();
		checkFlashUpdate();
		if( !is_reboot){ //先保证重启和系统升级的日志已经被记录
			checkPortsStatus();
		}
		if( is_reboot && sCount - recond_system_reboot_time_out >= 1 * 20){  //30s后运行
			recordSystemRebootLog();
			//recond_system_reboot_time_out = sCount;
		}
		isHasSessionTimeout();
		modifySnmpSystemClick();
		//checkSerialData();
		serialClient();
        serialServer();
        processSerialClientRemoteData();
        WatchDogTimerReset();
    }
    return 0;
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
