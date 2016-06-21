/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\hal\incl\veth_mac.h
@Author  : Duke Lee
@Date    : 18-Oct-2014
@Version : V1.0.00
@Breif   : CGI�ӿڣ�MAC�����ͷ�ļ� MAC表项的头文件
****************************************************************************************
**/

#ifndef _VETH_MAC_H__
#define _VETH_MAC_H__ 

#include <stdint.h>
#include "veth_common.h"

#define ENTRY_STATIC    0
#define ENTRY_DYNAMIC   1

typedef struct mac_api_t {
	uint8_t  add_or_delet;      /* '0' 为删除 ,'1'  为添加 */ //          
	uint8_t  new_mac[6];        /* 存放新添加删除或者读取到的entry的MAC地址*/ 
	uint16_t port_vector;       /* 存放添加或者读取到的端口向量*/ 
	uint8_t  sta_or_dym;        /* 存放静态动态标志*/  //ENTRY_STATIC for static entrys ENTRY_DYNAMIC for dynamic entrys   //ֻ��״̬�������õ�
	uint8_t  vlan_id;           /* 存放vlanid*/
}MAC_API_T;


/***********************************************************************/
//函数名   :  Set_Aging_time(uint16_t age_time)
//功能     :  设置老化时间
//参数     :  Age_time(Input)__老化时间取值范围应该为15的倍数（0~3825），否则设置的老化时间可能跟所输入的时间有出入
//                          PS：将老化时间设置为0表示禁止老化
//返回值   ： None
/***********************************************************************/
void Set_Age_time(uint16_t age_time);

/***********************************************************************/
//函数名     :  Get_Aging_time()
//功能       :  获取MAC表格的当前设置时间
//参数       ： None
//返回值     ： uint16_t 获取的MACentry老化时间
/***********************************************************************/
uint16_t Get_Age_time(void);

/***********************************************************************/
//函数名  ：  Add_or_Delet_MACentry(MAC_API_T *entry)
//功能    ：  添加或者删除一条静态MAC地址项
//参数    ：  entry，指向一个MAC_API_T ,用来传递需要添加或者删除的MAC entry 
//返回值  ：  uint8_t '1' 添加或者删除成功,'0' 添加/删除失败 
/***********************************************************************/
uint8_t Add_or_Delet_MACentry(MAC_API_T *entry);

/***********************************************************************/
//函数名    ： Display_All_MACentry(MAC_API_T *entry,MAC_ENTRY_T *mac_entry)
//功能      ： 查找所MAC表中的MAC entry 使用前需填入MAC_ENTRY_T 中的初始MAC值        
//参数      ： MAC_API_T *entry用来存放读取到的MAC_entry信息,所需信息见MAC_ENTRY_T 结构体定义
//             MAC_ENTRY_T *mac_entry用来存放每次访问MAC表的初始MAC地址，也就是：
//	  for(loop = 0 ;loop < 6; loop++ )
//	   {
//	      mac_entry->mac_entry_MAC[loop] = 0xff;  //MAC{0xff,0xff,0xff,0xff,0xff,0xff} used to get the first ATU entry
//		 }	
/***********************************************************************/	 
uint8_t Display_ALL_MACentry(MAC_API_T *entry,MAC_ENTRY_T *mac_entry,uint16_t vlan_id);

/***********************************************************************/
//函数名    : Flush_MAC_entry()
//功能      ：清除掉MAC表中的所有非静态的MAC entrys
//参数      ：None
//返回值    ：None
/***********************************************************************/
void Flush_MAC_entry(void);

/***********************************************************************/
//函数名        : Set_MAC_forbid(uint16_t port_vector)
//功能          ：设置端口MAC学习禁止模式
//参数          ：port_vector : 端口向量(0~9)
//返回值        ：None
/***********************************************************************/	
void Set_MAC_forbid(uint16_t port_vector);

/***********************************************************************/
//函数名        : Get_MAC_forbid()
//功能          ：获取端口MAC学习禁止模式的端口向量
//参数          ：None
//返回值        ：禁止模式的端口向量
/***********************************************************************/	
uint16_t Get_MAC_forbid(void);

/***********************************************************************/	
//函数名      	:	Dispaly_sta_MAC_entry(MAC_API_T *entry,MAC_ENTRY_T *mac_entry)
//功能          : 获取静态 ATU(:MAC entry table)项
//参数          : MAC_API_T  : 存放获得的MAC entry相关信息
//                MAC_ENTRY_T: 存放当前存放的MAC值，初次调用需对 MAC_ENTRY_T.mac_entry_MAC填入初值{0xff,0xff,0xff,0xff,0xff,0xff}
//                vlan_id    : MAC entry 对应的vlan id
//返回值        : '0' 表示到达了ATU(:MAC entry table)结尾 ,‘1’表示还可能有未读取到的静态MAC entry 
/***********************************************************************/
uint8_t Display_sta_MAC_entry(MAC_API_T *entry,MAC_ENTRY_T *mac_entry,uint16_t vlan_id);

/***********************************************************************/
//函数名   : MAC_demo()
//功能     ：测试所有编写的MAC表项接口函数
//参数     ：None
//返回值   ：None
/***********************************************************************/
void MAC_demo(void);

#endif

