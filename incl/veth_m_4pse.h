/**
****************************************************************************************
@File    : D:\ProgramFiles\KEIL\hal\incl\veth_m_4pse.h
@Author  : Duke Lee
@Date    : 06-August-2014
@Version : V1.0.0
@Breif   : The header file for veth_m_4pse.c
		       
****************************************************************************************
**/

#ifndef _M_4PSE_H__
#define _M_4PSE_H__

#include <stdint.h>

/*Define the event struct*/
typedef struct power_event_t  {
  uint8_t  PWEN_CHG;     //Power enable change event for the port
  uint8_t  PG_CHG;       //PGOOD change event for the port
}POWER_EVENT_T;

typedef struct detect_event_t {
  uint8_t DET_END;       //Detection completed on the port 
  uint8_t CLS_END;       //Classification completed on the port
}DET_END_T;

typedef struct fault_event_t  {
  uint8_t  DIS;         //DC load disconnect timeout on the port
  uint8_t  TCUT;        //Overcurrent disconnect timeout on the port 
}FAULT_EVENT_T;
	
typedef struct startup_event_t {
  uint8_t ICV;         //Current-limit disconnect timeout on port
  uint8_t TSTART;      //Startup failure on port
}STARTUP_EVENT_T;

/*Define the struct of the port event*/
typedef struct port_event_t {
  POWER_EVENT_T    port_power;
  DET_END_T        port_end;
  FAULT_EVENT_T    port_fault;
  STARTUP_EVENT_T	 port_startup;
}PORT_EVENT_T;

/*Define the configuration struct that the port use*/
typedef struct port_config_t {
  uint8_t        port_op_mode;      //only 2 low bits are valid,mode '00'for 'SHUTDWN','01'for 'MANUAL','10'for 'SEMI',and '11' for 'AUTO'
  uint8_t        port_mid_span;     //port_mid_span: only 4 low bits are valid ,each bit refers to a port    
  uint8_t        port_power;        //'1' for power on , '0'for power off
  uint8_t        port_ICUT;         //when "bit[6](ICUTRNG)=1",the icut value =bit[5:0]*37.5mA.Or icut = bit[5:0]*18.75mA 
}PORT_CONFIG_T;

/*Define the enum for detection result*/
typedef enum PSE_detect_result {
  NONE = 0x00,                     //Detection status unknown (default)
  DCP,                             //Positive DC supply connected at the port (VAGND - VOUT_ < 1V)
  HIGH_CAP,                        //High capacitance at the port (> 8.5uF (typ))
  RLOW,                            //Low resistance at the port (RDET < 15kohm)
  DET_OK,                          //Detection pass (15kI > RDET > 33kohm)
  RHIGH,                           //High resistance at the port (RDET > 33kohm)
  OPEN,                            //Open port (IOUT_ < 10uA)
  DCN                              //Low impedance to VEE at the port (VOUT_ - VEE < 2V)
}PSE_DETECT_T;

/*Define the enum for classification result*/
typedef enum PSE_classi_result {
  UNKNOWN,
  CLASS_2,
  CLASS_3,
  CLASS_4,
  CLASS_5,
  CLASS_0,
  CURRENT_LIMIT
}PSE_CLASS_T;

/*Define the struct for each PSE port's status*/
typedef struct PSE_port_status_t {
  PSE_DETECT_T detection;
  PSE_CLASS_T  classification;
  uint8_t  power;
  uint16_t current;
  uint16_t voltage;	
}PSE_PORT_T;

/*Define some const*/
#define  MEM_ADDR_SIZE    0x0001       //UNIT : (byte)
#define  TIME_OUT         0x0fff       //UNIT : (millisecond)

/**
The device address
**/
#define  CHIP_1                       ((uint8_t)0x40)
#define  CHIP_2                       ((uint8_t)0x41)

/**
The chip register address
**/
//INTERRUPT
#define  REG_INTERRUPT                ((uint8_t)0x00)
#define  REG_INTERRUPT_MASK_X         ((uint8_t)0x01)

//EVENTS
#define  REG_POWER_EVENT              ((uint8_t)0x02)
#define  REG_POWER_EVENT_COR          ((uint8_t)0x03)
#define  REG_DETECT_EVENT             ((uint8_t)0x04)
#define  REG_DETECT_EVENT_COR         ((uint8_t)0x05)
#define  REG_FAULT_EVENT              ((uint8_t)0x06)
#define  REG_FAULT_EVENT_COR          ((uint8_t)0x07)
#define  REG_STARTUP_EVENT            ((uint8_t)0x08)
#define	 REG_STARTUP_EVENT_COR        ((uint8_t)0x09)
#define  REG_SUPPLY_EVENT             ((uint8_t)0x0a)
#define  REG_SUPPLY_EVENT_COR         ((uint8_t)0x0b)

//STATUS
#define REG_PORT1_STATUS              ((uint8_t)0x0c)    //These registers indicates the detection and classification results
#define REG_PORT2_STATUS              ((uint8_t)0x0d)    //
#define REG_PORT3_STATUS              ((uint8_t)0x0e)    //
#define REG_PORT4_STATUS              ((uint8_t)0x0f)    //
#define REG_POWER_STATUS              ((uint8_t)0x10)
#define REG_PIN_STATUS                ((uint8_t)0x11)

//CONFIGURATION
#define REG_OPERATING_MODE            ((uint8_t)0x12)
#define REG_DISCONNECT_ENBALE         ((uint8_t)0x13)
#define REG_DETECTION_CLASS_ENBALE    ((uint8_t)0x14)
#define REG_MIDSPAN_ENBALE            ((uint8_t)0x15)
#define REG_MISCELLANEOUS             ((uint8_t)0x17)

//PUSHBUTTONS
#define REG_DETECT_CLASS_PUSHBUTTON   ((uint8_t)0x18)
#define REG_POWER_ENABLE_PUSHBUTTON   ((uint8_t)0x19)
#define REG_GLOBAL_PUSHBUTTON         ((uint8_t)0x1a)

//GENERAL
#define REG_CHIP_ID                   ((uint8_t)0x1b)
#define REG_CLASS5_ENBALE             ((uint8_t)0x1c)
#define REG_TLIM1_2_PROGRAMME         ((uint8_t)0x1e)
#define REG_TLIM3_4_PROGRAMME         ((uint8_t)0x1f)

//20~2f reserved for MAXIM
//CURRENT/VOLTAGE 
#define REG_PORT1_CURRENT_LO          ((uint8_t)0x30)  //PORT 1
#define REG_PORT1_CURRENT_HI          ((uint8_t)0x31)  //
#define REG_PORT1_VOLTAGE_LO          ((uint8_t)0x32)  //
#define REG_PORT1_VOLTAGE_HI          ((uint8_t)0x33)  //
#define REG_PORT2_CURRENT_LO          ((uint8_t)0x34)  //PORT 2
#define REG_PORT2_CURRENT_HI          ((uint8_t)0x35)  //
#define REG_PORT2_VOLTAGE_LO          ((uint8_t)0x36)  //
#define REG_PORT2_VOLTAGE_HI          ((uint8_t)0x37)  //
#define REG_PORT3_CURRENT_LO          ((uint8_t)0x38)  //PORT 3
#define REG_PORT3_CURRENT_HI          ((uint8_t)0x39)  //
#define REG_PORT3_VOLTAGE_LO          ((uint8_t)0x3a)  //
#define REG_PORT3_VOLTAGE_HI          ((uint8_t)0x3b)  //
#define REG_PORT4_CURRENT_LO          ((uint8_t)0x3c)  //PORT 4
#define REG_PORT4_CURRENT_HI          ((uint8_t)0x3d)  //
#define REG_PORT4_VOLTAGE_LO          ((uint8_t)0x3e)  //
#define REG_PORT4_VOLTAGE_HI          ((uint8_t)0x3f)  //

//OTHER FUNCTIONS
#define REG_FIRMWARE                  ((uint8_t)0x41)
#define REG_WATCH_DOG                 ((uint8_t)0x42)
#define REG_DEVELOPER_ID_AND_VERSION  ((uint8_t)0x43)
#define REG_HIGH_POWER_ENBALE         ((uint8_t)0x44)
#define REG_PORT1_GPMD                ((uint8_t)0x46)  //PORT 1
#define REG_PORT1_ICUT                ((uint8_t)0x47)
#define REG_PORT1_ILIM                ((uint8_t)0x48)
#define REG_PORT1_HIGH_POWER          ((uint8_t)0x49)
#define REG_PORT2_GPMD                ((uint8_t)0x4b)  //PORT 2
#define REG_PORT2_ICUT                ((uint8_t)0x4c)
#define REG_PORT2_ILIM                ((uint8_t)0x4d)
#define REG_PORT2HIGH_POWER           ((uint8_t)0x4e)
#define REG_PORT3_GPMD                ((uint8_t)0x50)  //PORT3
#define REG_PORT3_ICUT                ((uint8_t)0x51)
#define REG_PORT3_ILIM                ((uint8_t)0x52)
#define REG_PORT3HIGH_POWER           ((uint8_t)0x53)
#define REG_PORT4_GPMD                ((uint8_t)0x55)  //PORT 4
#define REG_PORT4_ICUT                ((uint8_t)0x56)
#define REG_PORT4_ILIM                ((uint8_t)0x57)
#define REG_PORT4HIGH_POWER           ((uint8_t)0x58)

/*Use for the test for the POE cgi functions*/
void poeDemo(void);
/*Functions for get The status of the chip*/
void getPoePortStatus(uint8_t portNo,PSE_PORT_T* portPSE);
/*Functions for get the event of the port*/
void getPoePortEvent(uint8_t portNo,PORT_EVENT_T *pevent);
/*Functions for set the configuration of the port*/
uint8_t setPoePortConfig(uint8_t portNo,PORT_CONFIG_T* port_config);
/*Functions for the reset*/
uint8_t setPoePortOn(uint8_t portNo);
uint8_t setPoePortDown(uint8_t portNo);
uint8_t setPoeChipReset(uint8_t portNo);
uint8_t setPoePortReset(uint8_t portNo);

#endif 
