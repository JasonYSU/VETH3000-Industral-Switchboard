/**
*************************************************************************
@File   : hal\src_code\veth_InVariate.h
@Author : Duke Lee
@Date   : 13-Oct-2014
@Brief  : The header file for veth_InVariate.c
*************************************************************************
**/

#ifndef _VETH_INVARIATE_H__
#define _VETH_INVARIATE_H__

#include <stdint.h>
//#include "veth_hal_init.h"
/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define FLASH_TARG_SET   0x01
#define FLASH_TARG_RESET 0x00

typedef struct veth_struct_t {         /*Store the basic information for the VETH2000 sets*/
    char The_type[30];
	char The_Author[30];
	char The_Soft[30];
	char The_Version[30];	  
}VETH_T;

typedef struct veth_ip_t {
	uint8_t ip_addr[4];
	uint8_t route_addr[4];
	uint8_t net_mask[4];
	uint8_t vlan_id;
}VETH_IP_T;

typedef struct vlan_member_t{
	uint16_t vlan_id;         //表示需要添加的VLAN表项的VLAN id :取值范围为0~4095
	//char vlan_name[50];      //vlan名称
	//uint16_t port_vector;     //VLAN端口成员向量
	uint8_t port_tag[10]; //VLAN端口成员发送tag的类型
	//uint8_t end;  //最后一个VLAN项的标志，1表示最后一项
  }VLAN_MEM_T;


typedef struct vlan_port_t{
	uint8_t port_type;   //端口类型
	uint8_t filter_option;  //入口过滤选项
	uint8_t frame_type;     //帧类型
	uint8_t port_id;       //端口id
	uint8_t port_mode;     //端口模式
}VLAN_PORT_T;

typedef struct bridge_info_t {
	uint16_t bridge_pri;
	uint16_t bridge_hellot;
	uint16_t bridge_max_age;
	uint16_t bridge_fw_delay;
}RSTP_INFO_T;

typedef struct user_info_t {
	char username[20];
	char password[20];
}USER_INFO_T;

/*Define a enum for modifying the RSTP instanse information*/
typedef enum rstp_info {
BRIDGE_PRIORITY,
BRIDGE_HELLOTIME,
BRIDGE_MAXAGE,
BRIDGE_FWD_DELAY
}RSTP_INFO_E;

typedef struct InVariate_struct_t  {   /*Store these parameters to some area or Get them out from my FLASH area I select*/
    uint8_t     The_MAC[6];
    VETH_T      The_Info;
	VETH_IP_T   The_ip;
	VLAN_MEM_T  v_mem;
	VLAN_PORT_T my_port[2];
	RSTP_INFO_T my_rstp;
	USER_INFO_T my_user;
}INVARIATE_T;
 

 
/*The invariate operation*/
uint8_t Update_InVariate(void *p_inv,uint32_t struct_len);
uint8_t setNetworkUpgradeTarg(uint8_t targ);
uint8_t getNetworkUpgradeTarg(uint8_t *ptarg);
uint8_t updateNewConfig(void);
uint8_t Get_InVariate(void *p_inv,uint32_t struct_len);
uint8_t Init_InVariate();
void InVariate_Default(INVARIATE_T *invarite);
uint8_t modifyRstpInfo(INVARIATE_T *invarite,char *p_num,RSTP_INFO_E info);
uint8_t modifyIpAddr(VETH_IP_T *ip,char *p_num);
uint8_t modifyPassWord(INVARIATE_T *invarite,char *p_num);
uint8_t modifyUserName(INVARIATE_T *invarite,char *p_num);
void displayRstpInfo(INVARIATE_T *invarite);
void displayIpInfo(VETH_IP_T *ip);
void displayBasicInfo(INVARIATE_T *inv);
/******************************************************************/
//函数名  :   updateInvariate(void *p_inv,uint16_t struct_len);
//功能    :   将配置信息保存到flash中去 
//参数    :   p_inv(Input) : 指向待保存的数据集合
//            struct_len   : 所指向的数据集合的长度（单位：字节）
//返回值  :   uint8_t 类型 ： ‘1’表示更新成功 ，‘0’表示更新失败，可能是FLASH的操作出了问题
/******************************************************************/	
uint8_t updateInvariate(void *p_inv,uint32_t struct_len);

/******************************************************************/
//函数名  :   updateInvariate(void *p_inv,uint16_t struct_len);
//功能    :   将配置信息从FLASH中读取出来 
//参数    :   p_inv(Input) : 指向数据集合所定义的全局变量，用来存放读取到的数据
//            struct_len   : 所指向的数据集合的长度（单位：字节）
//返回值  :   uint8_t 类型 ： ‘1’表示更新成功 ，‘0’表示更新失败，可能是FLASH的操作出了问题
/******************************************************************/
uint8_t getInvariate(void *p_inv,uint32_t struct_len);


#endif

