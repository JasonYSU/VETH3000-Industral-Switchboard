/*******************************************************
File name: ntp_client.c
Description: ntp客户端实现，向NTP服务器发送NTP请求。
TODO: 对请求的策略需要进一步的商榷，目前采用的策略是如果请求失败则加快请求一次断定服务器通信是否存在
	  如果服务器不存在，也没有做出相应的更改，需要进一步优化
Author: dujiancheng
Version: v1.0
********************************************************/
#include "ntp_client.h"
#include <time.h>
#include "LogUtility.h"
#include "system_info.h"

static struct udp_pcb * ntp_udp_client = NULL;

static uint32_t send_data_length = 0;
static struct ntp_packet ntp_client;
uint8_t request_failed_num = 0;  //NTP请求失败的次数
uint8_t is_need_record_time_log = 0;  //假定不需要记录
uint8_t is_open_ntp_request_timer;   //是否使用了发送请求计时器
uint32_t ntp_send_request_time;  //记录请求发送的时刻
 /**
 * Function: recordSystemStartTime
 * Description: 检查是否记录系统启动的时间，检测当前系统时间是否为默认的时间
 * 				如果是则需要计算并且记录启动时的时间，否则不需要记录
 * @param system_time  系统时间结构体指针，目前没有使用
 * @param time_sec 目前时间，以秒为计算单位，该时间是通过NTP服务器获取的
 * @return 1  系统时间需要更新，并且记录了系统的启动时间
 *		   0  系统时间不需要更新
**/
static uint8_t recordSystemStartTime(SYSTEM_TIME *system_time,time_t time_sec){
	struct tm *start_time;
	if(system_time->year < 1980){  //说明系统时间没有被更新过，根据当前time_sec来计算系统启动时的时间
		/* 系统启动的时间为当前时间秒数 - 系统运行的总秒数 */
		system_start_time = time_sec - sCount;
		start_time = localtime(&system_start_time);
		printf("system start time is \n\r%d-%02d-%02d %02d:%02d:%02d %d\n\r",(start_time->tm_year + 1900),(start_time->tm_mon + 1),start_time->tm_mday,start_time->tm_hour,
			start_time->tm_min,start_time->tm_sec,start_time->tm_wday);
		return 1;  //记录了系统启动的时间
	}
	return 0;  //系统时间是最新的不需要进行更新，此标志可以作为是否进行日志时间更新的标志
}
/**
 * Function: NTP_Init
 * Description: 初始化NTP客户端的数据包，该数据包即为发送的NTP请求包
**/
void NTP_Init(void) {
		ntp_client._flags = 0xdb;
        ntp_client._pcs = 0x00;//unspecified
        ntp_client._ppt = 0x01;
        ntp_client._pcp = 0x01;
        ntp_client._rdy = 0x00000000;//big-endian
        ntp_client._rdn = 0x00000000;
        ntp_client._rid = 0x00000000;
		
        ntp_client._ret = JAN_1970;
        ntp_client._ort = 0x0;
        ntp_client._rct = JAN_1970;
        ntp_client._trt = 0x0;
}
/**
 * Function: get_ntp_time
 * Description: NTP服务器响应请求时该函数会被调用，根据服务器响应的数据，来获取服务器当前的时间以此来更新本机时间
**/
void get_ntp_time(void *arg, struct udp_pcb *pcb, struct pbuf *p,ip_addr_t *addr, u16_t port){
    //fd_set pending_data;
	  int i,num;
      struct timeval tv1;
	  struct ntp_packet newpack,ntppack;
	  char compile_date[20];
	  char *recv_data;    //存放收到的数据
	  uint64_t last;
      uint64_t seconds;
	  time_t tt;
	  struct tm *local;
	  uint32_t high_time;
	  uint32_t low_time;
	  //struct tm temp_tm;
	  time_t temp_time;
	  uint8_t is_need_update_log_time;  //是否需要更新日志时间
	  char *current_system_time = NULL;
	  char message[96];
	  num = 0;
	  request_failed_num = 0; //请求失败的次数重置为0
	  //printf("recv ntp server length is %d\n\r",uip_len);
	  newpack = *((struct ntp_packet*)p->payload);
	  memcpy(&high_time,((char*)p->payload) + 40,sizeof(high_time));
	  memcpy(&low_time,((char*)p->payload) + 44,sizeof(low_time));
	high_time = ntohl(high_time);
	last = high_time  - 2208988800;
    tt = last + 8 * 3600;
	local = localtime(&tt);
	//printf("string time is %s\n\r",ctime(&tt));
	//printf("get time is \n\r%d-%02d-%02d %02d:%02d:%02d %d\n\r",(local->tm_year + 1900),(local->tm_mon + 1),local->tm_mday,local->tm_hour,
	//		local->tm_min,local->tm_sec,local->tm_wday);
			
	/* 先检测是否需要记录系统启动的时间，记录此时系统时间，之后判断是否需要更新日志中时间 */
	is_need_update_log_time = recordSystemStartTime(&system_time,tt);
	/* 只需要记录此时系统时间的总秒数，系统时间以此为基础进行计算 */
	system_time.year = (local->tm_year + 1900);  //作为判断是否使用了系统默认时间的依据
	system_time.system_sec = tt;
	/* 是否需要更新日志列表中的时间,该方法只会调用一次 */
	if(is_need_update_log_time == 1){  //需要更新日志列表的时间
		updateSystemLogTime(system_start_time);
	}
	/* 关闭计时器，重置计时器 */
	is_open_ntp_request_timer = 0;
	ntp_send_request_time = sCount;
	time_update_interval = sCount;
	if(is_need_record_time_log){  //之前时间同步失败过，现在时间恢复了，需要记录
		current_system_time = calcSystemTime();
		memset(message,0,sizeof(message));
		sprintf(message,"系统时间同步完成，当前系统时间为 %s",current_system_time);
		addLogEntry(INFO,"vorx",message);   //时间同步完成
		printSystemLog();
		is_need_record_time_log = 0;    //设置需要记录标志为false
	}
	/* 收到数据以后设置ntp请求时间为默认值 */
	ntp_request_timeout = DEFAULT_NTP_TIME_OUT;  
	free(current_system_time);
	current_system_time = NULL;
	/* 释放下层传递过来的内存空间，由于在udp_input函数中没有发现释放该空间的相关代码
	** 因此需要把该部分空间释放。
	*/
	pbuf_free(p);  //释放空间
}
/**
 * Function: send_packet
 * Description: 向NTP服务器发送NTP数据包，请求时间更新
 * @param usd 标志位，目前统一使用1
**/
int send_packet(int usd) {
    //char ntp_header[200];
	struct pbuf *ntp_packet;
	err_t err_result;
	memset(&ntp_client,0,sizeof(ntp_client));
	ntp_packet = pbuf_alloc(PBUF_RAW,sizeof(ntp_client),PBUF_RAM);
	if(!ntp_packet){
		printf("ntp pbuf allocate failed\n\r");
		return -1;
	}
	NTP_Init();
	memset(ntp_packet->payload,0,sizeof(ntp_client));
	memcpy(ntp_packet->payload,(void*)&ntp_client,sizeof(ntp_client));
	
    /*客户端给服务器端发送NTP协议数据包*/
	
	err_result = udp_send(ntp_udp_client,ntp_packet);
	if(err_result == ERR_OK){   //数据发送成功
		//释放pbuf空间
		pbuf_free(ntp_packet);
	}else{
		printf("ntp packet send failed\n\r");
		return -1;
	}
	is_open_ntp_request_timer = 1;
	ntp_send_request_time = sCount;
	//printf("NTP request has sent over\n\r");
}
/**
 * Function: buildNTPUDPConnection
 * Description: 向远程的主机建立UDP连接
 * @param ipaddr 远程主机的IP地址
 * @return 创建连接的返回的状态信息
**/
static err_t buildNTPUDPConnection(ip_addr_t* ipaddr){
	err_t err_status;
	ntp_udp_client = udp_new();
		if( !ntp_udp_client){
			printf("create udp pcb failed\n\r");
			return ERR_VAL;
		}
		/* 先绑定udp链接 */
		err_status = udp_bind(ntp_udp_client,IP_ADDR_ANY,1233);    //绑定任意地址和1233端口
		if(err_status != ERR_OK){  //绑定失败
			printf("ntp udp bind failed\n\r");
			return ERR_VAL;
		}
		err_status = udp_connect(ntp_udp_client,ipaddr,123);   //与远程NTP服务器建立连接
		if(err_status != ERR_OK){  //创建连接失败
			printf("ntp udp create connect failed\n\r");
			return ERR_VAL;
		}
		//UDP PCB收到数据时的回调函数
		udp_recv(ntp_udp_client,get_ntp_time,NULL);
		return ERR_OK;
}
/**
 * Function: connectNTPServer
 * Description: 向NTP服务器发送请求的主调度函数，生成请求数据，向服务器发送数据，每一次请求都被假定为失败的请求
 *				如果失败次数超过一定次数则认为链路不同
 *
**/
int connectNTPServer() {
    //客户端
    //使用的NTP服务器是10.0.99.177
   	err_t err_status;
	ip_addr_t ipaddr;//定义IP类型变量
	IP4_ADDR(&ipaddr, board.ntp_server_addr[0],board.ntp_server_addr[1],board.ntp_server_addr[2],board.ntp_server_addr[3]);   //局域网测试使用的ntp服务器地址
	/* 当不存在ntp连接时或者ntp服务器改变时，需要重建连接，保证系统中只能有一个NTP连接槽 */
	if( !ntp_udp_client){  //不存在ntp连接,创建一个新的ntp udp链接
		err_status = buildNTPUDPConnection(&ipaddr);
		if(err_status != ERR_OK){
			printf("build udp connection failed\n\r");
			return 0;
		}
		
	}else{  //存在连接，检查ntp服务器的地址是否修改
		if( !ip_addr_cmp(&(ntp_udp_client->remote_ip),&ipaddr)){  //判断ntp服务器的地址发生了改变
			//先移除之前的连接，在创建一个新的连接
			//printf("Modify NTP server address,connection will rebuild\n\r");
			udp_remove(ntp_udp_client);
			err_status = buildNTPUDPConnection(&ipaddr);
			if(err_status != ERR_OK){
				printf("rebuild udp connection failed\n\r");
				return 0;
			}
		}
	}
    
    if(ntp_udp_client != NULL) {
        //uip_udp_bind(ntp_udp_conn, HTONS(1233));//绑定本地端口为1233，也就是1233－－>123 发数据
        //printf("built connection with ntp server\n\r");
        generateSendData();
		request_failed_num++;    //假定本次请求失败
		send_packet(1);  //向服务器发送NTP数据包
        send_data_length = 0; //数据发送结束，设置发送数据长度为0

    }

}
int generateSendData() {
    send_data_length = 48;
}
