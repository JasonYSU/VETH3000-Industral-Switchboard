/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\veth2000_vlan\VETH2000_vlan\hal_20141022\scr_code\cgi\incl\veth_vlan.h
@Author  : Duke Lee
@Date    : 05-Nov-2014
@Version : V1.0.00
@Breif   : CGI接口：veth_vlan.c的头文件
****************************************************************************************
**/
#ifndef  _VETH_VLAN_H__
#define  _VETH_VLAN_H__
#include <stdint.h>
#include "veth_common.h"

#define ADD_VLAN       1
#define DELETE_VLAN    0



/*vlan设置项接口结构体*/
typedef struct api_vlan_t {
uint8_t  add_or_delete;   //标注设置VLAN成员的操作为添加操作还是删除操作: '1'为添加操作，'0'为删除操作
uint16_t vlan_id;         //表示需要添加的VLAN表项的VLAN id :取值范围为0~4095
uint8_t ports_tag[PORTSNUM]; //VLAN端口成员发送tag的类型       //表示需要添加到此VLAN id 的端口 bit 0表示为端口0，bit1表示端口1 以此类推
}API_VLAN_MEM_T;


/***********************************************************************/
//函数名  ： VLANDemo
//功能    ： web的VLAN表项测试样例程序
//参数    ： None
//返回值  ： None
/***********************************************************************/
void VLANDemo(void);

/***********************************************************************/
//函数名  ： setVlanMember(API_VLAN_T *vlan_entry)
//功能    ： 设置vlan成员
//参数    ： API_VLAN_MEM_T结构体指针，调用前需定义相应的结构体实体，并填入需要设置的VLAN成员值
//返回值  ： ‘1’表示操作成功 ‘0’表示失败
/***********************************************************************/
uint8_t setVlanMember(API_VLAN_MEM_T *vlan_entry);

/***********************************************************************/
//函数名  ： displayVlanMember(API_VLAN_MEM_T *vlan_entry)
//功能    ： 获取vlan成员状态
//参数    ： API_VLAN_MEM_T结构体用来装读取到的VLAN表项目,使用方式见demo程序
//返回值  ： 1表示VTU表项中可能还有未被读取到的VLAN表条目,0表示读取到达VTU表的末端
/***********************************************************************/  
uint8_t displayVlanMembers(API_VLAN_MEM_T *vlan_entry);

/***********************************************************************/
//函数名  ： setSecurityLevel(SECURITY_LEVEL_T level,int portNo)
//功能    ： 设置VLAN端口模式
//参数    ： SECURITY_LEVEL_T level枚举体
//           portNo 表示需要设置的端口号
//返回值  ： ‘1’表示设置成功，‘0’为失败
/***********************************************************************/
uint8_t setSecurityLevel(SECURITY_LEVEL_T level,int portNo);

/***********************************************************************/
//函数名  ： setPortVlanId(uint16_t vlan_id,int portNo)
//功能    ： 设置VLAN端口的ID
//参数    ： vlan_id表示需要设置的id，portNo表示对应的端口号
//返回值  ： '1'表示设置成功，‘0’表示失败
/***********************************************************************/  
uint8_t setPortVlanId(uint16_t vlan_id,int portNo);

/***********************************************************************/
//函数名  ： setFilterdFrames(uint16_t vlan_id)
//功能    ： 设置VLAN端口的入口过滤类型
//参数    ： TAG_T my_tag为标注的面板需进行配置的参数，调用时赋值枚举值见枚举定义
//           portNo 为配置的端口号（0~9）    
//返回值  ： ‘1’表示配置成功，‘0’表示配置失败
/***********************************************************************/ 
uint8_t setFilterdFrames(TAG_T my_tag,int portNo);



#endif 

