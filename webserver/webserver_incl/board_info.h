#ifndef BOARD_H
#define BOARD_H
#include <stdio.h>
#include <string.h>
#include<time.h>
#include "veth_common.h"
#include "veth_mac.h"
#include "mac_link.h"
#include "veth_marvell_88e6097f.h"
#include "veth_port.h"
#include "veth_vlan.h"
#include "vlan.h"
#include "veth_invariate.h"
#include "ports.h"
#include "ip_addr.h"
//#include "psock.h"

#define MAX_QUEUE_NUM 4   //允许的最大队列数
#define MAX_SERIAL_NUM 4  //串口的数量
#define MAX_SERIAL_REMOTE_NUM 64  //串口能连接的最大远程主机数

#define TRUE 1
#define FALSE 0
#define MAXMACADDRNUM 1024
#define MAXVLANMEMBERNUM 1024
//#define MAXREMOTECONNECTIONNUM 20   //定义最大的远程主机连接数
#define DEFAULT_NTP_TIME_OUT 8 * 60   //默认的NTP的时间周期为8 * 60
#define QUICK_NTP_REQUEST 20           //快速的NTP请求时间间隔为20s
#define NTP_REQUEST_TIME_OUT 60   //定义NTP请求超时的时间是60s
#define NTP_REQUEST_FAILED_MAX_NUM  10   //设置NTP请求失败的最大次数为20次


#define SPEED_DISABLE 0x00000000   //速率禁用模式
#define SPEED_AUTO 		0x00000001   //速度自动模式
#define SPEED_10M_HALF 0x00000002  //10M半双工模式
#define SPEED_10M_FULL  0x00000004 //10M全双工
#define SPEED_100M_HALF  0x00000008 //100M半双工
#define SPEED_100M_FULL  0x00000010  //100M全双工
#define SPEED_1000M_FULL  0x00000020 //1G全双工

#define ELECT_2_6_CAPABILITY  (SPEED_DISABLE | SPEED_AUTO | SPEED_10M_HALF | SPEED_10M_FULL | SPEED_100M_HALF | SPEED_100M_FULL)
							  //2+6模式的电口速率性能
							  
#define LIGHT_2_6_CAPABILITY (SPEED_DISABLE | SPEED_100M_HALF | SPEED_100M_FULL)  //2+6模式的光口速率性能
#define LIGHT_2_8_CAPABILITY (SPEED_DISABLE |SPEED_1000M_FULL)  //2+6模式的光口速率性能
#define PORT_LEDAY_TIME   15  //定义端口延迟配置的时间为15s

/* 定义下载文件的种类，以此来确定下载的文件名称 */
#define VETH_ALL_CONFIG_FILE   1       //定义下载配置文件代号为1

extern unsigned char isFirstRequestPorts;
extern unsigned char isFirstRequestVlanPorts;
 struct magic_and_check{
		char magic_num[14];
		uint32_t check_sum;
	 };
typedef struct special_t {
	struct magic_and_check magic_check;
	 //magic_check.magic_num = "vorx";
	/*定义存储本设备的基本信息*/
	INVARIATE_T sets_info;
}SPECIAL_T;
#pragma pack(4)   //指定2字节对齐
typedef struct board_info{
	//定义魔数和校验和
	struct magic_and_check magic_check;
	 //magic_check.magic_num = "vorx";
	/*定义存储本设备的基本信息*/
	INVARIATE_T sets_info;
	/*定义存储用户信息的结构体*/
	struct user_info{
		char user_name[20];
		char user_password[20];
		unsigned char end; //最后一项的标志
	}user_info[5];//end struct user_info
	
	
	/* 定义存储静态mac地址表项的配置信息所需要的变量 */
	struct mac_address{  //mac地址信息结构体，用于存储到Flash中
		unsigned char disable_age; //禁止老化标志
		int age_time; 		// 老化时间
		uint16_t learn_mode;  //端口的学习模式向量
		struct mac_entry{  //某一条mac地址的信息，用于存储到Flash中
			uint8_t vlan_id;    //MAC地址属于哪个VLAN
			uint8_t  mac_addr[6];        /* MAC地址*/ 
		    uint16_t port_vector;       /* 端口向量*/ 
			uint8_t  static_dyna;        /* 静态动态标志*/
			uint8_t end;     //最后一项mac的标志 1表示最后一项
		}mac_table[MAXMACADDRNUM];//end struct mac_entry
		
		//struct mac_entry ;  //存储所有的静态mac地址，默认大小为1024
	
	}mac_address; //end struct mac_address 
	/* 定义VLAN成员信息 */

	struct vlan_member{
		uint16_t vlan_id;         //表示需要添加的VLAN表项的VLAN id :取值范围为0~4095
		char vlan_name[50];      //vlan名称
		//uint16_t port_vector;     //VLAN端口成员向量
		uint8_t port_tag[PORTSNUM]; //VLAN端口成员发送tag的类型
		uint8_t end;  //最后一个VLAN项的标志，1表示最后一项
	}vlan_member[MAXVLANMEMBERNUM];
	
	/* 定义VLAN端口配置信息 */
	
	/* 发送数据的顺序
	 *1.端口类型 2.入口过滤选项
	 *3.帧的类型 4.端口id
	 *5.发送tag  6.端口模式
	 * */
	struct vlan_port{
		uint8_t port_type;   //端口类型
		uint8_t filter_option;  //入口过滤选项
		uint8_t frame_type;     //帧类型
		uint8_t port_id;       //端口id
		//uint8_t send_tag;     //发送tag
		uint8_t port_mode;     //端口模式
	}vlan_port[PORTSNUM];  //end struct vlan_port
	
	/* 定义存储端口配置的信息所需要的变量 */
	
	struct ports_config{
		int port_speed[PORTSNUM];  //端口对应的速率
		int port_fdx[PORTSNUM];//端口全双工模式
		int port_flow[PORTSNUM]; //端口的流控选项是否开启
		int port_auto[PORTSNUM] ; //端口模式是否是自动模式
		int speed_select_enable[PORTSNUM];  //速率选择是否可用
		char port_nick[PORTSNUM][50];  //端口别名	
	}ports_config;   //end struct ports_config
	
	VETH_IP_T ip_config;
	
	/* 定义存储串口相关的参数配置 */
	struct serial_mode{
		uint16_t com1_2_mode; //串口1和串口2的模式
		uint16_t com3_4_mode; //串口3和4串口4的模式
	}serial_mode;
	struct serial_config{
		uint8_t serial_no;  //串口号 和数组的下标相对应
		
		uint8_t serial_data_bit;  //串口的数据位
		uint8_t serial_parity;// 串口校验方式
		
		//uint16_t serial_mode;  //串口的模式
		uint32_t serial_bound_rate;  //串口的波特率
		uint16_t serial_listen_port; //串口监听的端口号,存储的是网络字节格式,存放前使用HTONS进行格式化
		float serial_stop_bit; //串口停止位
	}serial_config[MAX_SERIAL_NUM];
	/* 定义远程主机相关的配置信息 */
	struct serial_remote_config{
		char remote_ip[20];  //远端的IP地址,点十进制格式存放
		uint16_t remote_port;  //远程端口号
		uint8_t  serial_no;  //对应的串口号,存放的是实际的值,和数组的下标无关，和页面上用户的选择一致
		uint8_t  is_use_udp;  //是否使用UDP协议进行传输,默认的是TCP协议传输
		uint8_t end; //记录数组结束的标识
	}serial_remote_host[MAX_SERIAL_REMOTE_NUM];
	
	char compile_date[30];  //记录程序编译的日期时间，可以用于判断程序升级是否成功
	uint8_t ntp_server_addr[4];  //NTP服务器的IP地址
	uint8_t is_config_ntp_server; //是否启用了NTP服务器
	uint8_t is_network_update;    //是否进行了网络的升级,网络升级前先设置该标志  
	
}BOARD;

typedef struct port_band_width{
	int band_width_curve_refresh_time;
	uint64_t pre_curve_rx_byte;
	uint64_t pre_curve_tx_byte;
}PORTSBANDWIDTH;
typedef struct serial_port_info{
	char remote_ip[20];
	uint16_t remote_port;
	char send_content[100];
}SERIALPORT;
/* 定义远程的主机的socket， */
typedef struct remote_serial_server_socket{
	ip_addr_t remote_ip;
	uint16_t remote_port;
	uint16_t local_port;  //本地的端口号
	uint8_t  empty;   //该socket是否为空,为空时可以赋值新的数值
}REMOTE_SOCKET;
typedef struct remote_serial_udp_socket{
	ip_addr_t remote_ip;
	uint16_t remote_port;
	uint16_t local_port; //本地的端口号
	uint8_t  empty;   //该socket是否为空,为空时可以赋值新的数值
}REMOTE_UDP_SOCKET;
/* 记录系统时间的结构体 */
typedef struct system_time{
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint32_t week;    //星期几的数字形式
	uint8_t weekday[15];  //星期几的汉字表示形式
	time_t system_sec;   //系统时间对应的秒数(从1900年至今的秒数)
}SYSTEM_TIME;
typedef struct system_run_time{
	uint32_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}SYSTEM_RUN_TIME; //系统运行的时间
extern BOARD board;
extern BOARD board_temp;
extern SERIALPORT serial_port;
extern int ports_num;
extern int vlan_time_out;
extern int band_width_refresh_time;
extern int time_out;
extern REMOTE_SOCKET remote_serial_server_socket[MAX_SERIAL_REMOTE_NUM]; //存放所有的远程连接的IP和port，用于确定是串口的连接

extern REMOTE_UDP_SOCKET remote_serial_udp_socket[MAX_SERIAL_REMOTE_NUM];

extern uint8_t download_config_file_request;   //下载配置文件的请求

extern SYSTEM_TIME system_time;
extern uint32_t time_update_interval;  //时间更新经过的秒数
extern uint32_t ntp_request_timeout;   //ntp周期性请求的时间
extern uint8_t request_failed_num;    //NTP请求失败的次数，如果失败次数过多则认为网络不通

extern uint8_t is_open_ntp_request_timer;   //是否使用了发送请求计时器
extern uint32_t ntp_send_request_time;  //记录请求发送的时刻

extern uint8_t is_need_record_time_log;   //是否需要记录时间相关的日志，当时间同步失败或者恢复时需要记录

extern uint8_t is_recover_factory;      //记录系统是否进行了恢复出厂设置操作

extern uint8_t is_reboot;  //系统是否重新启动

extern uint32_t recond_system_reboot_time_out;   //

extern time_t system_start_time;    //系统启动时的时间，用秒记录(从1900年至今的秒数)


extern uint8_t is_open_delay_config_ports;    //某个端口是否开启了延迟配置模式
extern uint8_t is_modify_port[PORTSNUM];       //端口是否被修改   
extern uint32_t delay_config_ports_time;     //延迟配置端口的计时器

extern unsigned char data__lib_config_js[];

extern uint8_t save_all_config_result[16];

extern uint8_t is_file_download;     //是否是文件下载请求

//extern int band_width_curve_refresh_time;
extern int isDataFile;

extern char pre_compile_date[30];  //记录上一次的编译日期

extern uint8_t login_status[32];  //存放用户登录时填写信息是否有误

extern struct serial_client serial_client;

void initBoardInfo();
void recoverConfigEntries(char *type);

void recoverMacConfig();

void recoverPortsConfig();
void recoverVlanConfig();
void recoverVlanPortsConfig();
void recoverSerialParameterConfig();
int  readFlashToBoard();
void printBoardInfo(char *type);
uint16_t calculateChecksum(void *p_inv,uint32_t struct_len,uint32_t offset);
void initSerialInfo();
int web_main(void);
#endif
