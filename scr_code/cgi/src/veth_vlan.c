/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\veth2000_vlan\VETH2000_vlan\hal_20141022\scr_code\cgi\src\veth_vlan.c
@Author  : Duke Lee
@Date    : 05-Nov-2014
@Version : V1.0.00
@Breif   : CGI接口：veth_vlan.c
****************************************************************************************
**/

#include <stdint.h>
#include <stdio.h>

#include "veth_common.h" 
#include "veth_marvell_88e6097f.h"
#include "veth_vlan.h"
#include "veth_mac.h"
/***********************************************************************/
//函数名  ： VLANDemo
//功能    ： web的VLAN表项测试样例程序
//参数    ： None
//返回值  ： None
/***********************************************************************/
void VLANDemo()
  {
	VLAN_ENTRY_T    entrys;
	API_VLAN_MEM_T  vlan_entry;
//	uint8_t   i;
//	uint16_t  tem_reg;
	uint8_t   loop;	
	uint16_t  counter = 0;
	
//	Set_Age_time(150);
/*添加VLAN 2 成员项_端口0、2、4、6、8*/
	vlan_entry.add_or_delete = ADD_VLAN;
/*使用前对vlan_entry.mem所有成员项清0*/
//	for(loop = 0 ;loop <10;loop++)
//	  {vlan_entry.mem[loop] = 0x00;}
/**填写待添加的成员项的内容**/
//	for(loop = 0 ;loop <9 ; loop+=2)
//	  {
//	    vlan_entry.mem[loop] = 0x03;
//	  }
//	vlan_entry.vlan_id       = 2;
//	setVlanMember(&vlan_entry);
/*添加VLAN 3 成员项_端口1、3、5、7、9*/	
//	for(loop = 0 ;loop <10;loop++)
//	  {vlan_entry.mem[loop] = 0x00;}
//	vlan_entry.add_or_delete = ADD_VLAN;
//	for( loop =1; loop< 10; loop+=2)
//	  {
//		vlan_entry.mem[loop] = 0x03;
//	  }
//	vlan_entry.vlan_id       = 3;
//	setVlanMember(&vlan_entry);
/*显示VLAN成员项*/
	entrys.VTU_vid  = 0x0fff;
	Read_VTU_entry(&entrys);
	
	vlan_entry.vlan_id = 0x0fff;
	while(displayVlanMembers(&vlan_entry) != 0)
	  {
	    counter++;
	  }
	printf("The number of VTU entry is: %d \n\r",counter);
/*设置端口类型*/
	for(loop = 0 ;loop< 10 ;loop++)
	 {
		setSecurityLevel(SECURITY,loop);
	 }
/*设置端口的VLANID*/
	for (loop= 0;loop< 10;loop++)
	  {
		if(loop%2)
		  {
		   setPortVlanId(0x0003,loop);
		  }
		else
		  {
		    setPortVlanId(0x0002,loop);
		  }
	  }
/*设置端口0过滤类型*/
	setFilterdFrames(ALL_FRAME,0);
/*读取VLAN成员*/	
	entrys.VTU_vid  = 0x0fff;
	Read_VTU_entry(&entrys);	
#ifdef MY_DEBUG_P			
	printf("The read entry VID: \n\r");
	printf("%x \n\r",entrys.VTU_vid );
	printf("The read entry DATA:\n\r");
	printf("%x \n\r",entrys.VTU_data0);
	printf("%x \n\r",entrys.VTU_data1);
	printf("%x \n\r",entrys.VTU_data2);			
#endif 
  }
  
/***********************************************************************/
//函数名  ： setVlanMember(API_VLAN_T *vlan_entry)
//功能    ： 设置vlan成员
//参数    ： API_VLAN_MEM_T结构体指针，调用前需定义相应的结构体实体，并填入需要设置的VLAN成员值
//返回值  ： ‘1’表示操作成功 ‘0’表示失败
/***********************************************************************/
uint8_t setVlanMember(API_VLAN_MEM_T *vlan_entry)
  {
	return Creat_Delet_VLAN(vlan_entry->vlan_id,vlan_entry->ports_tag,vlan_entry->add_or_delete);
  }

/***********************************************************************/
//函数名  ： displayVlanMember(API_VLAN_MEM_T *vlan_entry)
//功能    ： 获取vlan成员状态
//参数    ： API_VLAN_MEM_T结构体用来装读取到的VLAN表项目,使用方式见demo程序
//返回值  ： 1表示VTU表项中可能还有未被读取到的VLAN表条目,0表示读取到达VTU表的末端
/***********************************************************************/  
uint8_t displayVlanMembers(API_VLAN_MEM_T *vlan_entry)
  {
	uint8_t loop;
	VLAN_ENTRY_T vtu_entry;
	uint8_t data_tag;

	/*Read the VTU entry out*/
	vtu_entry.VTU_vid = vlan_entry->vlan_id;
	Read_VTU_entry(&vtu_entry);
	vtu_entry.VTU_data2 = vtu_entry.VTU_data2 << 8;
	/**/
	if(vtu_entry.VTU_vid == 0x0fff)    //PS: 0x0fff表示VLAN表访问到达末端
	  {
		return 0;
	  }
	vlan_entry->vlan_id = vtu_entry.VTU_vid;
#ifdef BIT_WAY
	vlan_entry->port_vector = 0x0000;
	/*移位获取port_vector向量的值*/
	for (loop = 0 ;loop<10 ;loop++ )
	  {
		if( loop <=1 && loop >=0 )
		  {
			data_tag = 0;
		  }
		else
		  if( loop >=2 && loop <= 5 )
			{
			  data_tag = 1;
			}
		else
		  {
			data_tag =2;
		  }
	  switch(data_tag) 
		{
		  case 0 :
			transferVLANMember(&vtu_entry.VTU_data2,&vlan_entry->port_vector,loop);
			break;
		  case 1 :
			transferVLANMember(&vtu_entry.VTU_data1,&vlan_entry->port_vector,loop);
			break;
		  case 2 :
			transferVLANMember(&vtu_entry.VTU_data0,&vlan_entry->port_vector,loop);
			break;
		  default:
			break;
		}//end of switch
		tem_vector = vlan_entry->port_vector;
	  }//end of loop circle
#endif
	for(loop = 0 ;loop<10 ;loop++ )
	  {
		vlan_entry->ports_tag[loop] = 0x00;
	  }
	/*移位获取port_vector向量的值*/
	for (loop =0 ;loop <10;loop++ )
	  {
		if( loop <=1 && loop >=0 )
		  {
			data_tag = 0;
		  }
		else
		  if( loop >=2 && loop <= 5 )
			{
			  data_tag = 1;
			}
		else
		  {
			data_tag =2;
		  }
	  switch(data_tag) 
		{
		  case 0 :
			transferVLANMember(&vtu_entry.VTU_data2,vlan_entry->ports_tag,loop);
			break;
		  case 1 :
			transferVLANMember(&vtu_entry.VTU_data1,vlan_entry->ports_tag,loop);
			break;
		  case 2 :
			transferVLANMember(&vtu_entry.VTU_data0,vlan_entry->ports_tag,loop);
			break;
		  default:
			break;
		}//end of switch
	  }//end of loop circle
	return 1;
  }

/***********************************************************************/
//函数名  ： setSecurityLevel(SECURITY_LEVEL_T level,int portNo)
//功能    ： 设置VLAN端口模式
//参数    ： SECURITY_LEVEL_T level枚举体
//           portNo 表示需要设置的端口号
//返回值  ： ‘1’表示设置成功，‘0’为失败
/***********************************************************************/
uint8_t setSecurityLevel(SECURITY_LEVEL_T level,int portNo)
  {
	return Set_VLAN_port_Type(level,portNo);
  }

/***********************************************************************/
//函数名  ： setPortVlanId(uint16_t vlan_id,int portNo)
//功能    ： 设置VLAN端口的ID
//参数    ： vlan_id表示需要设置的id，portNo表示对应的端口号
//返回值  ： '1'表示设置成功，‘0’表示失败
/***********************************************************************/  
uint8_t setPortVlanId(uint16_t vlan_id,int portNo)
  {
	return Set_VLAN_ID(vlan_id,portNo);
  }

/***********************************************************************/
//函数名  ： setFilterdFrames(uint16_t vlan_id)
//功能    ： 设置VLAN端口的入口过滤类型
//参数    ： TAG_T my_tag为标注的面板需进行配置的参数，调用时赋值枚举值见枚举定义
//           portNo 为配置的端口号（0~9）    
//返回值  ： ‘1’表示配置成功，‘0’表示配置失败
/***********************************************************************/ 
uint8_t setFilterdFrames(TAG_T my_tag,int portNo)
  {
	return Set_VLAN_ingress_Type(my_tag,portNo);
  }


