/*******************************************************
File name: board_info.c
Description: 对板子进行一些初始化工作，在设备重新启动以后，需要调用该文件中的方法恢复之前对设备的配置
Author: dujiancheng
Version: v1.0
********************************************************/

#include "board_info.h"
#include "veth_COM.h"
#include "def.h"
#include "crc16.h"
#include "system_info.h"
int ports_num;   //全局extern变量，存放改设备对应的端口数
uint8_t download_config_file_request = 0;   //判断是否是文件下载请求
uint8_t is_open_delay_config_ports;    //是否开启了延迟配置端口模式
uint8_t is_modify_port[PORTSNUM];       //端口是否被修改  
uint32_t delay_config_ports_time;     //延迟配置端口的计时器
/**
 * Function: initBoardInfo
 * Description: 该函数用于初始化板子配置信息，当板子第一次运行或者恢复出厂设置时被调用
 * 
**/
void initBoardInfo() {
    int i;
	char system_compile_date[30];
    /* 初始化魔数 */
    strcpy(board.magic_check.magic_num,the_magic);
    /*初始化设备基本信息*/
    InVariate_Default(&board.sets_info);
    /*初始化IP信息*/
    board.ip_config = SET_IP;
    /* 初始化用户信息 */
    strcpy(board.user_info[0].user_name,"vorx");
    strcpy(board.user_info[0].user_password,"vorx");
    board.user_info[0].end = 0;
    board.user_info[1].end = 1;

    /* 初始化MAC地址表的配置信息 */
    board.mac_address.age_time = 330;
    board.mac_address .disable_age = 0;
    board.mac_address.learn_mode = 0x00;
    board.mac_address.mac_table[0].end = 1;
    /* 结束mac地址表的配置 */

    /* 初始化VLAN表的配置 ,应该遍历该数组*/
    board.vlan_member[0].vlan_id = 1;
    strcpy(board.vlan_member[0].vlan_name,"默认");
    //board.vlan_member[0].port_vector = 0xff;
    memset(board.vlan_member[0].port_tag,1,sizeof(board.vlan_member[0].port_tag));

    board.vlan_member[0].end = 0;
    board.vlan_member[1].end = 1;


    /*初始化VLAN端口的配置*/
    for(i = 0; i < PORTSNUM; i++) {
        board.vlan_port[i].port_type = 0;
        board.vlan_port[i].filter_option = 0;
        board.vlan_port[i].frame_type = 0;
        board.vlan_port[i].port_id = 1;
        //board.vlan_port[i].send_tag = 0;
        board.vlan_port[i].port_mode = 1;
    }
    /* 初始化port的配置信息 */
	//board.port_nick = (char**)malloc(PORTSNUM * sizeof(char*));
    for(i = 0; i < PORTSNUM; i++) {
        //board.port_nick[i] = (char*)malloc(50 * sizeof(char));
		memset(board.ports_config.port_nick[i],0,50 * sizeof(char));
		strcpy(board.ports_config.port_nick[i],"默认");
        board.ports_config.port_flow[i] = 0;
        board.ports_config.port_auto[i] = 1;
        board.ports_config.speed_select_enable[i] = 1;
        board.ports_config.port_speed[i] = 10;
        board.ports_config.port_fdx[i] = 0;
        //printf("nick is %s\n\r",board.ports_config.port_nick[i]);
    }

    /* 结束port的配置信息 */
    //配置子网掩码
    board.ip_config.vlan_id = 1;
    /* 结束IP地址的配置 */

    /* 初始化串口服务器相关的信息 */
    for(i = 0; i < MAX_SERIAL_NUM; i++) {  //对所有串口参数进行初始化
        board.serial_config[i].serial_no = i;
        board.serial_config[i].serial_bound_rate = 115200;
        board.serial_config[i].serial_data_bit = 8;
        board.serial_config[i].serial_listen_port = 8089 + i;
        board.serial_config[i].serial_parity = 0;
        board.serial_config[i].serial_stop_bit = 1.0;
        //printf("Init port is %d\n\r",board.serial_config[i].serial_listen_port);
    }
    board.serial_mode.com1_2_mode = 232;
    board.serial_mode.com3_4_mode = 232;
    /* 初始化远程主机为空 */
    for(i = 0; i < MAX_SERIAL_NUM; i++) {
        board.serial_remote_host[i].end = 1;
        memset(board.serial_remote_host[i].remote_ip,0,sizeof(board.serial_remote_host[i].remote_ip));
        board.serial_remote_host[i].remote_port = 0;
        board.serial_remote_host[i].serial_no = i;
		board.serial_remote_host[i].is_use_udp = 0;
    }

    /* 结束对串口服务器信息的初始化 */
	generateSystemCompileDate(system_compile_date);
	strcpy(board.compile_date,system_compile_date);
	/* 初始化系统的编译日期 */
	board.ntp_server_addr[0] = 192;
	board.ntp_server_addr[1] = 168;
	board.ntp_server_addr[2] = 1;
	board.ntp_server_addr[3] = 210;
	board.is_config_ntp_server = 0;  //默认没有开启NTP服务器
	/*初始化NTP服务器地址*/
	is_recover_factory = 1;  //该函数的执行说明系统进行了初始化设置
}
/**
 * Function: initSerialInfo
 * Description: 该函数用于初始化串口的配置信息，当设备重启之后改函数被调用
 * 
**/
void initSerialInfo() {
    int i;
    for(i = 0; i < MAX_SERIAL_REMOTE_NUM; i++) {
        remote_serial_server_socket[i].empty = 1;
        remote_serial_server_socket[i].remote_port = -1;
    }
    recoverConfigEntries("config_serial_parameter");
}
/**
 * Function: readFlashToBoard
 * Description: 该函数用于从Flash中读取保存的设备配置信息，当设备重启之后改函数被调用
 * 
**/
int readFlashToBoard() {
    getInvariate(&board,sizeof(board));
}
/**
 * Function: recoverConfigEntries
 * Description: 该函数用于恢复之前对设备的各种配置，当设备重启之后改函数被调用
 * 
**/
void recoverConfigEntries(char *type) {
    if( !strcmp(type,"config_mac")) { //恢复断电前对mac表的配置信息
        recoverMacConfig();
    } else if( !strcmp(type,"config_ports")) { //恢复断电前对ports的配置信息
        recoverPortsConfig();
    } else if( !strcmp(type,"config_vlan")) { //恢复断电前对VLAN的配置信息
        recoverVlanConfig();
    } else if( !strcmp(type,"config_vlan_ports")) {
        recoverVlanPortsConfig();
    } else if( !strcmp(type,"config_serial_parameter")) { //恢复对串口参数的配置
        recoverSerialParameterConfig();
    }
}
/**
 * Function: recoverVlanPortsConfig
 * Description: 该函数用于恢复断电前对VLAN端口的配置，该方法被recoverConfigEntries调用
 * 
**/
void recoverVlanPortsConfig() {
    /* 调用底层函数向交换芯片中写入信息 */
    modifySwitchVlanPorts();
    //printf("recover vlan ports set sucessful\n\r");
}
/**
 * Function: recoverVlanConfig
 * Description: 该函数用于恢复断电前对VLAN的配置，该方法被recoverConfigEntries调用
 * 
**/
void recoverVlanConfig() {
    API_VLAN_MEM_T vlan_entry;
    int value;
    int i;
    for(i = 0; i < MAXVLANMEMBERNUM; i++) {
        memset(vlan_entry.ports_tag,0,sizeof(vlan_entry.ports_tag));
        if(board.vlan_member[i].end == 1) {
            break;
        }
        vlan_entry.add_or_delete = ADD_VLAN;
        vlan_entry.vlan_id = board.vlan_member[i].vlan_id;
        memcpy(vlan_entry.ports_tag,board.vlan_member[i].port_tag,sizeof(board.vlan_member[i].port_tag));
        value = setVlanMember(&vlan_entry);
        if(value) {
            //printf("recover VLAN %d to switch chip sucessful\n\r",vlan_entry.vlan_id);
        }
    }

}

/**
 * Function: recoverMacConfig
 * Description: 该函数用于恢复断电之前对MAC地址的配置，该方法被recoverConfigEntries调用
 * 
**/
void recoverMacConfig() {
    int i;
    unsigned char sucess = 0;
    MAC_API_T mac_entry;
//	age_time  = board.mac_address.age_time;  //得到用户上一次设置的老化时间值
    if(board.mac_address.disable_age) { //设置禁用老化时间
        Set_Age_time(0);
    } else {  //设置老化时间
        Set_Age_time(board.mac_address.age_time);
    }
    Set_Manual_learning_mode(board.mac_address.learn_mode); //设置端口的学习模式
    /* 添加静态的MAC地址项 */
    for(i = 0; i < MAXMACADDRNUM; i++) {
        if(board.mac_address.mac_table[i].end) {
            break;
        }
        memcpy(mac_entry.new_mac,board.mac_address.mac_table[i].mac_addr,6);
        mac_entry.add_or_delet = 1;
        mac_entry.vlan_id = board.mac_address.mac_table[i].vlan_id;
        mac_entry.port_vector = board.mac_address.mac_table[i].port_vector;
        sucess = Add_or_Delet_MACentry(&mac_entry);
        if(sucess) {
            //printf("mac address id is %d\n\r",board.mac_address.mac_table[i].vlan_id);
            //printf("recover mac address sucessful!\n\r");
        } else {
            //printf("recover mac address failed!\n\r");
        }
    }
    //printf("recover mac address sucessful\n\r");
}

/**
 * Function: recoverPortsConfig
 * Description: 该函数用于恢复断电前对ports的配置信息，该方法被recoverConfigEntries调用
 * 
**/
void recoverPortsConfig() {
    int i;
    for(i = 0; i < PORTSNUM; i++) {
        //只需要恢复流控状态，不需要恢复速率，因为速率是时时获取
        Set_flow_control(i,board.ports_config.port_flow[i]);
        //printf("nick is %s\n\r",board.ports_config.port_nick[i]);
    }
}
#if 0
/**
 * Function: printBoardInfo
 * Description: 该函数用于显示当前对板子的配置信息，用于调试时调用
 * 
**/
void printBoardInfo(char *type) {
    int i,j;
    if(!strcmp(type,"config_mac")) { //打印board结构体中的mac地址信息
        printf("******mac address info********\n\r");
        printf("disable age is %d\n\r",board.mac_address.disable_age);
        printf("age time is %d\n\r",board.mac_address.age_time);
        printf("learn mode is %X\n\r",board.mac_address.learn_mode);
        printf("All static mac address entries\n\r");
        for(i = 0; i < MAXMACADDRNUM; i++) {
            if(board.mac_address.mac_table[i].end == 1) {
                break;
            }
            for(j = 0; j < 6; j++) {
                printf("%02X-",board.mac_address.mac_table[i].mac_addr[j]);
            }
            printf(";%X",board.mac_address.mac_table[i].port_vector);
            printf("\n\r");
        }
        printf("******end mac address info display********\n\r");
    } else if( !strcmp(type,"config_ports")) { //打印board结构体中的ports配置信息
        printf("******ports config info ************\n\r");
        for(i = 0; i < PORTSNUM; i++) {
            printf("port %d;port_nick is %s;port_flow is %d;port_auto is %d;enable is %d;port_speed is %d;port_fdx is %d\n\r",
                   i + 1,board.ports_config.port_nick[i],
                   board.ports_config.port_flow[i],
                   board.ports_config.port_auto[i],
                   board.ports_config.speed_select_enable[i],
                   board.ports_config.port_speed[i],
                   board.ports_config.port_fdx[i]);
        }
        printf("******end ports config info display ************\n\r");
    }
}
#endif

/**
 * Function: recoverSerialParameterConfig
 * Description: 该函数用于恢复断电前对本地串口参数的配置，该方法被recoverConfigEntries调用
 * 
**/
void recoverSerialParameterConfig() {
    int i;
    int retValue;
    if(board.serial_mode.com1_2_mode == 232) {
        setChipMode(0,RS232_MODE);
    } else {
        setChipMode(0,RS485_MODE);
    }
    if(board.serial_mode.com3_4_mode == 232) {
        setChipMode(1,RS232_MODE);
    } else {
        setChipMode(1,RS485_MODE);
    }
    for(i = 0; i < MAX_SERIAL_NUM; i++) {
        retValue = setComProperty(&board.serial_config[i]);
        if(retValue != 1) {
            printf("recover serial %d failed\n\r",i + 1);
        }
    }
    //printf("All serial recover sucessful\n\r");
}
/**
 * Function: calculateChecksum
 * Description: 使用CRC16算法实现对数据的校验，在该项目中主要用于计算board结构体中的数据有没有变化，决定是否要
 * 向Flash中更新数据
 * @param p_inv 要计算的数据指针，为任意数据类型
 * @param struct_len 是要计算的数据的大小
 * @param offset 计算的起始位置
 * @return 计算校验和的结果
**/
uint16_t calculateChecksum(void *p_inv,uint32_t struct_len,uint32_t offset) {
    uint16_t checksum;
    checksum = 0;
    checksum = doCRC16(0,offset,struct_len,(uint8_t*)p_inv);
    return checksum;
}
