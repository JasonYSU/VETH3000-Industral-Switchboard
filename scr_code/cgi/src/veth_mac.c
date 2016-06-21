/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\hal\src_code\cgi\src\veth_mac.c
@Author  : Duke Lee
@Date    : 18-Oct-2014
@Version : V1.0.00
@Breif   : CGI接口： MAC表项
****************************************************************************************
**/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "veth_common.h"
#include "veth_marvell_88e6097f.h"
#include "veth_mac.h"
#include "veth_vlan.h"

#define ADD_MAC  1
#define DEL_MAC  0
#define GET_LOW_4BITS 0x000f


uint8_t Targ_MAC[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

//内部函数声明
static void macDecreaseByOne(uint8_t *pMAC);

/***********************************************************************/
//函数名   :  Set_Aging_time(uint16_t age_time)
//功能     :  设置老化时间
//参数     :  Age_time(Input)__老化时间取值范围应该为15的倍数（0~3825），否则设置的老化时间可能跟所输入的时间有出入
//                          PS：将老化时间设置为0表示禁止老化
//返回值   ： None
/***********************************************************************/
void Set_Age_time(uint16_t age_time)
   {
      uint8_t tem;	 
/*计算switch所需要的age格式*/
	 tem =  ( uint8_t ) (age_time/ 15);
/*设置switch的老化时间*/
     set_aging_time(tem);
   }
	 
/***********************************************************************/
//函数名     :  Get_Aging_time()
//功能       :  获取MAC表格的当前设置时间
//参数       ： None
//返回值     ： uint16_t 获取的MACentry老化时间
/***********************************************************************/
uint16_t Get_Age_time()
   {
	 return get_aging_time()*15; 
   }	 

/***********************************************************************/
//函数名  ：  Add_or_Delet_MACentry(MAC_API_T *entry)
//功能    ：  添加或者删除一条静态MAC地址项
//参数    ：  entry，指向一个MAC_API_T ,用来传递需要添加或者删除的MAC entry 
//返回值  ：  uint8_t '1' 添加或者删除成功,'0' 添加/删除失败 
/***********************************************************************/
uint8_t Add_or_Delet_MACentry(MAC_API_T *entry)
  {
	MAC_ENTRY_T  MAC_entry;
	uint8_t      loop; 
/*填入新添加的MAC地址*/		
	for(loop= 0 ; loop<6 ; loop++)
     {
         MAC_entry.mac_entry_MAC[loop]  = entry->new_mac[loop];
	 }
	MAC_entry.mac_entry_oper = LOAD_PURGE_ENTRY_FID;
	if(entry->add_or_delet == ADD_MAC)
	 {
        MAC_entry.mac_entry_data = 0x0e | (entry->port_vector << 4) | 0x0000; //bit[15](trunk):'0'; bits[14:4]:port_vector; bits[3:0]:0x0e means the static entry ;
		MAC_entry.mac_entry_fid  = entry->vlan_id;
		/*Load the entry*/
		Load_Purge_ATUentry(&MAC_entry);
        /*Read it out to ensure the load operation*/
		macDecreaseByOne(&(MAC_entry.mac_entry_MAC[0]));
		Read_ATU_entry(&MAC_entry);
		/*Judge the read out entry,then compare it with the newly add one*/
		if(Str_cmp(MAC_entry.mac_entry_MAC,entry->new_mac,6) == 0 ) //The operation success
			{return 1;}
		else 
			{return 0;}
	 }//end of if
	 else     //entry->add_or_delet == DELETE_MAC
	 {
	  MAC_entry.mac_entry_data = 0x00 | (entry->port_vector << 4) |0x0000;
	  MAC_entry.mac_entry_fid  = entry->vlan_id;
	  Load_Purge_ATUentry(&MAC_entry);
	 /*Read it out to ensure the delete operation success*/
	  macDecreaseByOne(&(MAC_entry.mac_entry_MAC[0]));
	  Read_ATU_entry(&MAC_entry);
	 /*Judge the read out entry ,then compare it with the newly delete one*/
	  if(Str_cmp(MAC_entry.mac_entry_MAC,entry->new_mac,6) != 0 ) //The delete operation success
	    {return 1;}
	  else 
	    {return 0;}
     }//end of else


  }	 
	 
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
uint8_t Display_ALL_MACentry(MAC_API_T *entry,MAC_ENTRY_T *mac_entry,uint16_t vlan_id)
{
	uint8_t loop;
	
	mac_entry->mac_entry_fid = vlan_id;
	Read_ATU_entry(mac_entry);
	if(  !(Str_cmp(mac_entry->mac_entry_MAC,Targ_MAC,6))  ) //读取到{ff-ff-ff-ff-ff-ff}表示MAC表访问完毕
      {return 0;}	
/*将读取到的MAC值填入API_ENTRY_T*/ 
#ifdef MY_DEBUG //The debug mode select
    printf("The read out MAC entry: ");
    for(loop =0 ;loop<6 ;loop++ )
	  {
	    entry->new_mac[loop] = mac_entry->mac_entry_MAC[loop];
		printf("%x.",entry->new_mac[loop]);
	  }
	printf("\n\r");
#else   //j
	for(loop = 0 ; loop <6 ; loop++)
	  {
		entry->new_mac[loop] = mac_entry->mac_entry_MAC[loop];
	  }
#endif			 
			
/*填入读取到的静态动态标志*/
	if((mac_entry->mac_entry_data & 0x0f) <= 0x07)
	  {
		entry->sta_or_dym = ENTRY_DYNAMIC;
	  }
	else
	  {
		entry->sta_or_dym = ENTRY_STATIC;					
	  }
/**/
#ifdef MY_DEBUG
     printf("The sta_or_dym targ is : %d\n\r",entry->sta_or_dym);
#endif				
/*填入读取到的端口向量*/
	entry->port_vector = ((mac_entry->mac_entry_data) >>4);
/*填入读取的vlan_id*/
	entry->vlan_id  = mac_entry->mac_entry_fid;
//	printf("The entry FID is: %d\n\r",entry->vlan_id);
		return 1;
}	


/***********************************************************************/
//函数名    : Flush_MAC_entry()
//功能      ：清除掉MAC表中的所有非静态的MAC entrys
//参数      ：None
//返回值    ：None
/***********************************************************************/
void Flush_MAC_entry()
  {
    Flush_Move_ATUentry(ALL_NONSTA,FLUSH_ENTRY,0x00,0x00); 
  }

/***********************************************************************/
//函数名        : Set_MAC_forbid(uint16_t port_vector)
//功能          ：设置端口MAC学习禁止模式
//参数          ：port_vector : 端口向量(0~9)
//返回值        ：None
/***********************************************************************/	
void Set_MAC_forbid(uint16_t port_vector)
  {
	Set_Manual_learning_mode(port_vector);
  }

/***********************************************************************/
//函数名        : Get_MAC_forbid()
//功能          ：获取端口MAC学习禁止模式的端口向量
//参数          ：None
//返回值        ：禁止模式的端口向量
/***********************************************************************/	
uint16_t Get_MAC_forbid()
  {
	return Get_Manual_learning_mode();
  }	
/***********************************************************************/	
//函数名      	:	Display_sta_MAC_entry(MAC_API_T *entry,MAC_ENTRY_T *mac_entry)
//功能          : 获取静态 ATU(:MAC entry table)项
//参数          : MAC_API_T  : 存放获得的MAC entry相关信息
//                MAC_ENTRY_T: 存放当前存放的MAC值，初次调用需对 MAC_ENTRY_T.mac_entry_MAC填入初值{0xff,0xff,0xff,0xff,0xff,0xff}
//返回值        : '0' 表示到达了ATU(:MAC entry table)结尾 ,‘1’表示还可能有未读取到的静态MAC entry 
/***********************************************************************/	
uint8_t Display_sta_MAC_entry(MAC_API_T *entry,MAC_ENTRY_T *mac_entry,uint16_t vlan_id)
{
	uint16_t loop;
	
	while(1)  //Repeat till a static entry found
	 { 
		mac_entry->mac_entry_fid = vlan_id;
		Read_ATU_entry(mac_entry);//Read a MAC entry out 
		if(  !(Str_cmp(mac_entry->mac_entry_MAC,Targ_MAC,6))  ) //Judge if the read out MAC entry IS all '0xff's,
	     { return 0;}
		if((mac_entry->mac_entry_data & GET_LOW_4BITS) > 0x07)//The low 4 bits  refers to entry_State and (0x07~ 0x0f)indicates a static entry found
		  {
       /*Assign the readout entry to API_ENTRY*/ 
#ifdef MY_DEBUG //The debug mode select
		printf("The read out static MAC entry: ");
		for(loop =0 ;loop<6 ;loop++ )
		  {
			entry->new_mac[loop] = mac_entry->mac_entry_MAC[loop];
			printf("%x.",entry->new_mac[loop]);
		  }
		printf("\n\r");
#else   //
		for(loop = 0 ; loop <6 ; loop++)
		  {
			entry->new_mac[loop] = mac_entry->mac_entry_MAC[loop];
          }
#endif			 
      /*Fill in the sta_ordym area for struct API_ENTRY*/
		if((mac_entry->mac_entry_data & 0x0f) <= 0x07)
		  {
			entry->sta_or_dym = ENTRY_DYNAMIC;
          }
		else
		  {
			entry->sta_or_dym = ENTRY_STATIC;					
		  }
/*Fill in the port_vector for struct API_ENTRY*/
		entry->port_vector = ((mac_entry->mac_entry_data) >>4);
		entry->vlan_id     = mac_entry->mac_entry_fid ;
		return 1;
		}//end of 'if((mac_entry->mac_entry_data & GET_LOW_4BITS) > 0x07)'
	}//end of while(1)
}
/***********************************************************************/
//函数名   : MAC_demo()
//功能     ：测试所有编写的MAC表项接口函数
//参数     ：None
//返回值   ：None
/***********************************************************************/
void MAC_demo()
  {
	uint16_t test_tem;
	uint16_t loop;
	uint16_t circle;
	MAC_ENTRY_T entryx;
	MAC_API_T  api_entry;
	API_VLAN_MEM_T  vlan_entry;
	uint16_t   entry_counter;
	uint8_t    test_MAC[6]   = {0x01,0x00,0x00,0x00,0x00,0x00};
	uint8_t    test_MAC_2[6] = {0x00,0x20,0x30,0x1f,0xde,0x00};
	uint8_t    test_MAC_3[6] = {0x00,0xde,0x00,0x00,0x00,0x00};
	uint8_t    test_MAC_4[6] = {0xde,0x44,0x5e,0x00,0x00,0x00};
	uint8_t    test_MAC_5[6] = {0xdd,0xff,0x59,0xfd,0x00,0x00};
	uint16_t   vlan[2] = {1,2};
/**/
//	for(loop = 0 ;loop <10 ; loop++)
//	  {
//	    vlan_entry.mem[loop] = 0x00;
//	  }
//	for(loop = 0 ;loop <4 ; loop++)
//	  {
//	    vlan_entry.mem[loop] = 0x01;
//	  }
	vlan_entry.vlan_id       = 1;
	setVlanMember(&vlan_entry);
	
	for (loop= 0;loop <4 ;loop++ )
	  {
		setPortVlanId(0x0001,loop);
		setSecurityLevel(SECURITY,loop);
	  }
/**/
//	for(loop = 0 ;loop <10 ; loop++)
//	  {
//	    vlan_entry.mem[loop] = 0x00;
//	  }
//	for(loop = 5 ;loop <9 ; loop++)
//	  {
//	    vlan_entry.mem[loop] = 0x01;
//	  }
	vlan_entry.vlan_id       = 2;
	setVlanMember(&vlan_entry);
	
	for (loop= 5;loop <9 ;loop++ )
	  {
		setPortVlanId(0x0002,loop);
		setSecurityLevel(SECURITY,loop);
	  }
	  
	Flush_MAC_entry();  
//	for(loop = 0 ;loop <4 ; loop++)
//	  {
//	    vlan_entry.mem[loop] = 0x01;
//	  }
//	vlan_entry.vlan_id       = 2;
//	setVlanMember(&vlan_entry);
	
//	for (loop= 0;loop <4 ;loop++ )
//	  {
//		setPortVlanId(0x0001,loop);
//		setSecurityLevel(SECURITY,loop);
//	  }
/*测试老化时间函数*/
//	test_tem = Get_Age_time();     //Get the default aging time
//	printf("Default aging time : %d\n\r", test_tem);
//	Set_Age_time(15);              //Set a 15s aging time
//	test_tem = Get_Age_time();     //Confirm the aging time set
//	printf("Selfdef aging time : %d\n\r", test_tem);
//	Set_Age_time(31);
//	test_tem = Get_Age_time();     //Confirm the aging time set
//	printf("A none 15n s aging time : %d\n\r", test_tem);
//	Set_Age_time(0);               //Set the aging forbidden
/*测试显示MAC entry函数*/ //此测试前先拔掉网线
	entry_counter = 0;
	for(circle = 0 ;circle<2 ;circle++ )
	{
	  printf("The fid: %d\n\r",vlan[circle]);
	  Str_cpy(entryx.mac_entry_MAC,Targ_MAC,6);
	  while(Display_ALL_MACentry(&api_entry,&entryx,vlan[circle])) //Get All the MAC entry
		{
		entry_counter++;
		printf("The sta_or_dym targ is : %d\n\r",api_entry.sta_or_dym);		
		}
	}
	printf("The Read out MAC counter is:");
	printf("%d\n\r",entry_counter);	
/*测试添加一条MAC entry*/ //测试添加一条静态MAC entry 端口向量port5(0x01<<5),实例MAC地址：00-01-02-03-04-05
	for(loop =0 ;loop<6 ;loop++ )
	  {
		api_entry.new_mac[loop] = test_MAC[loop];
      }
	//strncpy(api_entry.new_mac,test_MAC,6);
	api_entry.add_or_delet = ADD_MAC;
    api_entry.port_vector  = 0x01 << 5;
	api_entry.vlan_id      = 1;
    if(Add_or_Delet_MACentry(&api_entry))
	  {
		printf("The add entry operation is successful!!\n\r");
	  }
/*连续添加几条MAC entry*/
	for(loop =0 ;loop<6 ;loop++ )
	 {
		api_entry.new_mac[loop] = test_MAC_2[loop];
	 }
	//strncpy(api_entry.new_mac,test_MAC,6);
	api_entry.add_or_delet = ADD_MAC;
    api_entry.port_vector  = 0x01 << 5;
	api_entry.vlan_id      = 1;
    if(Add_or_Delet_MACentry(&api_entry))
	  {
		printf("The add entry _2 operation is successful!!\n\r");
	  }
	for(loop =0 ;loop<6 ;loop++ )
	  {
		api_entry.new_mac[loop] = test_MAC_3[loop];
      }
	//strncpy(api_entry.new_mac,test_MAC,6);
	api_entry.add_or_delet = ADD_MAC;
    api_entry.port_vector  = 0x01 << 5;
	api_entry.vlan_id      = 2;
    if(Add_or_Delet_MACentry(&api_entry))
	{
	  printf("The add entry _3 operation is successful!!\n\r");
	}
//
	for(loop =0 ;loop<6 ;loop++ )
	  {
		api_entry.new_mac[loop] = test_MAC_4[loop];
	  }		
	api_entry.add_or_delet = ADD_MAC;
    api_entry.port_vector  = 0x01 << 5;
	api_entry.vlan_id      = 2;
    if(Add_or_Delet_MACentry(&api_entry))
	  {
		printf("The add entry _4 operation is successful!!\n\r");
      }
	for(loop =0 ;loop<6 ;loop++ )
	  {
		api_entry.new_mac[loop] = test_MAC_5[loop];
      }		
	api_entry.add_or_delet = ADD_MAC;
    api_entry.port_vector  = 0x01 << 5;
	api_entry.vlan_id      = 2;
    if(Add_or_Delet_MACentry(&api_entry))
	  {
		printf("The add entry _5 operation is successful!!\n\r");
      }
	
/*读取所有静态MAC entry */
	entry_counter = 0;
	for(circle = 0;circle <2 ;circle++ )
	{
	  printf("The fid: %d\n\r",vlan[circle]);
	  Str_cpy(entryx.mac_entry_MAC,Targ_MAC,6);
	  while(Display_sta_MAC_entry(&api_entry,&entryx,vlan[circle]))
	  {
		entry_counter++;
	  }
	  printf("The Read out static MAC counter is:");
	  printf("%d\n\r",entry_counter);
	}
/*再次读取所有MAC entry*/
	entry_counter = 0;
	for(circle = 0;circle <2 ;circle++ )
	{
	  for(loop = 0 ;loop < 6; loop++ )
		{
		  entryx.mac_entry_MAC[loop] = 0xff;          //MAC{0xff,0xff,0xff,0xff,0xff,0xff} used to get the first ATU entry
		}
	  printf("The fid: %d\n\r",vlan[circle]);
	  while(Display_ALL_MACentry(&api_entry,&entryx,vlan[circle])) //Get All the MAC entry
		{
		  entry_counter++;
		}
	}
	printf("The Read out MAC counter is:");
	printf("%d\n\r",entry_counter);
/*删掉刚才添加的MAC entry*/
//		strncpy((char *)api_entry.new_mac,test_MAC,6);
	for(loop =0 ;loop<6 ;loop++ )
	  {
		api_entry.new_mac[loop] = test_MAC[loop];
      }
	api_entry.add_or_delet = DEL_MAC;
	api_entry.port_vector  = 0x01<<5;
	if(Add_or_Delet_MACentry(&api_entry))
	  {
		printf("The delete operation successed!!\n\r");
	  }
/*读取所有MAC entry*/
	entry_counter = 0;
	for(circle = 0;circle <2 ;circle++ )
	{
	  printf("The fid: %d\n\r",vlan[circle]);
	  for(loop = 0 ;loop < 6; loop++ )
		{
		  entryx.mac_entry_MAC[loop] = 0xff;          //MAC{0xff,0xff,0xff,0xff,0xff,0xff} used to get the first ATU entry
		}
	  while(Display_ALL_MACentry(&api_entry,&entryx,vlan[circle])) //Get All the MAC entry
		{
		  entry_counter++;
		}
	}
	printf("The Read out MAC counter is:");
	printf("%d\n\r",entry_counter);
/*设置端口1为禁止学习模式*/
//	Set_Manual_learning_mode(0x0123);
//	test_tem = Get_MAC_forbid();
//	printf("The Read out forbid  port_vector is:");
//	printf("%x\n\r",test_tem);
/*再次读出所有MAC entry*/
	entry_counter = 0;
	for(circle = 0;circle <2 ;circle++ )
	{
	  printf("The fid: %d\n\r",vlan[circle]);
	  for(loop = 0 ;loop < 6; loop++ )
		{
		  entryx.mac_entry_MAC[loop] = 0xff;          //MAC{0xff,0xff,0xff,0xff,0xff,0xff} used to get the first ATU entry
		}
	  while(Display_ALL_MACentry(&api_entry,&entryx,vlan[circle])) //Get All the MAC entry
		{
		  entry_counter++;
		}
	}
	printf("The Read out MAC counter is:");
	printf("%d\n\r",entry_counter);	
/*清除所有非静态MAC entry*/ 
	Flush_MAC_entry();
//	Delays(5);
/*再次读取所有MAC_entry*/
	entry_counter = 0;
	for(circle = 0;circle <2 ;circle++ )
	{
	  printf("The fid: %d\n\r",vlan[circle]);
	  for(loop = 0 ;loop < 6; loop++ )
		{
		  entryx.mac_entry_MAC[loop] = 0xff;          //MAC{0xff,0xff,0xff,0xff,0xff,0xff} used to get the first ATU entry
		}
	  while(Display_ALL_MACentry(&api_entry,&entryx,vlan[circle])) //Get All the MAC entry
		{
		  entry_counter++;
		}
	}
	printf("The Read out MAC counter is:");
	printf("%d\n\r",entry_counter);	

/*Wait some time and read again*/
	entry_counter = 0;
	for(circle = 0;circle <2 ;circle++ )
	{
	  printf("The fid: %d\n\r",/*vlan*/0);
	  for(loop = 0 ;loop < 6; loop++ )
		{
		  entryx.mac_entry_MAC[loop] = 0xff;          //MAC{0xff,0xff,0xff,0xff,0xff,0xff} used to get the first ATU entry
		}
	  while(Display_ALL_MACentry(&api_entry,&entryx,vlan[circle])) //Get All the MAC entry
		{
		  entry_counter++;
		}
	}
	printf("The Read out MAC counter is:");
	printf("%d\n\r",entry_counter);	
  }

/*****************************************************************************/
//函数名 ： static macDecreaseByOne(uint8_t * pMAC)
//功能   ： MAC地址值减1
//参数   ： pMAC指向一个MAC数组
//返回值 ： None
/*****************************************************************************/
	
static void macDecreaseByOne(uint8_t *pMAC)
  { 
	uint8_t loop;
		
	loop= 6;
	while(loop--)
	{
	  if(*(pMAC+loop) == 0 )
		{
		  *(pMAC + loop)= 0xff;
		}
	  else
		{
		  (*(pMAC + loop))--;
		  break;
		}
	}
       
  }
	


