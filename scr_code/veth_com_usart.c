/**
*********************************************************************************
@File      : D:\ProgramFiles\KEIL\PRO\scr_code\com_usart.c
@Author    : Duke Lee
@Version   : V1.0
@Date      : 25-June-2014
@Breif     : The functions for my UART test
*********************************************************************************
**/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "stm32f4xx_hal_conf.h"
#include "veth_common.h"
#include "veth_com_uart.h"
#include "veth_hal_init.h"
#include "board_info.h"
/**
Define the Command set for my CLI base on USART1
**/
/*User name and password*/
char  User[10]    = "NoNoNoNo"; //The user name for VETH2000 set
char  Code[10]    = "Yaung";    //The password for user "NoNoNoNo"
/*The command on the root menu*/
char  MAC_op[10]  = "MAC oper"; //The command step into the MAC submenu  
	  char MAC_mod[30]="modify";
	  char MAC_dis [10] = "display";
char  IP_op[10]   = "IP oper";
	  char IP_mod[30] = "config";
	  char IP_dis[10] = "display";
char  SYSTEM_op[20] = "system oper";
      char SYS_RESTART[10] = "restart";
	  char SYS_REINIT[10]  = "reinit";
	  char SYS_RENAME[20]  = "rename";
	  char SYS_REENCRY[20] = "reencry";
char  RSTP_op[20]   = "RSTP oper";
	  char BRIDGE_PRI_mod[20]   = "bridge pri";
	  char BRIDGE_HLO_mod[20]   = "bridge hellot";
	  char BRIDGE_MAXAGE_mod[20]= "bridge max";
	  char BRIDGE_FWD_mod[20]   = "bridge fwd";
	  char BRIDGE_DISPLAY[10]   = "display";
char  UP_op[10] ="/";   //Step back the uper grade
char  Command_op[10]="?";
char  LOGOUT_op[10] = "logout";
/*Define the command array for UART*/
//char Command[30];       //Store the command for CLI
//uint8_t comd;              //Mark for the offset when inputing 
//uint32_t command_gen = 0;      //If a command produce ,than the parameter increased by 1
uint8_t  CLI_targ;
uint8_t  mac_targ;
uint8_t  ip_targ;
uint8_t  rstp_targ;
uint8_t  system_targ;
uint8_t  code_targ;
struct pt cli_pt;
/********************************************************************************/
//Function : CLI()
//Brief    : Function for realize the CLI(CLI : Command line interface)
//Parameterûhuart point to a UART Handler instanse 
/********************************************************************************/
PT_THREAD(CLI(struct pt *pt,UART_HandleTypeDef *huart,INVARIATE_T *invarite,uint32_t timeout))
 { 
	static uint32_t tem_command_gen;
//	static uint32_t tem_mscount;
	static char username[20];
	static char password[20];
	static uint8_t error_user=0;
	 
	PT_BEGIN(pt);
	 //sCount = 0;
	//tem_mscount = msCount;
	code_targ = 0;
	tem_command_gen= Get_command_gen();
	/*Start a command recieve IT*/
	UART_Recieve(huart,MAX_COMMAND_LEN);
	/*Waiting till a 'Carriage return' char command recieved or 5s timeout*/
	//while(msCount - tem_mscount  <= timeout)
	/*If there is a command in*/
	PT_WAIT_UNTIL(pt,(Get_command_gen()!= tem_command_gen)
					  &&Command[0] == 13);//{
	/*Judge  if the command a 'Carriage return' char to get into the CLI circle*/
	User:
		printf("Username:");
		CLI_targ = 1;
		tem_command_gen= Get_command_gen();
		Set_comd(0);
		/*Start a command recieve IT*/
		UART_Recieve(huart,MAX_COMMAND_LEN);
		while(1){
				  WatchDogTimerReset();
				  if(Get_command_gen() != tem_command_gen)
				  {
				    strncpy(username,Command,getComd());
				    //printf("username:%s\n\r\n\r",username);
				    break;
				  }
				}//end of the nearrest while(1)
				printf("\n\r\n\rPassword:");
				tem_command_gen= Get_command_gen();
				Set_comd(0);
				code_targ = 1;
				/*Start a command recieve IT*/
				UART_Recieve(huart,MAX_COMMAND_LEN);
		while(1)
		{
		  WatchDogTimerReset();
		  if(Get_command_gen() != tem_command_gen)
		  {
			strncpy(password,Command,getComd());
			//printf("\n\r\n\rpassword:%s\n\r\n\r",password);
			break;
		  }				  
		}
		if( strncmp(invarite->my_user.username,username,strlen(invarite->my_user.username)) == 0 &&
			strncmp(invarite->my_user.password,password,strlen(invarite->my_user.password)) ==0 )
		{
		  while(1){
					New_CLI(huart,invarite);
					if(CLI_targ == 0)
						break;
				  }
		}
		else
		{
		  printf("Invalid user!\n\r\n\r");
		  if(error_user++ <= 5){goto User;}
		}
		PT_END(pt);		
	//}//end of if(Command[0] == 13) 
	//PT_RESTART(pt);
	//}//end of if(Get_command_gen() != tem_command_gen)
  }//end of the function	
/**********************************************************************************/
//Function :    UART_Fillin_TX() 
//To do the transmit operations through IT manner
//parameter 'buffer' point to the COMx_TX_buffer[0]
//parameter 'size'   indicate the length of the packets you want to transmit
//parameter 'pdata'  point to the data you want to transmit
/**********************************************************************************/
void UART_Fillin_TX(uint16_t size,uint8_t *buffer,uint8_t *pdata)
  {
	uint16_t i;
	for (i = 0 ; i<size ;i++)
	  {
		buffer[i] = pdata[i];
	  }
  }

/********************************************************************************/
//Function : UART_Transmit
//include the operation of fill in the buffer and then send them out 
/********************************************************************************/	
void UART_Transmit(UART_HandleTypeDef *huart,uint8_t* pdata,uint16_t Size)
  {
/*At first,fill in the buffer with the data*/
	UART_Fillin_TX(Size,COM6_TX_buffer,pdata);
/*Call the Transmit function*/
	if(HAL_UART_Transmit_IT(huart,COM6_TX_buffer,Size)!= HAL_OK)
     {
      while(1){}   
     }
/*Wait for the state of the UART to get HAL_UART_STATE_READY*/
	while (HAL_UART_GetState(huart) != HAL_UART_STATE_READY)
	  {}
  }
/**********************************************************************************/
//Function :    UART_GETdata_RX() 
//To do the transmit operations through IT manner
//parameter 'buffer' point to the COMx_RX_buffer[0]
//parameter 'size'   indicate the length of the packets recieved
//parameter 'pdata'  point to the data where you want to store
/**********************************************************************************/
void UART_GETdata_RX(uint8_t *buffer,uint16_t size, uint8_t *pdata)
{
    uint16_t i;
	  for(i= 0;i<size;i++)
		{ 
        pdata[i] = buffer[i];
    }
}

/********************************************************************************/
//Function : UART_Recieve
//Recieve the data to the buffer
/********************************************************************************/	
MYERROR_INFO UART_Recieve(UART_HandleTypeDef *huart,uint16_t Size)
  {
/*When trigger a recieve operation ,Set the offset '0' to accept a new command*/
	Set_comd(0);	
/* Any data received will be stored "COM6_RX_buffer" buffer : the number max of  data received is 10 */
	if(huart->Instance == USART6)
	{
/*In able to use the rx buffer in a ring,you need to let the DOGaddr+1 just as : COM6_RX_buffer+1*/
	  if(HAL_UART_Receive_IT(huart,COM6_RX_buffer+1,Size) != HAL_OK)
	  { return COM6_RECIEVE_ERROR; } 
	}
	else
	  if(huart->Instance == USART1)
	  {
		if(HAL_UART_Receive_IT(huart,COM1_RX_buffer,Size) != HAL_OK)
		{  return COM1_RECIEVE_ERROR;  }
	  }
    else  
	  if(huart->Instance == UART5)
	  { 
		if(HAL_UART_Receive_IT(huart,COM5_RX_buffer,Size)!= HAL_OK)
		{ return COM5_RECIEVE_ERROR; }
	  }
	else 
	  if(huart->Instance == UART7)
	  {
		if(HAL_UART_Receive_IT(huart,COM7_RX_buffer,Size)!= HAL_OK)
		{  return COM7_RECIEVE_ERROR;  }
	  }
	else
	{
	  if(HAL_UART_Receive_IT(huart,COM4_RX_buffer,Size)!= HAL_OK)
	  { return COM4_RECIEVE_ERROR; }
	}
    return NO_ERROR;
}

/********************************************************************************/
//Function : Read_from_buffer()
//To get out the data in the bufer
//Parameter 'huart' indicate that from which buffer the data should be get
//Parameter ''
/********************************************************************************/
MYERROR_INFO Read_from_buffer(UART_HandleTypeDef *huart,uint8_t *Dest_Data,uint16_t Size)
  {
	uint16_t i;
	for (i =0 ;i<Size; i++ )
	{
	  if(huart->Instance == USART6)
		{ 
		  if(read_com6 == &COM6_RX_buffer[MAX_RX_BUFSIZE])
			read_com6 = &COM6_RX_buffer[0];
		  else 
			read_com6++;
/*Check if the com6_RX-buffer is empty or data done*/
		  if (read_com6 == huart->pRxBuffPtr)
			break;
/*The current byte of buffer has data,get it*/
		  Dest_Data[i] = *(read_com6);
		}
	  else 
		if (huart->Instance == USART1)
		  {
			if(read_com1 == &COM1_RX_buffer[MAX_RX_BUFSIZE])
			  read_com1 = &COM1_RX_buffer[0];
			else 
			  read_com1++;
/*Check if the com1_RX-buffer is empty or data done*/
			if(read_com1 == huart->pRxBuffPtr)
			  break;
			  Dest_Data[i] = *(read_com1);							
		  }
	  else
		{
		  if(read_com5 == &COM5_RX_buffer[MAX_RX_BUFSIZE])
			read_com5 = &COM5_RX_buffer[0];
		  else 
			read_com5++;
/*Check if the com1_RX-buffer is empty or data done*/	
		  if(read_com5 == huart->pRxBuffPtr )
			break;
		  Dest_Data[i] = *(read_com5);		 
		}
	}//end of for circle

	if(i<Size)
	  return GET_RX_BUFFER_ERROR;
	else
	  return NO_ERROR;			 
  }
/********************************************************************************/
//Function : displayMacCommand()
//Brief    : Display the MAC command
/********************************************************************************/
static void displayMacCommand()
  {
	printf("\n\r>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
	printf("The Instuction sets is as follows:\n\r\n\r");
	printf(" Command    Format                     Brief \n\r\n\r");
	printf("->modify  modify <MAC_address>         modifying the MAC address\n\r\n\r");
	printf("->display display                      display the address\n\r\n\r");
	printf("->/       /                            return back to the main menu\n\r\n\r");
	printf("->?       ?                            display the IP commands\n\r\n\r");
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
  }
/********************************************************************************/
//Function : MAC_CLI()
//Brief    : Realize the real CLI
/********************************************************************************/
void MAC_CLI(UART_HandleTypeDef *huart,INVARIATE_T *invarite)
  {
    uint32_t tem_gen; 
    uint8_t  submenu_state;
//		uint8_t loop;

	printf("MAC->");
	tem_gen= Get_command_gen();
	/*Start a command recieve IT*/
	UART_Recieve(huart,MAX_COMMAND_LEN);
	while(1)
	  {
		Blink_LED_RUN();
		WatchDogTimerReset();
		if(Get_command_gen() != tem_gen)
		  {
#ifdef MY_DEBUG
			for(loop =0 ;loop < 30 ;loop++)
			  {
			printf("%x  ",Command[loop]);
			  }
			printf("\n\r");
#endif
			if(strncmp(Command,MAC_mod,3)==0)
				submenu_state = 0;
			else
			  if(strncmp(Command,MAC_dis,3)==0)
				submenu_state = 1;
			else
			  if(strncmp(Command,UP_op,1)==0)
				submenu_state = 2;
			else
			  if(strncmp(Command,Command_op,1)==0)
				submenu_state = 3;
			else
				submenu_state = 0xff;
			printf("\n\r\n\r");
			switch (submenu_state) {
				  case 0:
					if( Modify_the_MAC(invarite,(uint8_t *)Command) != 1)
					  {
						printf("Input error! Try again...\n\r\n\r");
						printf("MAC->");
						tem_gen= Get_command_gen();
						UART_Recieve(huart,MAX_COMMAND_LEN);
						continue;
					  }
					return;
				  case 1:
					Display_the_MAC(invarite->The_MAC);
					return;
				  case 2:
				    mac_targ = 0;
					return;
				  case 3:
				    displayMacCommand();
				    return;
				  default :
						printf("Input error! Try again...\n\r\n\r");
						printf("MAC->");
						tem_gen= Get_command_gen();
						UART_Recieve(huart,MAX_COMMAND_LEN);
						continue;
				  }//end of switch
		}//end of if(Get_command) 
				
	  }//end of while(1)
  }

/********************************************************************************/
//Function : displayIpCommand()
//Brief    : Realize operation for IP's CLI
/********************************************************************************/	
static void displayIpCommand()
  {
	printf("\n\r>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
	printf("The Instuction sets is as follows:\n\r\n\r");
	printf(" Command    Format                             Brief \n\r\n\r");
	printf("->config  config <IpAddress> <IpMask> <IpNet>  configure the IP address\n\r\n\r");
	printf("->display display                              display the IP address\n\r\n\r");
	printf("->/       /                                    return back to the main menu\n\r\n\r");
	printf("->?       ?                                    display the IP commands\n\r\n\r");
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");	
  }
/********************************************************************************/
//Function : IP_CLI()
//Brief    : Realize operation for IP's CLI
/********************************************************************************/	
void IP_CLI(UART_HandleTypeDef *huart,VETH_IP_T *ip)
  {
	uint32_t tem_gen; 
	uint8_t  submenu_state;
//		uint8_t loop;

	printf("IP->");
	tem_gen= Get_command_gen();
	/*Start a command recieve IT*/
	UART_Recieve(huart,MAX_COMMAND_LEN);
	while(1)
	  {
		Blink_LED_RUN();
		WatchDogTimerReset();
		if(Get_command_gen() != tem_gen)
		  {
#ifdef MY_DEBUG
			for(loop =0 ;loop < 30 ;loop++)
			  {
			printf("%x  ",Command[loop]);
			  }
			printf("\n\r");
#endif
			if(strncmp(Command,IP_mod,5)==0)
				submenu_state = 0;
			else
			  if(strncmp(Command,IP_dis,5)==0)
				submenu_state= 1;
			else
			  if(strncmp(Command,UP_op,1)==0)
				submenu_state = 2;
			else
			  if(strncmp(Command,Command_op,1)==0)
				submenu_state = 3;
			else
			    submenu_state = 0xff;
			switch (submenu_state) {
				  case 0:
					if( modifyIpAddr(ip,Command) == 0)
					  {
						printf("Input error! Try again...\n\r\n\r");
						printf("IP->");
						tem_gen= Get_command_gen();
						UART_Recieve(huart,MAX_COMMAND_LEN);
						continue;
					  }
					return;
				  case 1:
					displayIpInfo(ip);
					return;
				  case 2:
				    ip_targ = 0;
					return;
				  case 3:
				    displayIpCommand();
					return;
				  default :
						printf("Input error! Try again...\n\r\n\r");
						printf("IP->");
						tem_gen= Get_command_gen();
						UART_Recieve(huart,MAX_COMMAND_LEN);
						continue;
				  }//end of switch
		}//end of if(Get_command) 
				
	  }//end of while(1)
  }
  
/********************************************************************************/
//Function : displaySystemCommand()
//Brief    : Display the system commands
/********************************************************************************/	
void displaySystemCommand()
  {	
	printf("\n\r>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
	printf("The Instuction sets is as follows:\n\r\n\r");
	printf(" Command    Format                     Brief \n\r\n\r");
	printf("->restart   restart                    restart the sets\n\r\n\r");
	printf("->reinit    reinit                     reinit the configuration for sets\n\r\n\r");
	printf("->rename    rename <new_user_name>     modify the username for CLI\n\r\n\r");
	printf("->re-encry  reencry  <new password>    modify the password for CLI\n\r\n\r");
	printf("->/         /                          return back to the main menu\n\r\n\r");
	printf("->?         ?                          display the system commands\n\r\n\r");
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
  }
/********************************************************************************/
//Function : SYSTEM_CLI()
//Brief    : Realize the operation for SYSTEM's CLI
/********************************************************************************/	 
void SYSTEM_CLI(UART_HandleTypeDef *huart,INVARIATE_T *invarite)
  {
	uint32_t tem_gen; 
    uint8_t  submenu_state;
	
	printf("system->");	
	tem_gen= Get_command_gen();
	/*Start a command recieve IT*/
	UART_Recieve(huart,MAX_COMMAND_LEN);
	while(1)
	  {
		Blink_LED_RUN();
		WatchDogTimerReset();
		if(Get_command_gen() != tem_gen)
		  {
#ifdef MY_DEBUG
			for(loop =0 ;loop < 30 ;loop++)
			  {
			printf("%x  ",Command[loop]);
			  }
			printf("\n\r");
#endif
			if( strncmp(Command,SYS_RESTART,4)==0 )
				submenu_state = 0;
			else
			  if( strncmp(Command,SYS_REINIT,4)==0 )
				submenu_state = 1;
			else
			  if( strncmp(Command,SYS_RENAME,4)==0 )
			    submenu_state = 2;
			else
			  if( strncmp(Command,SYS_REENCRY,4)==0 )
			    submenu_state = 3;
			else
			  if( strncmp(Command,UP_op,1)==0 )
				submenu_state = 4;
			else
			  if(strncmp(Command,Command_op,1)==0)
				submenu_state = 5;
			else
				submenu_state = 0xff;
			printf("\n\r\n\r");
			switch (submenu_state) {
				  case 0:
				    printf("restarting...\n\r\n\r");
				    NVIC_SystemReset();
					return;
				  case 1:
					InVariate_Default(invarite);
					updateNewConfig();
					return;
				  case 2:
				    if(modifyUserName(invarite,Command) == 0)
					  {
						printf("Input error! Try again...\n\r\n\r");
						printf("system->");
						tem_gen= Get_command_gen();
						UART_Recieve(huart,MAX_COMMAND_LEN);
						continue;
					  }
					return;
				  case 3:
				    //code_targ = 1;
				    if(modifyPassWord(invarite,Command) == 0)
					  {
						printf("Input error! Try again...\n\r\n\r");
						printf("system->");
						tem_gen= Get_command_gen();
						UART_Recieve(huart,MAX_COMMAND_LEN);
						continue;
					  }
					return;
				  case 4:
				    system_targ = 0;
					return;
				  case 5:
				    displaySystemCommand();
					return;
				  default :
				    {
					  printf("Input error! Try again...\n\r\n\r");
					  printf("system->");
					  tem_gen= Get_command_gen();
					  UART_Recieve(huart,MAX_COMMAND_LEN);
					  continue;
					}
				  }//end of switch
		}//end of if(Get_command) 
				
	  }//end of while(1)
	
  }

/********************************************************************************/
//Function : displayRstpCommand()
//Brief    : Display the basic RSTP Commands
/********************************************************************************/
void displayRstpCommand()
  {	 
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
	printf("The Instuction sets is as follows:\n\r\n\r");
	printf(" Command        Format                     Brief \n\r\n\r");
	printf("->bridge pri    bridge prio x(0~61440)  modify the bridge priority for RSTP\n\r\n\r");
	printf("->bridge hlot   bridge hlot x(1~10)     modify the bridge hellotime for RSTP\n\r\n\r");
	printf("->bridge maxage bridge hlot x(6~40)     modify the bridge maxage for RSTP\n\r\n\r");
	printf("->bridge fwdtim bridge fwdt x(4~30)     modify the bridge forward time for RSTP\n\r\n\r");
	printf("->display       display                 display the bridge information for RSTP\n\r\n\r");
	printf("->/             /                       return back to the main menu\n\r\n\r");
	printf("->?             ?                       display the RSTP commands\n\r\n\r");
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
  }
  
/********************************************************************************/
//Function : RSTP_CLI()
//Brief    : Realize the operation for SYSTEM's CLI
/********************************************************************************/	 
void RSTP_CLI(UART_HandleTypeDef *huart,INVARIATE_T *invarite)
  {
	uint32_t tem_gen; 
    uint8_t  submenu_state;
	
	printf("RSTP->");	
	tem_gen= Get_command_gen();
	/*Start a command recieve IT*/
	UART_Recieve(huart,MAX_COMMAND_LEN);
	while(1)
	  {
		Blink_LED_RUN();
		if(Get_command_gen() != tem_gen)
		  {
#ifdef MY_DEBUG
			for(loop =0 ;loop < 30 ;loop++)
			  {
			printf("%x  ",Command[loop]);
			  }
			printf("\n\r");
#endif
			if(strncmp(Command,BRIDGE_PRI_mod,8)==0)
				submenu_state = 0;
			else
			  if(strncmp(Command,BRIDGE_HLO_mod,8)==0)
				submenu_state = 1;
			else
			  if(strncmp(Command,BRIDGE_MAXAGE_mod,8)==0)
				submenu_state = 2;
			else
			  if(strncmp(Command,BRIDGE_FWD_mod,8)==0)
				submenu_state = 3;
			else
			  if(strncmp(Command,BRIDGE_DISPLAY,5)==0)
				submenu_state = 4;
			else
			  if(strncmp(Command,UP_op,1) == 0)
				submenu_state = 5;
			else
			  if(strncmp(Command,Command_op,1) == 0)
				submenu_state = 6;
			else
				submenu_state = 0xff;
			printf("\n\r\n\r");
			switch (submenu_state) {
				  case 0:
					if(modifyRstpInfo(invarite,Command,BRIDGE_PRIORITY)!= 1 )
					{
						printf("Input error! Try again...\n\r\n\r");
						printf("RSTP->");
						tem_gen= Get_command_gen();
						UART_Recieve(huart,MAX_COMMAND_LEN);
						continue;
					}
					return;
				  case 1:
					if(modifyRstpInfo(invarite,Command,BRIDGE_PRIORITY) != 1 )
					{
						printf("Input error! Try again...\n\r\n\r");
						printf("RSTP->");
						tem_gen= Get_command_gen();
						UART_Recieve(huart,MAX_COMMAND_LEN);
						continue;
					}
					return;
				  case 2:
				    if( modifyRstpInfo(invarite,Command,BRIDGE_PRIORITY)!= 1 )
					{
						printf("Input error! Try again...\n\r\n\r");
						printf("RSTP->");
						tem_gen= Get_command_gen();
						UART_Recieve(huart,MAX_COMMAND_LEN);
						continue;
					}
					return;
				  case 3:
				    if( modifyRstpInfo(invarite,Command,BRIDGE_PRIORITY) != 1 )
					{
						printf("Input error! Try again...\n\r\n\r");
						printf("RSTP->");
						tem_gen= Get_command_gen();
						UART_Recieve(huart,MAX_COMMAND_LEN);
						continue;
					}
					return;
				  case 4:
				    displayRstpInfo(invarite);
				    return;
				  case 5:
				    rstp_targ = 0;
					return;
				  case 6:
				    displayRstpCommand();
					return;
				  default :
						printf("Input error! Try again...\n\r\n\r");
						printf("RSTP->");
						tem_gen= Get_command_gen();
						UART_Recieve(huart,MAX_COMMAND_LEN);
						continue;
				  }//end of switch
		}//end of if(Get_command) 
				
	  }//end of while(1)
	
  } 

/********************************************************************************/
//Function : New_CLI()
//Brief    : Realize the real CLI
/********************************************************************************/
void New_CLI(UART_HandleTypeDef *huart,INVARIATE_T *invarite)
  {
    uint32_t tem_gen; 
    uint8_t  menu_state;
	  
	
    printf("\n\r\n\r->CLI Running...\n\r\n\r");	
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
	printf("The Instuction sets is as follows :\n\r\n\r");
	printf("->MAC oper    operation for the MAC address\n\r\n\r");
	printf("->IP oper     operation for the IP address\n\r\n\r");
	printf("->RSTP oper   operation for the RSTP brigde information\n\r\n\r");
	printf("->system oper operation for the system\n\r\n\r");
	printf("->'/'         operation for return back to the main menu\n\r\n\r");
	printf("->logout      operation for logout the CLI system\n\r\n\r");
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\r\n\r");
	printf("->");
	  
	tem_gen = Get_command_gen();
	/*Start a command recieve IT*/
	UART_Recieve(huart,MAX_COMMAND_LEN);
	while(1)/*The 'while'for*/
	  {
		Blink_LED_RUN();
		WatchDogTimerReset();
		if(Get_command_gen() != tem_gen)
		  {
			if(strncmp(Command,MAC_op,3) == 0)
				{menu_state = 0;}
			else
			  if(strncmp(Command,IP_op,2) == 0)
				{menu_state = 1;}
			else
			  if(strncmp(Command,RSTP_op,4) == 0)
			    {menu_state = 2;}
			else
			  if(strncmp(Command,SYSTEM_op,5) == 0)
			    {menu_state = 3;}
			else
			  if(strncmp(Command,UP_op,2) == 0)
				{ menu_state = 4;}
			else
			  if(strncmp(Command,LOGOUT_op,5) == 0)
				{ menu_state = 5;}
			else
				menu_state = 0xff;
	/*If there are other functions,add them*/
			mac_targ    = 1;
			ip_targ     = 1;
			rstp_targ   = 1;
			system_targ = 1;
			printf("\n\r\n\r");
			while(1){
			  switch (menu_state) {
				case 0:
						MAC_CLI(huart,invarite);
						if(mac_targ ==0)
						  break;
						else
						  continue;
				case 1:	  
						IP_CLI(huart,&board.ip_config);
						if(ip_targ ==0)
						  break;
						else
						  continue;
//				case 2:	  
//						RSTP_CLI(huart,invarite);
//						if(rstp_targ ==0)
//						  break;
//						else
//						  continue;
				case 3:	  
						SYSTEM_CLI(huart,invarite);
						if(system_targ ==0)
						  break;
						else
						  continue;
				case 4:
						return;
				case 5:
						CLI_targ = 0;
						return;
				default:
						break;
			    }break;//end of switch
			}//end of while(1)
            break;							 
		  }//end of if(Get_command_gen... 
			 
	  }//end of while(1)
  }//end of the function


/********************************************************************************/
//Function : getChar(uint8_t *p_data,UART_HandleTypeDef *puart)
//Brief    : Get a char recieved from the UART
//Retval   : None
/********************************************************************************/	
void getChar(uint8_t *p_data,uint16_t len)
  {
	while(HAL_UART_Receive(&Uart1Handle,p_data,len,0x000F) != HAL_OK)
     {}	
  }	  
 
/********************************************************************************/
//Function : sendChar(uint8_t p_data,UART_HandleTypeDef *puart)
//Brief    : Send a char to the UART
//Retval   : None
/********************************************************************************/		 
void sendChar(uint8_t *p_data,uint16_t len)
  {
	HAL_UART_Transmit(&Uart1Handle,p_data,1,0x00FF);
  }

