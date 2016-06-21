/**
**************************************************************************
@File    :  D:\ProgramFiles\KEIL\hal\incl\veth_common.h
@Author  :  Duke Lee
@Verdion :  V1.0
@Date    :  13-June-2014
@brief   :  constant in common
**************************************************************************
**/


#ifndef __COMMON_H_
#define __COMMON_H_


#include <stdint.h>
#include "stm32f4xx_hal_conf.h"
#include "veth_InVariate.h"
#include "veth_netupgrade.h"

/*Define the set number*/
#define VETH2000 
#define PORTSNUM  10
/*If the sets runs RSTP ,define this,else define VETH2000_WEB.*/ 
//#define  VETH2000_RSTP   
#define  VETH2000_WEB
/*define a time out for the uip_Rx_packet*/
#define UIP_RX_TIMEOUT 0x01 
#define CLI_TIMEOUT    0x01
/*Define a button for command line interface*/
#define CLI_BUTTON  1     //If you wanna cancel the CLI parts, just define CLI_BUTTON for '0'.

//define the size of the descriptor and the buffer for ether DMA
#define DESCRIPTORLIST_SIZE  0x00000050
#define MAX_TX_BUFSIZE       0x00000640
#define MAX_RX_BUFSIZE       0x00000640

/*Define how many ports the sets has*/
#define MAX_PORT_NUM         11

//if not DEBUG, annotate 'MY_DEBUG'
//#define MY_DEBUG
#define MY_DEBUG_P
#define MY_PRINT_RSTP

/*Choose the port attached to CPU*/
#define CPU_PORT_10
//#define CPU_PORT_9
//#define CPU_PORT_8

/*Choose the UARTx for the printf function*/
/*Only one of them can be defined*/
#define PRF_USART1
//#define PRF_USART6
//#define PRF_UART5
//#define PRF_UART7
//#define PRF_UART4

/*DSA(Destributed Switch Archetecture) added*/
#define DSA_ADDED              //For Marvell 88E6097F only 
#define THE_TAG_ADDR   0x10    //for The target of 88E6097F
#define THE_TAG_LENGTH 0x04    //

#define COM_DATA_SIZE      3072
#define COM_RCV_DATA_SIZE  3072
#define COM_XFER_DATA_SIZE 5000
#define COM_DATA_PACK_NUM  1
#define COM_GET_PACK_NUM   3 
#define INIT_RCV_LENGTH    80

/*Define The MAC address for my sets*/
#define THE_BIT_40          0x01  //Calculate along with the MAC i
#define THE_PORT_MAC_RESET  0xF0  //Use this const to clear the PORT MAC bits[3:0]

/*Define the I2C slave address for the device*/
#define I2C_ADDRESS 0x0A
/*Define which I2C bus to use*/
#define I2Cx  I2C1
/*Define the maximum size of the i2c TX buffer and the RX buffer*/
#define I2C_MAX_BUFSIZE  0x100

#define VLAN_MEM    0x3000
#define VLAN_UNMOD  0x0000
#define VLAN_UNTAG  0x1000
#define VLAN_TAGED  0x2000
/*定义标注设备型号的结构体*/
typedef struct veth_type_t {
	uint8_t set_name[20];     /*设备名*/
	uint8_t set_type[20];     /*设备型号*/
	uint8_t set_version[10];  /*设备版本*/
	uint8_t max_portNo;       /*设备最大端口数*/
}VETH_TYPE_T;


/*Define the enum for the  MAC operation*/
typedef enum atu_t         //ATU(Address Translation Unit) also means MAC table
{                
	READ_ENTRY,
	LOAD_PURGE_ENTRY,
	FLUSH_ENTRY,
	MOVE_ENTRY
}MAC_OPER;

/*Define the enum to choose the way fulsh/move operations will go*/
typedef enum atu_flushmove_t {
	ALL,
	ALL_NONSTA,
	ALL_FID,
	ALL_NONSTA_FID
}ATU_OPER_MODE;

typedef enum mac_entry_oper {
	 NO_OPERATION = 0x0,
	 FLUSH_MOVE_ENTRY_ALL,
	 FLUSH_MOVE_ENTRY_NONSTA,
	 LOAD_PURGE_ENTRY_FID,
	 GET_NEXT_FID,
	 FLUSH_MOVE_ENTRY_FID,
	 FLUSH_MOVE_ALL_NONSTA_FID,
	 GET_CLEAR_VIOLATION
}ATU_OPER_T;

/*Define the structure of the entry in the MAC table*/
typedef struct entry_t {          //The format of the entry (ATU,too)
	  ATU_OPER_T mac_entry_oper;  
	  uint16_t mac_entry_data;    //bit[15] trunk bit,bits[14:4] are the port_vector, bits[3:0] entry state : 0x7 for the static entry state 
	  uint8_t  mac_entry_MAC[6];
	  uint16_t mac_entry_fid;     //Only lower 12bits availiable
}MAC_ENTRY_T;

/*Define the VTU entry*/
typedef struct entry_v {
	  uint16_t  VTU_sid;
	  uint16_t  VTU_fid;
	  uint16_t  VTU_vid;
	  uint16_t  VTU_data0;
	  uint16_t  VTU_data1;
	  uint16_t  VTU_data2; 
}VLAN_ENTRY_T;


typedef struct com6_tx{
	uint8_t s;
       
}COM6_TX_T;

/*define a enum for set vlan port */
typedef enum tag_t 
  {
	ALL_FRAME = 0,
	TAGGED,
	UNTAGGED
  }TAG_T;

/*define a enum for egress frame type*/
typedef enum egress_type_t
  {
	UNTAG_PVID,
	TAG_ALL,
	UNTAG_ALL
  }EGRESS_TYPE_T;

/*Define a enum for the security level configuration*/
typedef enum secure_level_t {
VLAN_DISABLED,            //默认配置，801.qVLAN关闭
SECURITY,                //Security level,801.qVLAN开启
CHECK,                   //CHECK level
FALLBACK                 //Fallback level
}SECURITY_LEVEL_T;

/*Define the enum for the error events*/
typedef enum error_info{
	 NO_ERROR = 0,
	 GET_RX_BUFFER_ERROR,
	 SET_SPEED_DUPLEX_ERROR,
	 PORT_EXIT_IN_OTHER_TRUNK,
	 THE_TRUNK_MEMBER_INVALID,
	 THE_TRUNK_ISNT_EXIT,
	 THE_TRUNK_PORT_ERROR,
	 THE_TRUNK_ID_NOT_MATCH,
	 COM1_RECIEVE_ERROR,
	 COM6_RECIEVE_ERROR,
	 COM5_RECIEVE_ERROR,
	 COM4_RECIEVE_ERROR,
	 COM7_RECIEVE_ERROR,
	 THE_PORTNO_EXCEED
}MYERROR_INFO;

typedef struct rstp_counter_t {
 uint32_t RECIEVE_AVAILIABLE_BPDU;
 uint32_t RECIEVE_UNAVAILIABLE_BPDU;
 uint32_t RECIEVE_NONE_BPDU;
}RSTP_COUNTER;

#ifdef MY_DEBUG_P
typedef struct heap_t {
 
 uint32_t x[256];
 struct heap_t  *next;
}HEAP_T;
#endif

/*定义cgi接口函数port项的结构体定义*/
typedef struct api_rough_counter {
	uint64_t RxGoodOctets;         //设备收到的包的字节总数
	uint64_t TxGoodOctets;         //设备发送的包的字节总数
	
	uint32_t RxbadOctets;          //收到的bad包的字节总数
	
	uint32_t RxDiscard;            //因缓存不够而丢弃的包的个数
	
	uint16_t RxFiltered;           //设定规则下接收滤去的包的个数
	uint16_t TxFiltered;           //设定规则下发送滤去的包的个数

}API_ROUGH_COUNTER_T;

typedef struct api_detail_counter {
/*接收部分详细统计*/
/*set 1*/
	uint64_t RxGoodOctets;         //收到的包的字节总数
	uint32_t RxbadOctets;          //收到的bad包的字节总数
	
/*set 2*/	
	uint32_t RxUnicast;            //收到的单播包的个数，不包括异常包
	uint32_t RxBroadcasts;         //收到的广播包的个数，不包括异常包
	uint32_t RxMulticasts;         //收到的组播包的个数，不包括异常包
	uint32_t RxPause;              //收到的pause包的个数，不包括异常包
	uint64_t RxFrames;             //收到的包的总个数  ，不包括异常包

/*set 3*/	
	uint32_t RxUndersize;          //收到的低于包下限长度的包，FCS校验通过
	uint32_t RxFragments;          //收到的低于包下限长度的包，FCS校验错
	uint32_t RxOversize;           //收到的高于包上限长度的包，FCS校验通过
	uint32_t RxJabber;             //收到的高于包上限长度的包，FCS校验错
	uint32_t RxErr;                //从PHY收到错误标志的包
	uint32_t RxFCSErr;             //未被计入到RxFragments、RxJabber、RxErr的FCS校验错包
	
/*set 4*/ 
	uint32_t Octets_64;           //  64    字节包的总数，包含错误包
	uint32_t Octets_65to127;      // 64~127 字节包的总数，包含错误包
	uint32_t Octets_128to255;     //128~255 字节包的总数，包含错误包
	uint32_t Octets_256to511;     //256~511 字节包的总数，包含错误包
	uint32_t Octets_512to1023;    //512~1023字节包的总数，包含错误包
	uint32_t Octets_1024toMAX;    //1024~max字节包的总数，包含错误包

/*set 5*/
	uint64_t TxGoodOctets;        //设备发送的包的字节总数

/*发送部分详细统计*/	
/*set 6*/
	uint32_t TxUnicast;           //发送的单播包的个数，不包括异常包
	uint32_t TxBroadcasts;        //发送的广播包的个数，不包括异常包
	uint32_t TxMulticasts;        //发送的组播包的个数，不包括异常包
	uint32_t TxPause;             //发送的pause包的个数，不包括异常包
	uint64_t TxFrames;            //发送包的包总数，不包括异常包

/*set 7*/
	uint32_t TxDeferred;         //发送遇到冲突检测，但最终发送成功的包的个数
	uint32_t TxCollisions;       //未被计入TxSingle、TxMultiple、TxExcessive、TxLate的冲突事件项
	uint32_t TxSingle;           //成功发送的包中遇到一次冲突处理的包
	uint32_t TxMultiple;         //成功发送的包中遇到多次冲突处理的包
	uint32_t TxExcessive;        //遇到连续16次冲突处理并被返送到MAC的包
	uint32_t TxLate;             //冲突检测晚于512bits-times的时间
	uint32_t TxFCSErr;           //发送FCS错的包的个数

/**/	
	uint32_t RxDiscard;            //因缓存不够而丢弃的包的个数
	uint16_t RxFiltered;           //设定规则下接收滤去的包的个数
	uint16_t TxFiltered;           //设定规则下发送滤去的包的个数
}API_DETAIL_COUNTER_T;



extern uint8_t COM6_TX_buffer[MAX_TX_BUFSIZE];                  //The tx_buffer for the USART6 transmit operation
extern uint8_t COM6_RX_buffer[MAX_RX_BUFSIZE];                  //The rx_buffer for the USART6 transmit operation
extern uint8_t COM1_TX_buffer[MAX_TX_BUFSIZE];                  //The tx_buffer for the COM1 transmit operation
extern uint8_t COM1_RX_buffer[MAX_RX_BUFSIZE];                  //The rx_buffer for the COM1 transmit operation
extern uint8_t COM5_TX_buffer[MAX_TX_BUFSIZE];                  //The tx_buffer for the COM5 transmit operation
extern uint8_t COM5_RX_buffer[MAX_RX_BUFSIZE];                  //The rx_buffer for the COM5 transmit operation
extern uint8_t COM4_TX_buffer[MAX_TX_BUFSIZE];
extern uint8_t COM4_RX_buffer[MAX_RX_BUFSIZE];
extern uint8_t COM7_TX_buffer[MAX_TX_BUFSIZE];
extern uint8_t COM7_RX_buffer[MAX_RX_BUFSIZE];
extern uint8_t *read_com6;                                      //The pointer used to read the COM6_RX_buffer
extern uint8_t *read_com1;                                      //The pointer used to read the COM1_RX_buffer
extern uint8_t *read_com5;                                      //Thfile:///C:/Users/Administrator/AppData/Local/Yodao/DeskDict/frame/20140813153820/index.html#e pointer used to read the COM5_RX_buffer
extern uint32_t sCount;                                         //The target for the 1s counter 
extern uint32_t msCount;                                        //The target for the 1ms  counter
extern uint32_t rCount;
extern TIM_HandleTypeDef    TimHandle;                          //The structure for the 1s TIM2
extern TIM_HandleTypeDef    TimsHandle;                         //The structure for the 1ms TIM5
extern TIM_HandleTypeDef    TimrHandle;                         //The structure for the 500ms TIM3
extern UART_HandleTypeDef   Uart6Handle;                        //The structure for the USART6
extern UART_HandleTypeDef   Uart5Handle;                        //The structure for the UART5
extern UART_HandleTypeDef   Uart1Handle;                        //The structure for the USART1
extern UART_HandleTypeDef   Uart7Handle;                        //The structure for the UART7  
extern UART_HandleTypeDef   Uart4Handle;                        //The structure for the UART4
extern I2C_HandleTypeDef    I2C1Handle;                         //The structure for the I2C1
extern SDRAM_HandleTypeDef        SDRAMHandle;                  //The structure for SDRAM controller
extern NOR_HandleTypeDef          norHandle;                    //The structure for the FLASH controller
extern FMC_NORSRAM_TimingTypeDef  Timing;                       //The structure for the EXT FLASH
extern RSTP_COUNTER bpdu_count;

extern char Command[50];                                        //Store the command for CLI
extern uint8_t comd;                                            //Mark for the offset when inputing 
//extern uint32_t command_gen;                                  //If a command produce ,than the parameter increased by 1

extern uint16_t trunk_vector;                                   //The parameter indicates which ports are used to the trunk port 
extern uint16_t mask_table[8];                                  //mask entrys taht are load into the trunk mask table

extern INVARIATE_T my_invarite;                                 //The invariates
extern VETH_T  SET_INFO;                                        //The basic information for my set
extern VETH_IP_T SET_IP;                                        //The default IP address for my set
extern USER_INFO_T USER_INFO;
extern ETH_HandleTypeDef    Eth_Handle;                         //The structure for the ETH
extern uint8_t KEY_MAC[6];                                      //The MAC address for  my equipment
extern uint8_t MGMT_MAC[6];                                     //The muliticast MAC address for the RSTP dest address
extern uint8_t GET_NEXT[6];
extern uint8_t Targ_MAC[6];
extern uint8_t g_stats_mode;                                    //The counter mode targ for the 64octets 
extern uint8_t  CLI_targ;                                       //The targ for logout the CLI
extern uint8_t  code_targ;                                      //The targ for indicating the input string is invisible
extern uint8_t idle_targ;
extern uint8_t bp_targ;
extern uint8_t rstp_buf[4096];

extern char the_magic[14];
extern BIN_BUFFER_T recieved_package __attribute__((at(TEMP_UPGRADE_ADDR)));
//extern uint8_t com1dataBuffer1[COM_DATA_SIZE*COM_DATA_PACK_NUM];
//extern uint8_t com1dataBuffer2[COM_DATA_SIZE*COM_DATA_PACK_NUM];
extern uint32_t test_HaHa1;
extern uint32_t test_HaHa2;
extern uint32_t fifo_error_counter;
extern uint32_t transfer_error_counter;
extern uint32_t test_HaHa_else;
extern uint32_t test_HaHa_EN;
extern uint32_t test_HaHa_TC;
extern uint32_t test_HaHa_len;
/*@ref TIMx */
//测试使用的变量
//extern int isReadIPData;



void Delay(int i);
void Delays(int time);
void UART6_Puts(char * str);
void UART1_Puts(char * str);
void UART5_Puts(char * str);
void UART_Puts(char * str,UART_HandleTypeDef *uart);
uint8_t Get_DIAL_Mode(void);   //Get the dial-up switch vector(only 4 bits)
void Blink_LED_RUN(void);      //Blink the LED_RUN
void resetOrReinit(void);
void resetInit(void);
void FATfsDemo();
/*打印信息*/
void printPortStatus(int portNo);
void printStatus(int portNo);
void setSelfRefresh(void);
void setNormalMode(void);
/*My private lib functions*/
void Str_cpy(uint8_t *s1,uint8_t *s2,uint16_t len);
uint8_t Str_cmp(uint8_t *s1,uint8_t *s2,uint16_t len);
void WatchDogTimerReset();
void strCopy(char *s1,char *s2);
void HAL_UART_MspInita(UART_HandleTypeDef *huart);
void HAL_UART_MspInitb(UART_HandleTypeDef *huart);
//void uartRecieveIdleCallback(UART_HandleTypeDef *huart)
/*The callback function for the uart idle interrupt*/
void uartRecieveIdleCallback(UART_HandleTypeDef *huart);
#endif

