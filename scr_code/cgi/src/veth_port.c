/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\hal\src_code\cgi\src\veth_port.c
@Author  : Duke Lee
@Date    : 21-Oct-2014
@Version : V1.0.00
@Breif   : CGI接口：
****************************************************************************************
**/

#include <stdio.h>
#include <stdint.h>

#include "veth_port.h"
#include "veth_common.h"
#include "veth_eth_ap.h"
uint8_t g_stats_mode = 0x03;


/***********************************************************************/
//函数名  ： printStatus(int portNo)
//功能    ： 打印某些PHY寄存器
//参数    ： 端口号
//返回值  ： None
/***********************************************************************/

void printStatus(int portNo)
{ 
	 uint16_t  tem_reg ;
	  
	 READ_PHYREGISTER(SWITCH_PHY_0+portNo,REG_PHY_CONTROL,&tem_reg);
	 printf("REG_PHY_CONTROL value : %x \n\r",tem_reg);
	 READ_PHYREGISTER(SWITCH_PHY_0+portNo,REG_PHY_SPECIFIC_CONTROL1,&tem_reg);
	 printf("REG_PHY_SPECIFIC_CONTROL1 value : %x \n\r",tem_reg);
	 READ_PHYREGISTER(SWITCH_PHY_0+portNo,REG_PHY_STATUS,&tem_reg);
	 printf("REG_PHY_STATUS value : %x \n\r",tem_reg);
	 READ_PHYREGISTER(SWITCH_PHY_0+portNo,REG_PHY_SPECIFIC_STATUS,&tem_reg);
	 printf("REG_PHY_SPECIFIC_STATUS value : %x\n\r",tem_reg);
	 READ_PHYREGISTER(SWITCH_PHY_0+portNo,REG_AUTO_NEG_ADVERTISE,&tem_reg);	
	 printf("REG_AUTO_NEG_ADVERTISE value : %x\n\r",tem_reg);
	 READ_PHYREGISTER(SWITCH_PHY_0+portNo,REG_LINK_PARTNER_ABILITY,&tem_reg);
	 printf("REG_LINK_PARTNER_ABILITY value : %x\n\r",tem_reg);
	 READ_PHYREGISTER(SWITCH_PHY_0+portNo, REG_AUTO_NEG_EXPANSION,&tem_reg);
	 printf("REG_AUTO_NEG_EXPANSION value : %x\n\r",tem_reg);
	 READ_PHYREGISTER(SWITCH_PHY_0+portNo,REG_NEXT_PAGE_TRANSMIT,&tem_reg);
	 printf("REG_NEXT_PAGE_TRANSMIT value : %x\n\r",tem_reg);
	 READ_PHYREGISTER(SWITCH_PHY_0+portNo,REG_LINK_PARTNER_NEXT_PAGE,&tem_reg);
	 printf("REG_LINK_PARTNER_NEXT_PAGE value : %x\n\r",tem_reg);
	 READ_PHYREGISTER(SWITCH_PHY_0+portNo,REG_RECIEVE_ERROR_COUNTER,&tem_reg);
	 printf("REG_RECIEVE_ERROR_COUNTER value : %x\n\r",tem_reg);
	 READ_PHYREGISTER(SWITCH_PHY_0+portNo,REG_PHY_SPECIFIC_CONTROL2,&tem_reg);
	 printf("REG_PHY_SPECIFIC_CONTROL2 value : %x\n\r",tem_reg);
 
}
/***********************************************************************/
//函数名  ： printPortStatus(int portNo)
//功能    ： 打印端口状态信息，双工模式： ‘1’全双工 ‘0’半双工 速率：'10'_10M '100'_100M '1000'_1G 连接状态：‘1’ on ‘0’ down
//参数    ： 端口号
//返回值  ： None
/***********************************************************************/
void printPortStatus(int portNo)
{
	int tem_result ;
	
	tem_result = Get_link_status(portNo);
	printf("After Set,the link status is : %d\n\r",tem_result);
	tem_result = Get_speed_status(portNo);
	printf("After Set,the speed status is : %d\n\r",tem_result);
	tem_result = Get_duplex_status(portNo);	
	printf("After Set,the duplex status is : %d\n\r",tem_result);
}

/***********************************************************************/
//函数名  ： printRoughCounter(API_ROUGH_COUNTER_T *p_rough)
//功能    ： 打印流量统计
//参数    ： p_rough，指向待打印的API_ROUGH_COUNTER_T结构体
//返回值  ： None
/***********************************************************************/
static void printRoughCounter(API_ROUGH_COUNTER_T *p_rough)
{
	 
	printf("The RxGoodOctets counter value : %lld\n\r",p_rough->RxGoodOctets);
	printf("The TxGoodOctets counter value : %lld\n\r",p_rough->TxGoodOctets);
	printf("The RxbadOctets counter value : %d\n\r",p_rough->RxbadOctets);
	printf("The RxDiscard counter value : %d\n\r",p_rough->RxDiscard);	
	printf("The RxFiltered counter value : %d\n\r",p_rough->RxFiltered);	
	printf("The TxFiltered counter value : %d\n\r",p_rough->TxFiltered);		
}

/***********************************************************************/
//函数名  ： printDetailCounter(API_DETAIL_COUNTER_T *p_detail)
//功能    ： 打印流量统计
//参数    ： p_detail，指向待打印的API_DETAIL_COUNTER_T结构体
//返回值  ： None
/***********************************************************************/	
static void printDetailCounter(API_DETAIL_COUNTER_T *p_detail)
{
	printf("The RxGoodOctets counter value : %lld \n\r",p_detail->RxGoodOctets);
	printf("The RxbadOctets  counter value : %d \n\r",p_detail->RxbadOctets);
	printf("The RxUnicast counter value : %d \n\r",p_detail->RxUnicast);
	printf("The RxBroadcasts counter value : %d \n\r",p_detail->RxBroadcasts);
	printf("The RxMulticasts  counter value : %d \n\r",p_detail->RxMulticasts);
	printf("The RxPause  counter value : %d \n\r",p_detail->RxPause);
	printf("The RxFrames  counter value : %lld \n\r",p_detail->RxFrames);
	printf("The RxUndersize  counter value : %d \n\r",p_detail->RxUndersize);
	printf("The RxFragments  counter value : %d \n\r",p_detail->RxFragments);
	printf("The RxOversize  counter value : %d \n\r",p_detail->RxOversize);
	printf("The RxJabber  counter value : %d \n\r",p_detail->RxJabber);
	printf("The RxErr  counter value : %d \n\r",p_detail->RxErr);
	printf("The RxFCSErr  counter value : %d \n\r",p_detail->RxFCSErr);
	printf("The RxDiscard  counter value : %d \n\r",p_detail->RxDiscard);
	printf("The RxFiltered  counter value : %d \n\r",p_detail->RxFiltered);
	
	printf("The TxGoodOctets  counter value : %lld \n\r",p_detail->TxGoodOctets);
	printf("The TxUnicast  counter value : %d \n\r",p_detail->TxUnicast);
	printf("The TxBroadcasts  counter value : %d \n\r",p_detail->TxBroadcasts);
	printf("The TxMulticasts  counter value : %d \n\r",p_detail->TxMulticasts);
	printf("The TxPause  counter value : %d \n\r",p_detail->TxPause);
	printf("The TxFrames  counter value : %lld \n\r",p_detail->TxFrames);
	printf("The TxDeferred  counter value : %d \n\r",p_detail->TxDeferred);
	printf("The TxCollisions  counter value : %d \n\r",p_detail->TxCollisions);
	printf("The TxSingle  counter value : %d \n\r",p_detail->TxSingle);
	printf("The TxMultiple  counter value : %d \n\r",p_detail->TxMultiple);
	printf("The TxExcessive  counter value : %d \n\r",p_detail->TxExcessive);
	printf("The TxLate  counter value : %d \n\r",p_detail->TxLate);
	printf("The TxFCSErr  counter value : %d \n\r",p_detail->TxFCSErr);
	printf("The TxFiltered  counter value : %d \n\r",p_detail->TxFiltered);
}

/***********************************************************************/
//函数名  ： Port_demo();
//功能    ： cgi接口函数：port项程序测试样例
//参数    ： None
//返回值  ： None
/***********************************************************************/
void Port_demo(void)
{
	int tem_result;
//	uint16_t tem_reg;
	uint8_t tem_mode;
	uint8_t loop;
	API_ROUGH_COUNTER_T  rough_counter;
	API_DETAIL_COUNTER_T detail_counter;
	VETH_TYPE_T  the_type;

/*设置端口1 10M半双工*/
	Set_port_status(1,SPEED10_HALFDPX);
	printStatus(1);
	printPortStatus(1);
/*设置端口1为100M全双工*/
	Set_port_status(1,SPEED100_FULLDPX);
	printStatus(1);
	printPortStatus(1);
/*设置端口1为DISABLED*/
	Set_port_status(1,DISABLED);  
/*设置端口1为AUTO*/
	Set_port_status(1,AUTO);
	printStatus(1);
	printPortStatus(1);
/*设置端口为100M半双工*/  
	Set_port_status(1,SPEED100_HALFDPX);
	printStatus(1);
	printPortStatus(1);	
/*设置端口为10M全双工*/  
	Set_port_status(1,SPEED10_FULLDPX);
	printStatus(1);
	printPortStatus(1);
/*设置端口1为AUTO*/  
	Set_port_status(1,AUTO);
	printStatus(1);
	printPortStatus(1);
/*设置端口8——DISABLED*/
	Set_port_status(9,DISABLED); 
/*设置端口8--1000M_FDX*/
	Set_port_status(9,SPEED1000_FULLDPX);
/*获取端口类型*/
	for(loop =0 ;loop < 8;loop++)
	{
	  tem_result = getPortType(loop);
	  printf("The result porttype: %d\n\r",tem_result);
	}
/*测试获取设备型号的*/		
	getSetType(&the_type);
	printf("\n\r");
	printf("The set infor:\n\r");
	printf("%s\n\r",the_type.set_name);
	printf("%s\n\r",the_type.set_type);
	printf("%s\n\r",the_type.set_version);
	printf("Max_portNo : %d\n\r",the_type.max_portNo);
	loop =1;
	while(loop--){
/*设置端口1的流控开启*/
     Set_port_status(1,SPEED100_FULLDPX);
	 Set_flow_control(1,1);
	 printStatus(1);
//	 Set_port_status(1,AUTO);
//	 Set_flow_control(1,1);  //原型Set_flow_control(int portNo, int flow_con)
//	 printStatus(1);
	 Set_flow_control(9,1);
/*查看流控是否已经开启*/
	 tem_result = Get_flow_status(1);
///*设置端口1的流控开启*/
//  Set_flow_control(1,0);  //原型Set_flow_control(int portNo, int flow_con)
//	printStatus(1);
/*查看流控是否已经关闭*/
	tem_result = Get_flow_status(8);
//	Set_port_status(1,SPEED100_FULLDPX);
	}//end of while(loop--)	
/*查看最大包长*/
	tem_result = Get_max_package();
	printf("The maximum package surpported is :%d\n\r",tem_result);

/*显示某个端口(例如端口1)的流量统计*/
	Display_Rough_Statistics(1,&rough_counter);
	printRoughCounter(&rough_counter);
/*显示某个端口(例如端口1)的详细统计*/
	Display_Detail_Statistics(1,&detail_counter);
	printDetailCounter(&detail_counter);

/*清除单个端口的计数器*/  //也可以拔掉该口的网线，查看清除后的结果是0
	Flush_Port_counters(1);
/*清除后再次显示该口计数器的值以及其他口的计数器值*/ 
	printf("\n\r\n\r After flush port: \n\r\n\r");
	Display_Detail_Statistics(1,&detail_counter);
	printDetailCounter(&detail_counter);
	printf("\n\r\n\r And other port:\n\r\n\r");
	Display_Detail_Statistics(2,&detail_counter);
	printDetailCounter(&detail_counter);

/*重新插上网线一段时间后拔掉，然后清掉所有的计数器再读取计数器的值*/
	Flush_ALL_counters();
	printf("\n\r\n\r After flush ALL: \n\r\n\r");
	Display_Detail_Statistics(1,&detail_counter);
	printDetailCounter(&detail_counter);
	printf("\n\r\n\r And other port:\n\r\n\r");
	Display_Detail_Statistics(2,&detail_counter);
	printDetailCounter(&detail_counter);
/*获取64octets一类计数器的模式*/
	tem_mode = getStatsMode();
	if(tem_mode == 1)
		printf("The 64octets counters Rx\n\r");
	if(tem_mode == 2)
		  printf("The 64octets counters Tx\n\r");
	if(tem_mode == 3)
		  printf("The 64octets counters Tx&Rx\n\r");
/*插上网线一会拔掉,更改计数模式*/
	changeStatsMode(1);
/*获取64octets一类计数器的模式*/
	tem_mode = getStatsMode();
	if(tem_mode == 1)
		printf("The 64octets counters Rx\n\r");
	if(tem_mode == 2)
		  printf("The 64octets counters Tx\n\r");
	if(tem_mode == 3)
		  printf("The 64octets counters Tx&Rx\n\r");
/*粗略显示计数器的值*/
	Display_Rough_Statistics(1,&rough_counter);
	printRoughCounter(&rough_counter);
}
/***********************************************************************/
//函数名  :  Get_link_status(int portNo) 
//功能    ： 获取端口的link状态
//参数    ： 端口号portNo
//返回值  ： ’1‘ link on  ’0‘link down
//
/***********************************************************************/
int Get_link_status(int portNo)
{
	return Get_Port_status(portNo);
}

/***********************************************************************/
//函数名  : Get_speed_status(int portNo)
//功能    ：获取端口的速率
//参数    ：端口号 portNo
//返回值  ：’10‘ 10M ，’100‘100M ，‘1000’ 1G
//
/***********************************************************************/
int Get_speed_status(int portNo)
{
	return Get_Oper_speed(portNo);
}

/***********************************************************************/
//函数名  : Get_duplex_status(int portNo)
//功能    ：获取双工模式
//参数    ：端口号portNo
//返回值  ：’1‘表示全双工，’0‘表示半双工
//
/***********************************************************************/
int Get_duplex_status(int portNo)
{
	return Get_Port_duplex(portNo);
}

/***********************************************************************/
//函数名  : Set_port_status(int portNo, PORT_STAS sta)
//功能    ：设置端口的速率
//参数    ：端口号：portNo ，需要设置的状态 PORT_STAS sta 端口状态枚举，具体看枚举定义
//返回值  ：None
//
/***********************************************************************/
void Set_port_status(int portNo, PORT_STAS sta)
{
	Set_PORT_STATS(portNo,sta);
}

/***********************************************************************/
//函数名  : Set_flow_control(int portNo, int flow_con)
//功能    ：设置端口的流控
//参数    ：端口号portNo，flow_con ：’1‘表示设置流控，’0‘表示取消流控设置
//返回值  ：None
//
/***********************************************************************/
void Set_flow_control(int portNo, int flow_con)
{
	Set_FLOW_CONTROL(portNo ,flow_con);
}

/***********************************************************************/
//函数名  : Get_flow_status(int portNo)
//功能    ：获取流控开启状态
//参数    ：端口号portNo
//返回值  ：'1'表示流控开启，’0‘表示流控关闭
//
/***********************************************************************/
int Get_flow_status(int portNo)
{
	return Get_FLOW_STATUS(portNo);
}

/***********************************************************************/
//函数名  : getPortType(int portNo)
//功能    ：获取端口类型
//参数    ：端口号portNo
//返回值  ：'1'表示光口，’0‘表示电口
//
/***********************************************************************/
int getPortType(int portNo)
{
	return Get_Port_Type(portNo);
}
/***********************************************************************/
//函数名  :  Get_max_package()
//功能    ： 获取支持的最大包长
//参数    ： None
//返回值  ： 最大包长
//
/***********************************************************************/
int Get_max_package()
{
	return PACKAGE_MAX_LENTH;
}

/***********************************************************************/
//函数名  : Display_Rough_Statistics(int portNo，API_ROUGH_COUNTER_T *prough_cnt)
//功能    ：显示端口的流量统计
//参数    ：portNo，填入端口号进行统计
//          prough_cnt 传入的API_ROUGH_COUNTER_T指针，用来收集统计结果
//返回值  ：None
//
/***********************************************************************/
void Display_Rough_Statistics(int portNo,API_ROUGH_COUNTER_T *p_rough)
{
	Get_Port_Counters_rou(p_rough,portNo);
}

/***********************************************************************/
//函数名  : Display_Detail_Statistics(int portNo，API_DETAIL_COUNTER_T *pdetail)
//功能    ：显示端口的流量详细统计
//参数    ：portNo，填入端口号进行统计
//          pdetail 传入的API_DETAIL_COUNTER_T指针，用来收集统计结果
//返回值  ：None
//
/***********************************************************************/
void Display_Detail_Statistics(int portNo,API_DETAIL_COUNTER_T *p_detail)
{
	Get_Port_Counters_det(p_detail,portNo); 
}

/***********************************************************************/
//函数名  : Flush_ALL_counters()
//功能    ：清楚所有的数据统计
//参数    ：None
//返回值  ：None
//
/***********************************************************************/ 
void Flush_ALL_counters()
{
	Change_COUNTER_mode(g_stats_mode);
	Clear_ALL_COUNTER(CLEAR_ALL);
}	

/***********************************************************************/
//函数名  : Flush_Port_counters(int portNo)
//功能    ：清除指定端口的计数器
//参数    ：端口号portNo
//返回值  ：None
//
/***********************************************************************/
void Flush_Port_counters(int portNo)
{
	Clear_ALL_COUNTER((uint8_t)portNo);
}

/***********************************************************************/
//函数名   ：Change_stats_mode()
//功能     ：切换关于端口流量详细统计的具体字节统计模式，64octets等等
//参数     ：histogram ,uint8_t 类型，取值范围在1到3之间 
//           模式'1'表示64octets一类的计数器只计Rx ，模式’2‘表示64octets一类计数器只计Tx，
//           模式’3‘表示64octets一类计数器计算Rx&Tx
//特别注意 ：每次切换详细字节64octets列计数模式时，内置所有计数器清0函数
/***********************************************************************/
void changeStatsMode(uint8_t histogram)
{
/*更改全局计数模式标志位*/    
	g_stats_mode = histogram;
/*将更新的计数模式添加进去*/	  
	Change_COUNTER_mode(histogram);
/*每次切换模式将触发清除所有计数器的操作*/
	Clear_ALL_COUNTER(CLEAR_ALL);
}

/***********************************************************************/
//函数名   ：Get_stats_mode()
//功能     ：获取关于端口流量详细统计的具体字节统计模式，64octets等等
//参数     ：None
//返回值   ：模式'1'表示64octets一类的计数器只计Rx ，模式’2‘表示64octets一类计数器只计Tx，
//             模式’3‘表示64octets一类计数器计算Rx&Tx
/***********************************************************************/
uint8_t getStatsMode()
{
	g_stats_mode = getCounterMode();
	return g_stats_mode;
}
