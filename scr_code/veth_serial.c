/**
**************************************************************************
@File    :  hal\src_code\veth_serial.c
@Author  :  Duke Lee
@Verdion :  V1.0
@Date    :  14-Jan-2015
@brief   :  Functions for the serialserver and serialclient
**************************************************************************
**/
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
//#include <rt_heap.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "veth_common.h"
#include "veth_serial.h"
#include "veth_com_uart.h"
#include "SerialUtility.h"
#include "veth_COM.h"
#include "FIFO.h"
/*Macro definations*/
#define GAIN_DATA_TIMEOUT     100
//#define GAIN_DATA_TIMEOUT     0
#define GAI_DATA_TIMEOUT      0
#define GAIN_DATA_LENGTH      1500
#define UART_TIMEOUT          22000
#define RCV_BUFFER_NUM        15
#define SERIAL_TOTAL_LENGTH   0//292000
#define NODE_MAX_NUM          2920
#define DOUBLE_BUFFER_LENGTH  800
#define LITTLE_DATA_TIMEOUT   1500
#define TEST_TRANSFER_LEN     500000
/*@PS*/
/*These strings only used for testing program*/
uint8_t testString[20] = "My name is Jim Green";
uint8_t testStrings[20] = "My name is Han MeiMe";
uint8_t testStringss[20] = "My name is Tracy Li";
uint8_t testStringsss[20] = "My name is Duke Li";
uint8_t my_str[20] ="abcdefg";
uint8_t targ=0;
uint8_t test_String[COM_DATA_SIZE*COM_DATA_PACK_NUM] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
//uint8_t test_array[500];
uint32_t serial_send_total_length_1 = 0;// 串口发送的数据总长度
uint32_t serial_send_total_length_2 = 0;// 串口发送的数据总长度
uint32_t serial_send_total_length_3 = 0;// 串口发送的数据总长度
uint32_t serial_send_total_length_4 = 0;// 串口发送的数据总长度
uint32_t test_total_length  =0;
uint32_t test_total_FIFO_data = 0;
uint32_t test_FIFO_before = 0;
uint32_t test_FIFO_after  = 0;
uint32_t test_HaHa1 = 0;
uint32_t test_HaHa2 = 0;
uint32_t fifo_error_counter = 0;
uint32_t transfer_error_counter = 0;
uint32_t test_HaHa_else =0 ;
uint32_t temp_register ;
uint32_t test_HaHa_EN = 0 ;
uint32_t test_HaHa_TC = 0 ;
uint32_t test_HaHa_len =0 ;
uint32_t test_rcv_length = 0;
uint8_t  idle_targ = 0;
uint32_t test_string_len=0;
uint16_t  i;
uint16_t  x=0;
uint16_t  the_int_wait;
//uint32_t count_targ = 0;
//uint32_t test_total_get_length = 0;
//uint16_t test_chk_byte;
//uint32_t test_error_byte=0;
//uint32_t test_rcv_len = 0 ;
//uint8_t  double_finish_targ = 0;
//uint8_t  single_finish_targ = 1;

/*Define data buffer for the serialserver*/
uint8_t com1dataBuffer1[COM_DATA_SIZE*COM_DATA_PACK_NUM] __attribute__((section("COMDATABUFFER")));
uint8_t com1dataBuffer2[COM_DATA_SIZE*COM_DATA_PACK_NUM] __attribute__((section("COMDATABUFFER")));
uint8_t com1dataBuffer[COM_DATA_SIZE*COM_DATA_PACK_NUM]  __attribute__((section("COMDATABUFFER")));
uint8_t com2dataBuffer1[COM_DATA_SIZE*COM_DATA_PACK_NUM] __attribute__((section("COMDATABUFFER")));
uint8_t com2dataBuffer2[COM_DATA_SIZE*COM_DATA_PACK_NUM] __attribute__((section("COMDATABUFFER")));
uint8_t com2dataBuffer[COM_DATA_SIZE*COM_DATA_PACK_NUM]  __attribute__((section("COMDATABUFFER")));
uint8_t com3dataBuffer1[COM_DATA_SIZE*COM_DATA_PACK_NUM] __attribute__((section("COMDATABUFFER")));
uint8_t com3dataBuffer2[COM_DATA_SIZE*COM_DATA_PACK_NUM] __attribute__((section("COMDATABUFFER")));
uint8_t com3dataBuffer[COM_DATA_SIZE*COM_DATA_PACK_NUM]  __attribute__((section("COMDATABUFFER")));
uint8_t com4dataBuffer1[COM_DATA_SIZE*COM_DATA_PACK_NUM] __attribute__((section("COMDATABUFFER")));
uint8_t com4dataBuffer2[COM_DATA_SIZE*COM_DATA_PACK_NUM] __attribute__((section("COMDATABUFFER")));
uint8_t com4dataBuffer[COM_DATA_SIZE*COM_DATA_PACK_NUM]  __attribute__((section("COMDATABUFFER")));
/*Define the data buffer for serialClient*/
uint8_t com1rcvBuffer1 [COM_RCV_DATA_SIZE*RCV_BUFFER_NUM] ;//__attribute__((section("COMDATABUFFER")));
uint8_t com1rcvBuffer2 [COM_RCV_DATA_SIZE*RCV_BUFFER_NUM] ;
uint8_t com2rcvBuffer1 [COM_RCV_DATA_SIZE*RCV_BUFFER_NUM] ;//__attribute__((section("COMDATABUFFER")));
uint8_t com2rcvBuffer2 [COM_RCV_DATA_SIZE*RCV_BUFFER_NUM] ;
uint8_t com3rcvBuffer1 [COM_RCV_DATA_SIZE*RCV_BUFFER_NUM] ;//__attribute__((section("COMDATABUFFER")));
uint8_t com3rcvBuffer2 [COM_RCV_DATA_SIZE*RCV_BUFFER_NUM] ;
uint8_t com4rcvBuffer1 [COM_RCV_DATA_SIZE*RCV_BUFFER_NUM] ;//__attribute__((section("COMDATABUFFER")));
uint8_t com4rcvBuffer2 [COM_RCV_DATA_SIZE*RCV_BUFFER_NUM] ;
/*Define the FIFO for the four DMA stream*/
uint8_t com1dataFIFO[COM_DATA_SIZE*10];
uint8_t com2dataFIFO[COM_DATA_SIZE*10];
uint8_t com3dataFIFO[COM_DATA_SIZE*10];
uint8_t com4dataFIFO[COM_DATA_SIZE*10];
uint8_t com1rcvFIFO[COM_RCV_DATA_SIZE*10];
uint8_t com2rcvFIFO[COM_RCV_DATA_SIZE*10];
uint8_t com3rcvFIFO[COM_RCV_DATA_SIZE*10];
uint8_t com4rcvFIFO[COM_RCV_DATA_SIZE*10];
FIFO    com1FIFO;
FIFO    com2FIFO;
FIFO    com3FIFO;
FIFO    com4FIFO;
FIFO    com1rxFIFO;
FIFO    com2rxFIFO;
FIFO    com3rxFIFO;
FIFO    com4rxFIFO;
/*Define the buffer status*/
DMA_MEM_STATUS_T com1DmaStatus;
DMA_MEM_STATUS_T com2DmaStatus;
DMA_MEM_STATUS_T com3DmaStatus;
DMA_MEM_STATUS_T com4DmaStatus;
UART_QUEUE_T     com1RcvStatus;
UART_QUEUE_T     com2RcvStatus;
UART_QUEUE_T     com3RcvStatus;
UART_QUEUE_T     com4RcvStatus;

/*private functions*/
void uartDmaXferCpltCallback(DMA_HandleTypeDef *hdma);
void uartDmaXferM1CpltCallback(DMA_HandleTypeDef *hdma);
void DmaXferCpltCallback(DMA_HandleTypeDef *hdma);
static void uartDmaRcvCpltCallback(DMA_HandleTypeDef *hdma);
static void uartDmaRcvM1CpltCallback(DMA_HandleTypeDef *hdma);
static void uartRecieveCpltCallback(DMA_HandleTypeDef *hdma);
//static uint8_t getUartRecievedData(UART_HandleTypeDef *huart,uint8_t bufferNo);
static uint8_t getUartRecievedData(UART_HandleTypeDef *huart);
HAL_StatusTypeDef halUartRecieveDma(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status, uint32_t Timeout);
/******************************************************************************/
/*Function : Delayserial()*/
/*Brief    : Delay time second for serial use*/
/*Parameter: time*/
/*retval   : None*/
/******************************************************************************/
static void Delayserial(uint32_t time) {
    uint32_t tem_scount;
    tem_scount = sCount;
    while(sCount - tem_scount <= time) {
    }
}

/********************************************************************************************************/
/****************************Uart transfer functions for serialserver************************************/
/********************************************************************************************************/

/******************************************************************************/
/*Function : serialServer()*/
/*Brief    : The serialServer function for the highest while(1)*/
/*Parameter: */
/*retval   : None*/
/******************************************************************************/
void serialServer() {
    static uint32_t tem_mscont = 0;

    if( (msCount - tem_mscont) >=  GAI_DATA_TIMEOUT) {
#ifdef MY_DEBUG
        halUartTransmitDma(&Uart6Handle);
        halUartTransmitDma(&Uart5Handle);
        halUartTransmitDma(&Uart7Handle);
        halUartTransmitDma(&Uart4Handle);
        tem_mscont = msCount;
        comDemo();
#else
		//halUartTransmitDma(&Uart6Handle);
		uartTransfer(&Uart6Handle);
		uartTransfer(&Uart5Handle);
		uartTransfer(&Uart7Handle);
		uartTransfer(&Uart4Handle);
#endif
    }



//	static uint32_t tem_mscont = 0;

//	if( (msCount - tem_mscont) >=  GAIN_DATA_TIMEOUT)
//	{
//	  printf("%d\n\r",msCount - tem_mscont);
//	  halUartRecieveDma(&Uart6Handle);
//	  tem_mscont = msCount;
//	}
}
/******************************************************************************/
/*Function : HAL_StatusTypeDef uartTransfer(UART_HandleTypeDef *huart)*/
/*Brief    : Tranfer the data recieved from the network,FIFO through*/
/*Parameter: None*/
/*retval   : None*/
/******************************************************************************/
HAL_StatusTypeDef uartTransfer(UART_HandleTypeDef *huart){
	char *pdata;
	uint8_t  serialNo = 0xff;
    uint32_t length;
	uint32_t fifo_len;
    uint32_t retlen = 0; //the return value ,indicate the filled data length
	uint8_t  tem_buffer[2920];
	uint32_t test_time;
	uint8_t  loop;
	
	if(USART6 == huart->Instance) {
        serialNo = 0;
    } else if(UART5 == huart->Instance) {
        serialNo = 1;
    } else if(UART7 == huart->Instance) {
        serialNo = 2;
    } else if(UART4 == huart->Instance) {
        serialNo = 3;
    } else {
        return HAL_ERROR;
    }
/*Check the specific FIFO's status,and get the data from the network node*/
	for(loop =0 ;loop<1;loop++){
		if( CheckCanWriteNum(huart->pfifo) >= NODE_MAX_NUM ){
	#ifdef MY_DEBUG_P
			//printf("Can read number\n\r");
			pdata = getServerSerialData(serialNo,&length,retlen,COM_DATA_SIZE*COM_DATA_PACK_NUM);
			if(pdata ==NULL){
				free(pdata);
			  //vethFree(pdata);
			  //printf("pdata Null\n\r");
			  //return HAL_ERROR;
			}else{
			  //printf("The node address=%x\n\r",(uint32_t)pdata);
			  huart->test_total_get_length += length;
			  serial_server_queue_total[serialNo] -= length;
			  //printf("Serial%dAAAAAtest_total_get_length=%d\n\r",serialNo+1,huart->test_total_get_length);
			  //printf("Serial %d server queue length is %d\n\r",serialNo + 1,serial_server_queue_total[serialNo]);
			  memcpy(tem_buffer,pdata,length);
			  //vethFree(pdata);
			  free(pdata);
			  //printf("read server queue free memory sucessful\n\r");
			  //printf("after read serial %d server queue usage memory\n\r",serialNo + 1);
			 // mallco_dev.perused();
			  if(WriteFIFO(huart->pfifo,tem_buffer,length)){
				huart->test_before += length;
				//printf("FIFO_before =%d\n\r",huart->test_before);
			  }else{
				printf("tranfer FIFO overlap\n\r");
				huart->test_fifo_full++;
			  }
			}
	#else
			if( test_FIFO_before < SERIAL_TOTAL_LENGTH ){
			  length = 2920;
			  memcpy(tem_buffer,test_String,length);
			  if(WriteFIFO(huart->pfifo,tem_buffer,length)){
	//			test_total_FIFO_data += length;
	//			printf("FIFO data= %d\n\r",test_total_FIFO_data);
				test_FIFO_before += length;
				printf("test_FIFO_before =%d\n\r",test_FIFO_before);
				//printf("Add FIFO\n\r");
			  }
			}
	#endif
		}
	}
/*Check the the current data length in the FIFO and judge if a double mode enabled or a single one*/
	test_time = msCount;
#ifdef MY_DEBUG_P
	fifo_len = CheckCanReadNum(huart->pfifo);
	//printf("TTTTTTTTTTTTTTTTTTT=%d\n\r",msCount-test_time);
	if( fifo_len >= DOUBLE_BUFFER_LENGTH){/*The data are continous that need to use double buffer mode to send*/
	  //printf("canread=%d\n\r",fifo_len);
	  huart->count_targ = 0;
	  if(huart->comDmaStatus->single_finish_targ == 1){
		//printf("Oh my god\n\r");
		halUartTransmitDma(huart);
	  }
	}else if(fifo_len > 0){
#ifdef MY_DEBUG_P
	  //printf("A test\n\r");
	  if(huart->count_targ == 0){
		huart->count_targ = msCount;
	  }else{
	    if((msCount - huart->count_targ > LITTLE_DATA_TIMEOUT)&&(huart->comDmaStatus->double_finish_targ)){
		  /*Here need to add the transfer for liitle block of data*/
		  huart->count_targ = 0;
		  if(HAL_OK == halUartTransmitDmaLittle(huart) ){
			//printf("%d__little block enabled\n\r",serialNo+1);
		  }
		}
	  }
#endif
	}
#else
	if(HAL_OK == halUartTransmitDmaLittle(huart) ){
		printf("serial%d_test_String_len=%d\n\r",serialNo+1,test_string_len);
	}
#endif
	return HAL_OK;
}

/******************************************************************************/
/*Function : fillinComBuffer(uint8_t* pBuffer)*/
/*Brief    : get the data recieved through network*/
/*Parameter: uint8_t pointer*/
/*retval   : uint32_t: indicates the data length that fill in the comBuffer*/
/******************************************************************************/
uint32_t fillinComBuffer(uint8_t *pBuffer,uint8_t comNo) {
    char*    pdata = NULL;
    uint8_t  loop;
	uint8_t  i;
    uint32_t length;
    uint32_t retlen = 0; //the return value ,indicate the filled data length
	static uint32_t tem_scount=0;
#ifdef MY_DEBUG_P
    memset(pBuffer,0,COM_DATA_SIZE*COM_DATA_PACK_NUM);
    while( retlen < COM_DATA_SIZE-2920 ) {
        pdata = getServerSerialData(comNo,&length,retlen,COM_DATA_SIZE*COM_DATA_PACK_NUM);
        if(pdata == NULL) {
            return retlen;
        }
        //printf("Serial %d length=%d\n\r",comNo+1,length);
		if(0 == comNo){
          serial_send_total_length_1 += length;
		  printf("Serial 1 Send total_len__%d\n\r",serial_send_total_length_1);
        }else if(1 == comNo){
		  serial_send_total_length_2 += length;
		  //printf("Serial 2 Send total_len__%d\n\r",serial_send_total_length_2);
		}else if(2 == comNo){
		  serial_send_total_length_3 += length;
		  //printf("Serial 3 Send total_len__%d\n\r",serial_send_total_length_3);
		}else{
		  serial_send_total_length_3 += length;
		  //printf("Serial 4 Send total_len__%d\n\r",serial_send_total_length_4);
		}
		if(sCount-tem_scount >= 600)
		{
		  printf("Serial 1 Send total_len__%d\n\r",serial_send_total_length_1);
		  printf("Serial 2 Send total_len__%d\n\r",serial_send_total_length_2);
		  printf("Serial 3 Send total_len__%d\n\r",serial_send_total_length_3);
		  printf("Serial 4 Send total_len__%d\n\r",serial_send_total_length_4);
		  tem_scount = sCount;
		}
		memcpy(pBuffer+retlen,pdata,length);
        retlen += length;
        free(pdata);
    }
#else
	if(test_total_length < SERIAL_TOTAL_LENGTH){
      memcpy(pBuffer,test_String,50);
	  retlen = 50;
	  test_total_length += retlen;
	  //printf("t_len=%d\n\r ",test_total_length);
    }else{
	  retlen = 0;
	}
	//retlen = 20;
#endif
    return retlen;
}

uint32_t addToComBuffer(uint8_t *pBuffer,uint8_t comNo,uint32_t retlen) {
    char*    pdata = NULL;
    uint8_t  loop;
	uint8_t  i;
    uint32_t length;
    //uint32_t retlen = 0; //the return value ,indicate the filled data length
	static uint32_t tem_scount=0;
    //memset(pBuffer,0,COM_DATA_SIZE*COM_DATA_PACK_NUM);
    while(retlen < COM_DATA_SIZE-2920){
        pdata = getServerSerialData(comNo,&length,retlen,COM_DATA_SIZE*COM_DATA_PACK_NUM);
        if(pdata == NULL) {
            return retlen;
        }
        //printf("Serial %d length=%d\n\r",comNo+1,length);
		if(0 == comNo){
          serial_send_total_length_1 += length;
		  //printf("Serial 1 Send total_len__%d\n\r",serial_send_total_length_1);
        }else if(1 == comNo){
		  serial_send_total_length_2 += length;
		  //printf("Serial 2 Send total_len__%d\n\r",serial_send_total_length_2);
		}else if(2 == comNo){
		  serial_send_total_length_3 += length;
		  //printf("Serial 3 Send total_len__%d\n\r",serial_send_total_length_3);
		}else{
		  serial_send_total_length_3 += length;
		  //printf("Serial 4 Send total_len__%d\n\r",serial_send_total_length_4);
		}
		if(sCount-tem_scount >= 600)
		{
		  printf("Serial 1 Send total_len__%d\n\r",serial_send_total_length_1);
		  printf("Serial 2 Send total_len__%d\n\r",serial_send_total_length_2);
		  printf("Serial 3 Send total_len__%d\n\r",serial_send_total_length_3);
		  printf("Serial 4 Send total_len__%d\n\r",serial_send_total_length_4);
		  tem_scount = sCount;
		}
		memcpy(pBuffer+retlen,pdata,length);
        retlen += length;
        free(pdata);
    }
	return retlen;
  }
uint32_t fillinComBuffer1(UART_HandleTypeDef *huart,FIFO *pF,uint8_t *pBuffer) {
#ifdef MY_DEBUG_P
	uint16_t  i;
	if( 0 == ReadFIFO(pF,pBuffer,DOUBLE_BUFFER_LENGTH) ){
	  printf("The read fifo operation failed\n\r");
	  return 0;
	}else{
	  //test_FIFO_after += DOUBLE_BUFFER_LENGTH;
	  //printf("test_FIFO_after= %d\n\r",test_FIFO_after);
		huart->test_chk_byte = pBuffer[0];
//		for(i=0;i<DOUBLE_BUFFER_LENGTH;i++){
//			if(pBuffer[i] != huart->test_chk_byte){
//			  huart->test_error_transfer++;
//			}
//			if(huart->test_chk_byte == 255){
//			  huart->test_chk_byte=0;
//			}else{
//			  huart->test_chk_byte++;
//			}
//		}
	  return DOUBLE_BUFFER_LENGTH;
	}
#else

#endif
}

uint32_t fillinComBuffer2(UART_HandleTypeDef *huart,FIFO *pF,uint8_t *pBuffer) {
#ifdef MY_DEBUG_P
	uint32_t length;
	uint16_t i;
	length = CheckCanReadNum(pF);
	if( 0 == ReadFIFO(pF,pBuffer,length) ){
	  printf("The read fifo operation failed\n\r");
	  return 0;
	}else{
	  if (0 != CheckCanReadNum(pF)){
	    printf("After get the little block of data,still exit data\n\r");
	  }
//		huart->test_chk_byte = pBuffer[0];
//		for(i=0;i<length;i++){
//			if(pBuffer[i] != huart->test_chk_byte){
//				huart->test_error_transfer++;
//			}
//			if(huart->test_chk_byte == 255){
//				huart->test_chk_byte=0;
//			}else{
//				huart->test_chk_byte++;
//			}
//		}
	  //printf("Little block=%d\n\r",length);
	  return length; 
	}
#else
	if(test_string_len < TEST_TRANSFER_LEN){
	  memcpy(pBuffer,test_String,500);
	  test_string_len += 500;
	  return 500;
	}else{
	  return 0;
	}
#endif
}
#ifdef MY_DEBUG
/******************************************************************************/
/*Function : HAL_StatusTypeDef halUartTransmitDma(UART_HandleTypeDef *huart)*/
/*Brief    : Transmit the data through UART by the DMA double buffer mode*/
/*Parameter: None*/
/*retval   : None*/
/******************************************************************************/
HAL_StatusTypeDef halUartTransmitDma(UART_HandleTypeDef *huart) {
    uint32_t *tmp;
    uint32_t *tmpa;
    uint32_t tem_count;
    static uint32_t print_time = 0;
    uint8_t tmpx = 0;
    DMA_HandleTypeDef *hdma =huart->hdmatx;
//  uint8_t current;
//  char     *pdata = NULL;
//  uint32_t length = 0;
    uint8_t  serialNo = 0xff;

    if(USART6 == huart->Instance) {
        serialNo = 0;
    } else if(UART5 == huart->Instance) {
        serialNo = 1;
    } else if(UART7 == huart->Instance) {
        serialNo = 2;
    } else if(UART4 == huart->Instance) {
        serialNo = 3;
    } else {
        return HAL_ERROR;
    }
    /*Get the current memory status of mem0 and mem1 for using double buffer mode*/
    if( (huart->hdmatx->Instance->CR & DMA_SxCR_CT) != DMA_SxCR_CT) {
        /*Check the DMA status*/
        if((huart->State == HAL_UART_STATE_READY) || (huart->State == HAL_UART_STATE_BUSY_RX)) {
            huart->comDmaStatus->mem0_length = fillinComBuffer(huart->comDmaStatus->pbuffer1,serialNo);
            if(MEM_EMPTY == huart->comDmaStatus->mem0_length) {
                return HAL_ERROR;
            }
			tem_count = msCount;
            __HAL_LOCK(huart);
            //__HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
            huart->pTxBuffPtr  = huart->comDmaStatus->pbuffer1;
            huart->TxXferSize  = huart->comDmaStatus->mem0_length;
            huart->TxXferCount = huart->comDmaStatus->mem0_length;
            //printf("size: %d\n\r",huart->TxXferSize);
            huart->ErrorCode = HAL_UART_ERROR_NONE;
            /* Check if a receive process is ongoing or not */
            if(huart->State == HAL_UART_STATE_BUSY_RX) {
                huart->State = HAL_UART_STATE_BUSY_TX_RX;
            } else {
                huart->State = HAL_UART_STATE_BUSY_TX;
            }
            /* Set the UART DMA transfer complete callback */
            huart->hdmatx->XferCpltCallback = uartDmaXferCpltCallback;
            /* Set the UART DMA Half transfer complete callback */
            huart->hdmatx->XferHalfCpltCallback = NULL;//UART_DMATxHalfCplt;
            /* Set the DMA error callback */
            huart->hdmatx->XferErrorCallback = NULL;//UART_DMAError;
            /* Set the UART DMA transfer complete callback*/
            huart->hdmatx->XferM1CpltCallback = uartDmaXferM1CpltCallback;
            /* Enable the UART transmit DMA Stream */
            //printf("ready0:%d\n\r",huart->TxXferSize);
            tmp = (uint32_t*)huart->comDmaStatus->pbuffer1;
            tmpa = (uint32_t*)huart->comDmaStatus->pbuffer2;
            //printf("pbuffer1:%x\n\r",(uint32_t)huart->comDmaStatus->pbuffer1);
            HAL_DMAEx_MultiBufferStart_IT(huart->hdmatx,(uint32_t)tmp,(uint32_t)&huart->Instance->DR,(uint32_t)tmpa,huart->TxXferSize);
            if(huart->TxXferSize <20){printf("JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ");}
			huart->hdmatx->Instance->CR &= (uint32_t)0xfffffffe;
            huart->hdmatx->Instance->NDTR = huart->comDmaStatus->mem0_length;
			if(huart->hdmatx->Instance->NDTR <20 ){printf("KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK");}
            huart->hdmatx->Instance->CR |= (uint32_t)0x0001;            
            //printf("DMA_CR:%x\n\r",huart->hdmatx->Instance->CR);
            /* Enable the DMA transfer for transmit request by setting the DMAT bit in the UART CR3 register */
            huart->Instance->CR3 |= USART_CR3_DMAT;
			//printf("t_len=%d\n\r ",test_total_length);
            //printf("test_ms=%d\n\r",msCount-tem_count);
			/* Process Unlocked */
            __HAL_UNLOCK(huart);
            return HAL_OK;
        } else {
            if( 0 == huart->comDmaStatus->mem1_length) { //if Buffer2
                //printf("Ts_len=%d\n\r ",test_total_length);
				huart->comDmaStatus->mem1_length = fillinComBuffer(huart->comDmaStatus->pbuffer2,serialNo);
			}else{
                huart->comDmaStatus->mem1_length = addToComBuffer(huart->comDmaStatus->pbuffer2,serialNo,huart->comDmaStatus->mem1_length);
			}
				if( 0 != huart->comDmaStatus->mem1_length ) {};
                //printf("ready_1\n\r");
            }
        }
    } else { //huart->hdmatx->Instance->CR & DMA_SxCR_CT) == MEM_1
        /*Check the DMA status*/
        if((huart->State == HAL_UART_STATE_READY) || (huart->State == HAL_UART_STATE_BUSY_RX)) {
            //printf("DMA_>CR:%x\n\r",huart->hdmatx->Instance->CR);
            huart->comDmaStatus->mem1_length = fillinComBuffer(huart->comDmaStatus->pbuffer2,serialNo);
            if(MEM_EMPTY == huart->comDmaStatus->mem1_length) {
                return HAL_ERROR;
            }
			tem_count = msCount;
//	  huart->hdmatx->Instance->CR &= (uint32_t)0xfffffffe;
//	  //printf("DMA_CR:%x\n\r",huart->hdmatx->Instance->CR);
//	  huart->hdmatx->Instance->CR &= ~(uint32_t)0x00080000;
//	  huart->hdmatx->Instance->CR &= (uint32_t)0x00000001;
//	  printf("  1_GGGGGGGet successful :");
//	  printf("  %s\n\r",huart->comDmaStatus->pbuffer2);
            //printf("The hal uart state is ready\n\r");
            __HAL_LOCK(huart);
            //__HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
            huart->pTxBuffPtr  = huart->comDmaStatus->pbuffer2;
            huart->TxXferSize  = huart->comDmaStatus->mem1_length;
            huart->TxXferCount = huart->comDmaStatus->mem1_length;
            huart->ErrorCode = HAL_UART_ERROR_NONE;
            /* Check if a receive process is ongoing or not */
            if(huart->State == HAL_UART_STATE_BUSY_RX) {
                huart->State = HAL_UART_STATE_BUSY_TX_RX;
            } else {
                huart->State = HAL_UART_STATE_BUSY_TX;
            }
            /* Set the UART DMA transfer complete callback */
            huart->hdmatx->XferCpltCallback = uartDmaXferCpltCallback;
            /* Set the UART DMA Half transfer complete callback */
            huart->hdmatx->XferHalfCpltCallback = NULL;//UART_DMATxHalfCplt;
            /* Set the DMA error callback */
            huart->hdmatx->XferErrorCallback = NULL;//UART_DMAError;
            /* Set the UART DMA transfer complete callback*/
            huart->hdmatx->XferM1CpltCallback = uartDmaXferM1CpltCallback;
            //printf("The callback function%x\n\r",(uint32_t)&uartDmaXferM1CpltCallback);
            /* Enable the UART transmit DMA Stream */
            //printf("ready2:%d\n\r",huart->TxXferSize);
            tmp = (uint32_t*)huart->comDmaStatus->pbuffer2;
            tmpa = (uint32_t*)huart->comDmaStatus->pbuffer1;
            //printf("pbuffer1:%x\n\r",(uint32_t)huart->comDmaStatus->pbuffer1);
//	  tem_count = msCount;
//	  while( msCount - tem_count <10){};
            HAL_DMAEx_MultiBufferStart_IT(huart->hdmatx,(uint32_t)tmpa,(uint32_t)&huart->Instance->DR,(uint32_t)tmp,huart->TxXferSize);
            if(huart->TxXferSize ==1){printf("JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ");}
			huart->hdmatx->Instance->CR &= (uint32_t)0xfffffffe;
            huart->hdmatx->Instance->NDTR = huart->comDmaStatus->mem1_length;
			if(huart->hdmatx->Instance->NDTR == 0x00001){printf("KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK");}
            huart->hdmatx->Instance->CR |= (uint32_t)0x0001;
//	  tem_count = msCount;
//	  while( msCount - tem_count < 10){};
#ifdef MY_DEBUG
            printf("After:\n\r");
            printf("DMA_>CR:%x\n\r",huart->hdmatx->Instance->CR);
            printf("DMA_>FCR:%x\n\r",huart->hdmatx->Instance->FCR);
            printf("DMA_>M0AR:%x\n\r",huart->hdmatx->Instance->M0AR);
            printf("DMA_>M1AR:%x\n\r",huart->hdmatx->Instance->M1AR);
            printf("DMA_>NDTR:%x\n\r",huart->hdmatx->Instance->NDTR);
            printf("DMA_>PAR:%x\n\r",huart->hdmatx->Instance->PAR);
            printf("DMA2_LISR:%x\n\r",DMA2->LISR);
            printf("DMA2_HISR:%x\n\r",DMA2->HISR);
#endif
            /* Enable the DMA transfer for transmit request by setting the DMAT bit
             in the UART CR3 register */
            huart->Instance->CR3 |= USART_CR3_DMAT;
			//printf("test_ms=%d\n\r",msCount-tem_count);
            /* Process Unlocked */
            __HAL_UNLOCK(huart);
            return HAL_OK;
        } else {
            if( 0 == huart->comDmaStatus->mem0_length) { //if Buffer2
               // printf("Ts_len=%d\n\r ",test_total_length);
				huart->comDmaStatus->mem0_length = fillinComBuffer(huart->comDmaStatus->pbuffer1,serialNo);
            }else{
				huart->comDmaStatus->mem0_length = addToComBuffer(huart->comDmaStatus->pbuffer1,serialNo,huart->comDmaStatus->mem0_length);
			}
        }
    }
    return HAL_BUSY;
}
#else
/******************************************************************************/
/*Function : HAL_StatusTypeDef halUartTransmitDma(UART_HandleTypeDef *huart)*/
/*Brief    : Transmit the data through UART by the DMA double buffer mode*/
/*Parameter: None*/
/*retval   : None*/
/******************************************************************************/
HAL_StatusTypeDef halUartTransmitDma(UART_HandleTypeDef *huart) {
    uint32_t *tmp;
    uint32_t *tmpa;
    uint32_t tem_count;
    static uint32_t print_time = 0;
    uint8_t tmpx = 0;
    DMA_HandleTypeDef *hdma =huart->hdmatx;
//  uint8_t current;
//  char     *pdata = NULL;
//  uint32_t length = 0;
    uint8_t  serialNo = 0xff;

    if(USART6 == huart->Instance) {
        serialNo = 0;
    } else if(UART5 == huart->Instance) {
        serialNo = 1;
    } else if(UART7 == huart->Instance) {
        serialNo = 2;
    } else if(UART4 == huart->Instance) {
        serialNo = 3;
    } else {
        return HAL_ERROR;
    }
    if(sCount - print_time >= 10 ) {
#ifdef MY_DEBUG
	  printf("test_HaHa1 =%d\n\r",test_HaHa1);
	  printf("test_HaHa2 =%d\n\r",test_HaHa2);
	  printf("fifo_error_counter=%d\n\r",fifo_error_counter);
	  printf("transfer_error_counter=%d\n\r",transfer_error_counter);
	  printf("test_HaHa_else =%d \n\r",test_HaHa_else);
	  printf("test_HaHa_EN =%d \n\r",test_HaHa_EN);
	  printf("test_HaHa_TC =%d \n\r",test_HaHa_TC);
	  printf("test_HaHa_len=%d \n\r",test_HaHa_len);
	  printf("%d_test_error byte=%d\n\r",serialNo+1,huart->test_error_byte);
	  printf("test_rcv_len=%d\n\r",huart->test_rcv_len);
	  printf("pbuffer1:%x\n\r");
	  print_time = sCount;
#endif
    }
	if( (huart->hdmatx->Instance->CR & DMA_SxCR_CT) != DMA_SxCR_CT){
	/*Check the DMA status*/
	tmpx = huart->State;
	if((tmpx == HAL_UART_STATE_READY) || (tmpx == HAL_UART_STATE_BUSY_RX)) {
		//printf("Get MMMMMMMMMMMMM0\n\r");
		if((0 != huart->comDmaStatus->mem0_length) || ( 0!= huart->comDmaStatus->mem1_length) ){
		  huart->test_lose_block++;
		}
		if(0 == huart->comDmaStatus->mem0_length){
		  huart->comDmaStatus->mem0_length = fillinComBuffer1(huart,huart->pfifo,huart->comDmaStatus->pbuffer1);
		  huart->test_after += huart->comDmaStatus->mem0_length;
		} 
		if(MEM_EMPTY == huart->comDmaStatus->mem0_length) {
			return HAL_ERROR;
		}
//		}else{
//			
//			//printf("test_after=%d\n\r",huart->test_after);
//		}
		//printf("0AAAA\n\r");
		//printf("0\n\r");
		__HAL_LOCK(huart);
		huart->pTxBuffPtr  = huart->comDmaStatus->pbuffer1;
		huart->TxXferSize  = huart->comDmaStatus->mem0_length;
		huart->TxXferCount = huart->comDmaStatus->mem0_length;
		huart->ErrorCode = HAL_UART_ERROR_NONE;
		/* Check if a receive process is ongoing or not */
		if(huart->State == HAL_UART_STATE_BUSY_RX) {
				huart->State = HAL_UART_STATE_BUSY_TX_RX;
		} else {
				huart->State = HAL_UART_STATE_BUSY_TX;
		}
		/* Set the UART DMA transfer complete callback */
		huart->hdmatx->XferCpltCallback = uartDmaXferCpltCallback;
		/* Set the UART DMA Half transfer complete callback */
		huart->hdmatx->XferHalfCpltCallback = NULL;//UART_DMATxHalfCplt;
		/* Set the DMA error callback */
		huart->hdmatx->XferErrorCallback = NULL;//UART_DMAError;
		/* Set the UART DMA transfer complete callback*/
		huart->hdmatx->XferM1CpltCallback = uartDmaXferM1CpltCallback;
		/* Enable the UART transmit DMA Stream */
		tmp = (uint32_t*)huart->comDmaStatus->pbuffer1;
		tmpa = (uint32_t*)huart->comDmaStatus->pbuffer2;
		//printf("start Buffer0\n\r");
		//printf("pbuffer1:%x\n\r",(uint32_t)huart->comDmaStatus->pbuffer1);
		huart->comDmaStatus->double_finish_targ = 0;
		if(0x00000001 != (hdma->Instance->CR & 0x00000001) ){
			//Delay(2);
			if( (serialNo>=0 || serialNo<=1) && board.serial_mode.com1_2_mode == 485 ){
			   setComPortDirect(serialNo);
			}else if((serialNo>=2 && serialNo<=3) && board.serial_mode.com3_4_mode == 485 ){
			   setComPortDirect(serialNo);
			}
		}else{
			printf("When change the direct,the transmit already exit\n\r");
		}
		//Delays(5);
		HAL_DMAEx_MultiBufferStart_IT(huart->hdmatx,(uint32_t)tmp,(uint32_t)&huart->Instance->DR,(uint32_t)tmpa,huart->TxXferSize);
//		HAL_DMA_Start_IT(huart->hdmatx,(uint32_t)tmp,(uint32_t)&huart->Instance->DR, uint32_t DataLength);
//			huart->hdmatx->Instance->CR &= (uint32_t)0xfffffffe;
//			huart->hdmatx->Instance->NDTR = huart->comDmaStatus->mem0_length;
//			huart->hdmatx->Instance->CR |= (uint32_t)0x0001;
#ifdef MY_DEBUG
            printf("After:\n\r");
            printf("DMA_>CR:%x\n\r",huart->hdmatx->Instance->CR);
            printf("DMA_>FCR:%x\n\r",huart->hdmatx->Instance->FCR);
            printf("DMA_>M0AR:%x\n\r",huart->hdmatx->Instance->M0AR);
            printf("DMA_>M1AR:%x\n\r",huart->hdmatx->Instance->M1AR);
            printf("DMA_>NDTR:%x\n\r",huart->hdmatx->Instance->NDTR);
            printf("DMA_>PAR:%x\n\r",huart->hdmatx->Instance->PAR);
            printf("DMA2_LISR:%x\n\r",DMA2->LISR);
            printf("DMA2_HISR:%x\n\r",DMA2->HISR);
#endif
            /* Enable the DMA transfer for transmit request by setting the DMAT bit in the UART CR3 register */
            huart->Instance->CR3 |= USART_CR3_DMAT;
            /* Process Unlocked */
            __HAL_UNLOCK(huart);
            return HAL_OK;
        } else {
            if( 0 == huart->comDmaStatus->mem1_length) { //if Buffer2
                huart->comDmaStatus->mem1_length = fillinComBuffer1(huart,huart->pfifo,huart->comDmaStatus->pbuffer2);
				if( 0 != huart->comDmaStatus->mem1_length ) {
					huart->test_after += huart->comDmaStatus->mem1_length;
					//printf("test_after=%d\n\r",huart->test_after);
				}
                //printf("ready_1\n\r");
            }
        }
    } else { //huart->hdmatx->Instance->CR & DMA_SxCR_CT) == MEM_1
        /*Check the DMA status*/
        tmpx = huart->State;
        if((tmpx == HAL_UART_STATE_READY) || (tmpx == HAL_UART_STATE_BUSY_RX)) {
            //printf("DMA_>CR:%x\n\r",huart->hdmatx->Instance->CR);
            //printf("Get MMMMMMMMMMMMM1\n\r");
			if((0 != huart->comDmaStatus->mem0_length) || ( 0!= huart->comDmaStatus->mem1_length) ){
			   huart->test_lose_block++;
			}
			if(0 != huart->comDmaStatus->mem1_length){
			  huart->comDmaStatus->mem0_length = huart->comDmaStatus->mem1_length;
			  memcpy(huart->comDmaStatus->pbuffer1,huart->comDmaStatus->pbuffer2,huart->comDmaStatus->mem0_length);
			  huart->comDmaStatus->mem1_length=0;
			}else if( 0 !=  huart->comDmaStatus->mem0_length){
			}else{
			  huart->comDmaStatus->mem0_length = fillinComBuffer1(huart,huart->pfifo,huart->comDmaStatus->pbuffer1);
			  if(MEM_EMPTY == huart->comDmaStatus->mem0_length) {
				return HAL_ERROR;
			  }else{
				huart->test_after += huart->comDmaStatus->mem0_length;
				//printf("test_after=%d\n\r",huart->test_after);
			  }
			}

#ifdef MY_DEBUG
            printf("Before:\n\r");
            printf("DMA_>CR:%x\n\r",huart->hdmatx->Instance->CR);
            printf("DMA_>FCR:%x\n\r",huart->hdmatx->Instance->FCR);
            printf("DMA_>M0AR:%x\n\r",huart->hdmatx->Instance->M0AR);
            printf("DMA_>M1AR:%x\n\r",huart->hdmatx->Instance->M1AR);
            printf("DMA_>NDTR:%x\n\r",huart->hdmatx->Instance->NDTR);
            printf("DMA_>PAR:%x\n\r",huart->hdmatx->Instance->PAR);
            printf("DMA2_LISR:%x\n\r",DMA2->LISR);
            printf("DMA2_HISR:%x\n\r",DMA2->HISR);
#endif
//	  huart->hdmatx->Instance->CR &= (uint32_t)0xfffffffe;
//	  //printf("DMA_CR:%x\n\r",huart->hdmatx->Instance->CR);
//	  huart->hdmatx->Instance->CR &= ~(uint32_t)0x00080000;
//	  huart->hdmatx->Instance->CR &= (uint32_t)0x00000001;
//	  printf("  1_GGGGGGGet successful :");
//	  printf("  %s\n\r",huart->comDmaStatus->pbuffer2);
			//printf("1AAAAAA\n\r");
            //printf("1\n\r");
			//printf("The hal uart state is ready\n\r");
            __HAL_LOCK(huart);
            //__HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
            huart->pTxBuffPtr  = huart->comDmaStatus->pbuffer2;
            huart->TxXferSize  = huart->comDmaStatus->mem0_length;
            huart->TxXferCount = huart->comDmaStatus->mem0_length;
            huart->ErrorCode = HAL_UART_ERROR_NONE;
            /* Check if a receive process is ongoing or not */
            if(huart->State == HAL_UART_STATE_BUSY_RX) {
                huart->State = HAL_UART_STATE_BUSY_TX_RX;
            } else {
                huart->State = HAL_UART_STATE_BUSY_TX;
            }
            /* Set the UART DMA transfer complete callback */
            huart->hdmatx->XferCpltCallback = uartDmaXferCpltCallback;
            /* Set the UART DMA Half transfer complete callback */
            huart->hdmatx->XferHalfCpltCallback = NULL;//UART_DMATxHalfCplt;
            /* Set the DMA error callback */
            huart->hdmatx->XferErrorCallback = NULL;//UART_DMAError;
            /* Set the UART DMA transfer complete callback*/
            huart->hdmatx->XferM1CpltCallback = uartDmaXferM1CpltCallback;
            //printf("The callback function%x\n\r",(uint32_t)&uartDmaXferM1CpltCallback);
            /* Enable the UART transmit DMA Stream */
            //printf("ready2:%d\n\r",huart->TxXferSize);
            tmp = (uint32_t*)huart->comDmaStatus->pbuffer2;
            tmpa = (uint32_t*)huart->comDmaStatus->pbuffer1;
            //printf("pbuffer1:%x\n\r",(uint32_t)huart->comDmaStatus->pbuffer1);
//	  tem_count = msCount;
//	  while( msCount - tem_count <10){};
			//printf("start Buffer1\n\r");
			if( DMA_SxCR_CT == ( huart->hdmatx->Instance->CR & DMA_SxCR_CT) ){
			  huart->hdmatx->Instance->CR &= ~DMA_SxCR_CT;
			}
			huart->comDmaStatus->double_finish_targ = 0;
			if(0x00000001 != (hdma->Instance->CR & 0x00000001) ){
				//Delay(2);
				if( (serialNo>=0 || serialNo<=1) && board.serial_mode.com1_2_mode == 485 ){
					setComPortDirect(serialNo);
				}else if((serialNo>=2 && serialNo<=3) && board.serial_mode.com3_4_mode == 485 ){
					setComPortDirect(serialNo);
				}
			}else{
				printf("When change the direct,the transmit already exit\n\r");
			}
			//Delays(5);
            HAL_DMAEx_MultiBufferStart_IT(huart->hdmatx,(uint32_t)tmpa,(uint32_t)&huart->Instance->DR,(uint32_t)tmp,huart->TxXferSize);
            //if(huart->TxXferSize <20){printf("JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ\n\r");}
//			huart->hdmatx->Instance->CR &= (uint32_t)0xfffffffe;
//			huart->hdmatx->Instance->NDTR = huart->comDmaStatus->mem1_length;
//			huart->hdmatx->Instance->CR |= (uint32_t)0x0001;
#ifdef MY_DEBUG
            printf("After:\n\r");
            printf("DMA_>CR:%x\n\r",huart->hdmatx->Instance->CR);
//            printf("DMA_>FCR:%x\n\r",huart->hdmatx->Instance->FCR);
//            printf("DMA_>M0AR:%x\n\r",huart->hdmatx->Instance->M0AR);
//            printf("DMA_>M1AR:%x\n\r",huart->hdmatx->Instance->M1AR);
//            printf("DMA_>NDTR:%x\n\r",huart->hdmatx->Instance->NDTR);
//            printf("DMA_>PAR:%x\n\r",huart->hdmatx->Instance->PAR);
//            printf("DMA2_LISR:%x\n\r",DMA2->LISR);
//            printf("DMA2_HISR:%x\n\r",DMA2->HISR);
#endif
            /* Enable the DMA transfer for transmit request by setting the DMAT bit
             in the UART CR3 register */
            huart->Instance->CR3 |= USART_CR3_DMAT;
            /* Process Unlocked */
            __HAL_UNLOCK(huart);
            return HAL_OK;
        } else {
            if( 0 == huart->comDmaStatus->mem0_length) { //if Buffer2
                huart->comDmaStatus->mem0_length = fillinComBuffer1(huart,huart->pfifo,huart->comDmaStatus->pbuffer1);
				if(MEM_EMPTY != huart->comDmaStatus->mem0_length){
					huart->test_after += huart->comDmaStatus->mem0_length;
					//printf("test_after=%d\n\r",huart->test_after);
				}
//		  printf("ready3\n\r");
            }
        }
    }
    return HAL_BUSY;
}
#endif
/******************************************************************************/
/*Function : HAL_StatusTypeDef halUartTransmitDmaLittle(UART_HandleTypeDef *huart)*/
/*Brief    : Transmit the data through UART by the DMA double buffer mode*/
/*Parameter: None*/
/*retval   : None*/
/******************************************************************************/
HAL_StatusTypeDef halUartTransmitDmaLittle(UART_HandleTypeDef *huart){

	uint32_t *tmp;
	uint8_t  serialNo;
	
	serialNo = (huart->Instance == USART6)? 0:
	           (huart->Instance == UART5 )? 1:
			   (huart->Instance == UART7 )? 2:
			   (huart->Instance == UART4 )? 3:
			   0xff;                
	if((huart->State == HAL_UART_STATE_READY) || (huart->State == HAL_UART_STATE_BUSY_RX)) {
		huart->comDmaStatus->mem_length = fillinComBuffer2(huart,huart->pfifo,huart->comDmaStatus->pbuffer);
		if(MEM_EMPTY == huart->comDmaStatus->mem_length) {
		  return HAL_ERROR;
		}else{
		  huart->test_after += huart->comDmaStatus->mem_length;
		}
//	printf("mem0_length =%d\n\r",huart->comDmaStatus->mem0_length);
//	printf("mem1_length =%d\n\r",huart->comDmaStatus->mem1_length);
#ifdef MY_DEBUG
	printf("Before:\n\r");
	printf("DMA_>CR:%x\n\r",huart->hdmatx->Instance->CR);
	printf("DMA_>FCR:%x\n\r",huart->hdmatx->Instance->FCR);
	printf("DMA_>M0AR:%x\n\r",huart->hdmatx->Instance->M0AR);
	printf("DMA_>M1AR:%x\n\r",huart->hdmatx->Instance->M1AR);
	printf("DMA_>NDTR:%x\n\r",huart->hdmatx->Instance->NDTR);
	printf("DMA_>PAR:%x\n\r",huart->hdmatx->Instance->PAR);
	printf("DMA2_LISR:%x\n\r",DMA2->LISR);
	printf("DMA2_HISR:%x\n\r",DMA2->HISR);
#endif
	__HAL_LOCK(huart);
	//__HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
	huart->pTxBuffPtr  = huart->comDmaStatus->pbuffer;
	huart->TxXferSize  = huart->comDmaStatus->mem_length;
	huart->TxXferCount = huart->comDmaStatus->mem_length;
	huart->ErrorCode = HAL_UART_ERROR_NONE;
	/* Check if a receive process is ongoing or not */
	if(huart->State == HAL_UART_STATE_BUSY_RX) {
	  huart->State = HAL_UART_STATE_BUSY_TX_RX;
	} else {
	  huart->State = HAL_UART_STATE_BUSY_TX;
	}
	/* Set the UART DMA transfer complete callback */
	huart->hdmatx->XferDmaCallback = DmaXferCpltCallback;
	/* Set the UART DMA Half transfer complete callback */
	huart->hdmatx->XferHalfCpltCallback = NULL;//UART_DMATxHalfCplt;
	/* Set the DMA error callback */
	huart->hdmatx->XferErrorCallback = NULL;//UART_DMAError;
	/* Set the UART DMA transfer complete callback*/
	huart->hdmatx->XferM1CpltCallback = NULL;//uartDmaXferM1CpltCallback;
	//printf("The callback function%x\n\r",(uint32_t)&uartDmaXferM1CpltCallback);
	/* Enable the UART transmit DMA Stream */
	//printf("ready2:%d\n\r",huart->TxXferSize);
	tmp = (uint32_t*)huart->comDmaStatus->pbuffer;
	//tmpa = (uint32_t*)huart->comDmaStatus->pbuffer1;
	//printf("pbuffer1:%x\n\r",(uint32_t)huart->comDmaStatus->pbuffer1);
//	  tem_count = msCount;
//	  while( msCount - tem_count <10){};
	//HAL_DMAEx_MultiBufferStart_IT(huart->hdmatx,(uint32_t)tmpa,(uint32_t)&huart->Instance->DR,(uint32_t)tmp,huart->TxXferSize);
	if (0 != CheckCanReadNum(huart->pfifo)){
		printf("After get the little block of data,still exit data\n\r");
	}
	huart->comDmaStatus->single_finish_targ = 0;
	if(0x00000001 != (huart->hdmatx->Instance->CR & 0x00000001) ){
		//Delay(2);
		if( (serialNo>=0 || serialNo<=1) && board.serial_mode.com1_2_mode == 485 ){
			setComPortDirect(serialNo);
		}else if((serialNo>=2 && serialNo<=3) && board.serial_mode.com3_4_mode == 485 ){
			setComPortDirect(serialNo);
		}else{
			printf("SerialNo error\n\r");
		}
	}else{
		printf("When change the direct,the transmit already exit\n\r");
	}
	//Delays(5);
	HAL_DMA_Start_IT(huart->hdmatx,(uint32_t)tmp,(uint32_t)&huart->Instance->DR,huart->comDmaStatus->mem_length);
	//if(huart->TxXferSize <20){printf("JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ\n\r");}
//	huart->hdmatx->Instance->CR &= (uint32_t)0xfffffffe;
//	huart->hdmatx->Instance->NDTR = huart->comDmaStatus->mem1_length;
//	huart->hdmatx->Instance->CR |= (uint32_t)0x0001;
#ifdef MY_DEBUG
            printf("After:\n\r");
            printf("DMA_>CR:%x\n\r",huart->hdmatx->Instance->CR);
            printf("DMA_>FCR:%x\n\r",huart->hdmatx->Instance->FCR);
            printf("DMA_>M0AR:%x\n\r",huart->hdmatx->Instance->M0AR);
            printf("DMA_>M1AR:%x\n\r",huart->hdmatx->Instance->M1AR);
            printf("DMA_>NDTR:%x\n\r",huart->hdmatx->Instance->NDTR);
            printf("DMA_>PAR:%x\n\r",huart->hdmatx->Instance->PAR);
            printf("DMA2_LISR:%x\n\r",DMA2->LISR);
            printf("DMA2_HISR:%x\n\r",DMA2->HISR);
#endif
	/* Enable the DMA transfer for transmit request by setting the DMAT bit
	in the UART CR3 register */
	huart->Instance->CR3 |= USART_CR3_DMAT;
	/* Process Unlocked */
	__HAL_UNLOCK(huart);
	return HAL_OK;
  }else{
	return HAL_ERROR;
  }
}
/******************************************************************************/
/*Function : UartXferM1CpltCallback(DMA_HandleTypeDef *hdma)*/
/*Brief    : Uart transfer callback function for Memory 1*/
/*Parameter: hdma point to the corresponding DMA Handle*/
/*retval   : None*/
/******************************************************************************/
void uartDmaXferM1CpltCallback(DMA_HandleTypeDef *hdma) {
    UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
	uint8_t serialNo;
	serialNo = (huart->Instance == USART6)? 0:
			   (huart->Instance == UART5 )? 1:
			   (huart->Instance == UART7 )? 2:
			   (huart->Instance == UART4 )? 3:
			   0xff; 
//	_UART_WAIT_TC_FLAG__(huart);
//	huart->Instance->SR |= USART_SR_TC;
	/* Clear the transfer complete flag */
	//__HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
//	/*Check if the EN bit still enabled*/
//	if( 0x00000000 != (hdma->Instance->CR &0x00000001) )
//	{
//	  test_HaHa_EN++;
//	  //while(0x00000000 != (hdma->Instance->CR &0x00000001)){}
//	}
//	if(hdma->Instance->NDTR != 0 )
//	{
//	  test_HaHa_len = hdma->Instance->NDTR;
//	}
#ifdef MY_DEBUG_P
    /*At first clear the corresponding targ for specific buffer*/
    huart->comDmaStatus->mem1_length = MEM_EMPTY;
    /*Check if the memory 0 has valid data to be tranfered*/
    if ( MEM_EMPTY == huart->comDmaStatus->mem0_length ) {
        /*No valid data in memory 0,stop the DMA request,and set the huart state ready or RX*/
		/* Disable the DMA transfer for transmit request by setting the DMAT bit
        in the UART CR3 register */  
		__HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
		huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAT);
//		i=1000;
//		while(i--){}
		hdma->Instance->CR &= (uint32_t)0xfffffffe;
        /* Clear the transfer complete flag */
        __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
        //while(USART_SR_TC != (huart->Instance->SR & USART_SR_TC)){};
		/* Check if a receive process is ongoing or not */
        if(huart->State == HAL_UART_STATE_BUSY_TX_RX) {
            huart->State = HAL_UART_STATE_BUSY_RX;
        } else {
            huart->State = HAL_UART_STATE_READY;
        }
        //hdma->Instance->CR &= (uint32_t)0xfffffffe;
		//while(USART_SR_TC != (huart->Instance->SR & USART_SR_TC)){}
		//if( (huart->Instance->SR & USART_SR_TC) )
		huart->Instance->SR &= ~USART_SR_TC;
		the_int_wait=5000;
		while(the_int_wait--){};
		if( (serialNo>=0 || serialNo<=1) && board.serial_mode.com1_2_mode == 485 ){
		   resetComPortDirect(serialNo);
		}else if((serialNo>=2 && serialNo<=3) && board.serial_mode.com3_4_mode == 485 ){
		   resetComPortDirect(serialNo);
		}
		huart->comDmaStatus->double_finish_targ = 1;
    } else { /*Memory0 has valid data to be transferred*/
//        /* Clear the transfer complete flag */
//        __HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
//        //huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAT);
//        hdma->Instance->CR &= (uint32_t)0xfffffffe;
//        hdma->Instance->NDTR = huart->comDmaStatus->mem0_length;
		if(hdma->Instance->NDTR <=10){
		  test_HaHa1++;
		}
		__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
		//huart->Instance->SR &= ~USART_SR_TC;
    }
#else
    /* Clear the transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
    printf("mem1\n\r");
#endif
}
/******************************************************************************/
/*Function : UartXferCpltCallback(DMA_HandleTypeDef *hdma)*/
/*Brief    : Uart transfer callback function for Memory 0*/
/*Parameter: hdma point to the corresponding DMA Handle*/
/*retval   : None*/
/******************************************************************************/
void uartDmaXferCpltCallback(DMA_HandleTypeDef *hdma) {
    UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
/*Wait for the UART transmit complete targ set,then clear it*/
//	if( 0x00000000 != (hdma->Instance->CR &0x00000001) )
//	{
//	  test_HaHa_EN++;
//	  //while(0x00000000 != (hdma->Instance->CR &0x00000001)){}
//	}
//	if( USART_SR_TC != (USART_SR_TC  & huart->Instance->SR) )
//	{
//	  test_HaHa_TC++;
//	}
//	if(hdma->Instance->NDTR != 0 )
//	{
//	  //test_HaHa_len++;
//	  test_HaHa_len = hdma->Instance->NDTR;
//	}
	uint8_t serialNo;
	serialNo = (huart->Instance == USART6)? 0:
			   (huart->Instance == UART5 )? 1:
			   (huart->Instance == UART7 )? 2:
			   (huart->Instance == UART4 )? 3:
			   0xff; 
#ifdef MY_DEBUG_P
//	/*At first clear the corresponding targ for specific buffer*/
    huart->comDmaStatus->mem0_length = MEM_EMPTY;
    /*Check if the memory 0 has valid data to be tranfered*/
    if ( MEM_EMPTY == huart->comDmaStatus->mem1_length ) {
        /*No valid data in memory 0,stop the DMA request,and set the huart state ready or RX*/
		/* Disable the DMA transfer for transmit request by setting the DMAT bit
        in the UART CR3 register */
		__HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
//		i=1000;
//		while(i--){}
		huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAT);
		hdma->Instance->CR &= (uint32_t)0xfffffffe;
        /* Clear the transfer complete flag */
        __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
        //while(USART_SR_TC != (huart->Instance->SR & USART_SR_TC)){};
		/* Check if a receive process is ongoing or not */
        if(huart->State == HAL_UART_STATE_BUSY_TX_RX) {
            huart->State = HAL_UART_STATE_BUSY_RX;
        } else {
            huart->State = HAL_UART_STATE_READY;
        }
		//while(USART_SR_TC != (huart->Instance->SR & USART_SR_TC)){}
		huart->Instance->SR &= ~USART_SR_TC;
		the_int_wait=5000;
		while(the_int_wait--){};
		if( (serialNo>=0 || serialNo<=1) && board.serial_mode.com1_2_mode == 485 ){
		   resetComPortDirect(serialNo);
		}else if((serialNo>=2 && serialNo<=3) && board.serial_mode.com3_4_mode == 485 ){
		   resetComPortDirect(serialNo);
		}
//		the_int_wait=3000;
//		while(the_int_wait--){};
		huart->comDmaStatus->double_finish_targ = 1;
//	  }
    } else { /*Memory1 has valid data to be transferred*/
        /* Clear the transfer complete flag */
        //__HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
        //huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAT);
//        hdma->Instance->CR &= (uint32_t)0xfffffffe;
//        hdma->Instance->NDTR = huart->comDmaStatus->mem1_length;
		if(hdma->Instance->NDTR <=10){
		  test_HaHa2++;
		}
		//huart->Instance->SR &= ~USART_SR_TC;
        __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
//     hdma->Instance->CR |= (uint32_t)0x0001;
//	  /* Clear the transfer complete flag */
        /*Enable the UART DMA request again*/
        //huart->Instance->CR3 |= ((uint32_t)USART_CR3_DMAT);
    }
#else
    printf("mem0\n\r");
#endif
}

/******************************************************************************/
/*Function : DmaXferCpltCallback(DMA_HandleTypeDef *hdma)*/
/*Brief    : Uart transfer callback function for Single buffer mode*/
/*Parameter: hdma point to the corresponding DMA Handle*/
/*retval   : None*/
/******************************************************************************/
void DmaXferCpltCallback(DMA_HandleTypeDef *hdma){
	UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
//	i=1000;
//	while(i--){}
	uint8_t serialNo;
	serialNo = (huart->Instance == USART6)? 0:
			   (huart->Instance == UART5 )? 1:
			   (huart->Instance == UART7 )? 2:
			   (huart->Instance == UART4 )? 3:
			   0xff; 
	/* Disable the DMA transfer for transmit request by setting the DMAT bit
	in the UART CR3 register */
	__HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
	//__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
	huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAT);
	hdma->Instance->CR &= 0xfffffffe;
	__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
	//while(USART_SR_TC != (huart->Instance->SR & USART_SR_TC)){};
	/* Check if a receive process is ongoing or not */
	if(huart->State == HAL_UART_STATE_BUSY_TX_RX)
	{
	  huart->State = HAL_UART_STATE_BUSY_RX;
	}
	else
	{
	  huart->State = HAL_UART_STATE_READY;
	}
	the_int_wait=5000;
	while(the_int_wait--){};
	if( (serialNo>=0 || serialNo<=1) && board.serial_mode.com1_2_mode == 485 ){
		resetComPortDirect(serialNo);
	}else if((serialNo>=2 && serialNo<=3) && board.serial_mode.com3_4_mode == 485 ){
		resetComPortDirect(serialNo);
	}
	huart->comDmaStatus->single_finish_targ = 1;
	
	//while(USART_SR_TC != (huart->Instance->SR & USART_SR_TC)){}
}
/********************************************************************************************************/
/****************************Uart recieve functions for serialclient*************************************/
/********************************************************************************************************/
void serialClient() {
    static uint32_t tem_mscont = 0;

    if( (msCount - tem_mscont) >=  GAIN_DATA_TIMEOUT) {
        //printf("%d\n\r",msCount - tem_mscont);
        halUartRecieveDma(&Uart6Handle);
		getUartRecievedData(&Uart6Handle);
        halUartRecieveDma(&Uart5Handle);
		getUartRecievedData(&Uart5Handle);
        halUartRecieveDma(&Uart7Handle);
		getUartRecievedData(&Uart7Handle);
        halUartRecieveDma(&Uart4Handle);
		getUartRecievedData(&Uart4Handle);
        tem_mscont = msCount;
    }
}
/******************************************************************************/
/*Function : HAL_StatusTypeDef halUartRecieveDma(UART_HandleTypeDef *huart)*/
/*Brief    : The serialServer function for the highest while(1)*/
/*Parameter: */
/*retval   : None*/
/******************************************************************************/
HAL_StatusTypeDef halUartRecieveDma(UART_HandleTypeDef *huart) {
	uint32_t *tmp;
	uint32_t *tmpa;
	uint32_t tem_count;
	//static uint32_t print_time = 0;
	uint8_t tmpx = 0;
	  //uint8_t need_check = 0;
	DMA_HandleTypeDef *hdma =huart->hdmarx;
//  uint8_t current;
//  char     *pdata = NULL;
//  uint32_t length = 0;
	uint8_t  serialNo = 0xff;

	if(USART6 == huart->Instance) {
		serialNo = 0;
	} else if(UART5 == huart->Instance) {
		serialNo = 1;
	} else if(UART7 == huart->Instance) {
		serialNo = 2;
	} else if(UART4 == huart->Instance) {
        serialNo = 3;
    } else {
        return HAL_ERROR;
    }
    if(sCount - huart->test_print_time >= 100 ) {
#ifdef MY_DEBUG
	  printf("XXXXXXXXXXXXXXXXXXXXXXXX_________%d\n\r",serialNo+1);
	  //printf("test_HaHa1 =%d\n\r",test_HaHa1);
	  //printf("test_HaHa2 =%d\n\r",test_HaHa2);
	  printf("fifo_error_counter=%d\n\r",fifo_error_counter);
	  printf("transfer_error_counter=%d\n\r",transfer_error_counter);
	  //printf("test_HaHa_else =%d \n\r",huart->idle_occus);
	  //printf("poll_round_count =%d \n\r",huart->poll_round_count);	
	  //printf("test_HaHa_EN =%d \n\r",test_HaHa_EN);
	  //printf("test_HaHa_TC =%d \n\r",test_HaHa_TC);
	  //printf("test_HaHa_len=%d \n\r",test_HaHa_len);
	  //printf("%d_test_error byte=%d\n\r",serialNo+1,huart->test_error_byte);
	  //printf("%d_test_error_transfer=%d\n\r",serialNo+1,huart->test_error_transfer);
	  printf("test_rcv_len=%d\n\r",huart->test_rcv_len);
	  printf("test_rcv_before=%d\n\r",huart->test_rcv_before);
	  printf("test_before=%d\n\r",huart->test_before);
	  printf("test_after= %d\n\r",huart->test_after);
	  //printf("test_FIFO_full = %d\n\r",huart->test_fifo_full);
	  //printf("Rcv_pbuffer1:%x\n\r",(uint32_t)huart->uartRcvQueue->pinbuffer1);
	  //printf("Rcv_pbuffer2:%x\n\r",(uint32_t)huart->uartRcvQueue->pinbuffer2);
	  //printf("Transfer_pbuffer1:%x\n\r",(uint32_t)huart->comDmaStatus->pbuffer1);
	  //printf("Transfer_pbuffer2:%x\n\r",(uint32_t)huart->comDmaStatus->pbuffer2);
	  //printf("Transfer_pbuffer:%x\n\r",(uint32_t)huart->comDmaStatus->pbuffer);
	  //printf("test_transfer_cpt=%d\n\r",huart->test_transfer_cpt);
	  //printf("test_idle_dma_targ=%d\n\r",huart->test_idle_dma_targ);
	  //printf("test_the_error_byte=%d\n\r",huart->test_the_error_byte);
	  //printf("test_can_get_in_buffer=%d\n\r",huart->test_can_get_in_buffer);
	  printf("test_lose_block=%d",huart->test_lose_block);
	  huart->test_print_time = sCount;
#endif
    }
	if((huart->poll_round_count > 10) && (huart->idle_occus >= 1)){
	//if(0){
		__HAL_LOCK(huart);
		huart->poll_round_count = 0;
		huart->idle_occus = 0; 
		huart->uartRcvQueue->last_frame = 1; 
		if(0 != huart->hdmarx->Instance->NDTR){
			huart->hdmarx->Instance->CR &= UART_DMA_DISABLE;
//			while(1){
//				if( UART_DMA_ENABLE != (huart->hdmarx->Instance->CR & UART_DMA_ENABLE ) ){
//				break;
//				}
//			}	
		}
		__HAL_UNLOCK(huart);
	}else if (huart->poll_round_count >10){
		//huart->poll_round_count = 0;
		//huart->idle_occus = 1;          		
	}else{
	   huart->poll_round_count += 1;		
	} 
	//huart->poll_round_count += 1;
	if(MEM_EMPTY != huart->uartRcvQueue->buffer1_length) {
		printf("The revieved buffer1 exit data\n\r");
		return HAL_ERROR;
	}
	if(MEM_EMPTY != huart->uartRcvQueue->buffer2_length) {
		printf("The revieved buffer2 exit data\n\r");
		return HAL_ERROR;
	}
	if(!huart->first_time){
		huart->first_time = 1;
		__HAL_LOCK(huart);
		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->hdmarx->XferCpltCallback = uartDmaRcvCpltCallback;
		tmp = (uint32_t*)huart->uartRcvQueue->pinbuffer1;
		tmpa = (uint32_t*)huart->uartRcvQueue->pinbuffer2;
		HAL_DMAeRx_MultiBufferStart_IT(huart->hdmarx,(uint32_t)&huart->Instance->DR,(uint32_t)tmp,(uint32_t)tmpa,INIT_RCV_LENGTH);
		//HAL_DMA_Start_IT(huart->hdmarx, (uint32_t)&huart->Instance->DR,(uint32_t)tmp, INIT_RCV_LENGTH);
		if( (uint32_t)0x0001 != ( huart->hdmarx->Instance->CR & (uint32_t)0x0001) ){
			huart->hdmarx->Instance->NDTR = INIT_RCV_LENGTH;
//			huart->hdmarx->Instance->M1AR = (uint32_t)tmpa;//SecondMemAddress;
//			huart->hdmarx->Instance->M0AR = (uint32_t)tmp;//DstAddress;
			huart->hdmarx->Instance->CR |= (uint32_t)0x0001;
		}else{
			//printf("Rx1 DMA enabled\n\r");
		}
	
		/*Clear the idle line interrupt targ through a read to SR followed by a read to DR register*/
		//temp_register = huart->Instance->SR;
		//temp_register = huart->Instance->DR;
		__HAL_UART_CLEAR_FLAG(huart,UART_FLAG_IDLE);
		__HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);
		huart->Instance->CR3 |= USART_CR3_DMAR;	
		__HAL_UNLOCK(huart);
		return HAL_OK;  
	}else{
		__HAL_LOCK(huart);
		huart->hdmarx->XferCpltCallback = uartDmaRcvCpltCallback;
		tmp = (uint32_t*)huart->uartRcvQueue->pinbuffer1;
		tmpa = (uint32_t*)huart->uartRcvQueue->pinbuffer2;
		HAL_DMAeRx_MultiBufferStart_IT(huart->hdmarx,(uint32_t)&huart->Instance->DR,(uint32_t)tmp,(uint32_t)tmpa,INIT_RCV_LENGTH);
		if( (uint32_t)0x0001 != ( huart->hdmarx->Instance->CR & (uint32_t)0x0001) ){
			huart->hdmarx->Instance->NDTR = INIT_RCV_LENGTH;
			huart->hdmarx->Instance->M1AR = (uint32_t)tmpa;//SecondMemAddress;
			huart->hdmarx->Instance->M0AR = (uint32_t)tmp;//DstAddress;
			huart->hdmarx->Instance->CR |= (uint32_t)0x0001;
		}else{
			//printf("Rx1 DMA enabled\n\r");
		}	
		/*Clear the idle line interrupt targ through a read to SR followed by a read to DR register*/
		//temp_register = huart->Instance->SR;
		//temp_register = huart->Instance->DR;
		//__HAL_UART_CLEAR_FLAG(huart,UART_FLAG_IDLE);
		//__HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);
		//huart->Instance->CR3 |= USART_CR3_DMAR;
		__HAL_UNLOCK(huart);
    }	
}	
//HAL_StatusTypeDef halUartRecieveDma(UART_HandleTypeDef *huart) {
//    uint32_t *tmp;
////    uint32_t tmp1 = 0;
////    static uint32_t the_tem = 0;

////#ifdef MY_DEBUG
////    if( sCount - the_tem > 10) {
////        printf("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM:\n\r");
////        printf("buffer_foot:%x\n\r",(uint32_t)huart->uartRcvQueue->buffer_foot);
////        printf("buffer_head:%x\n\r",(uint32_t)huart->uartRcvQueue->buffer_head);
////        printf("p_front:%x\n\r",(uint32_t)huart->uartRcvQueue->p_front);
////        printf("p_rear:%x\n\r",(uint32_t)huart->uartRcvQueue->p_rear);
////        printf("datalength :%d\n\r",huart->uartRcvQueue->data_length);
////        printf("buffer_status:%d\n\r",huart->uartRcvQueue->data_status);
////        printf("Before:\n\r");
////        printf("DMA_>CR:%x\n\r",huart->hdmarx->Instance->CR);
////        printf("DMA_>FCR:%x\n\r",huart->hdmarx->Instance->FCR);
////        printf("DMA_>M0AR:%x\n\r",huart->hdmarx->Instance->M0AR);
////        printf("DMA_>M1AR:%x\n\r",huart->hdmarx->Instance->M1AR);
////        printf("DMA_>NDTR:%x\n\r",huart->hdmarx->Instance->NDTR);
////        printf("DMA_>PAR:%x\n\r",huart->hdmarx->Instance->PAR);
////        printf("DMA2_LISR:%x\n\r",DMA2->LISR);
////        printf("DMA2_HISR:%x\n\r",DMA2->HISR);
////        the_tem = sCount;
////    }
////#endif
//    if((huart->State == HAL_UART_STATE_READY) || (huart->State == HAL_UART_STATE_BUSY_TX)) {
//		printf("huart ready in\n\r");
//	  /* Check if a transmit rocess is ongoing or not */
//		if(huart->State == HAL_UART_STATE_BUSY_TX) {
//		  huart->State = HAL_UART_STATE_BUSY_TX_RX;
//        } else {
//		  huart->State = HAL_UART_STATE_BUSY_RX;
//        }
//		__HAL_LOCK(huart);
//        /* Set the UART DMA transfer complete callback */
//        huart->hdmarx->XferCpltCallback = uartRecieveCpltCallback;
//        /* Set the UART DMA Half transfer complete callback */
//        huart->hdmarx->XferHalfCpltCallback = NULL;//UART_DMARxHalfCplt;
//        /* Set the DMA error callback */
//        huart->hdmarx->XferErrorCallback = NULL;//UART_DMAError;
//		/*Check the current DMA buffer*/
//		if( MEM_0 == huart->uartRcvQueue->current_buffer ) {/*The current DMA recieved buffer is MEM_0*/
//		  /*Enable a DMA recieve IT*/
//		  tmp = (uint32_t*)huart->uartRcvQueue->pinbuffer1;
//		  HAL_DMA_Start_IT(huart->hdmarx, (uint32_t)&huart->Instance->DR,(uint32_t)tmp,huart->uartRcvQueue->buffer_datalen); 
//		}else{/*current DMA buffer is MEM_1*/
//		  /*Enable a DMA recieve IT*/
//		  tmp = (uint32_t*)huart->uartRcvQueue->pinbuffer2;
//		  HAL_DMA_Start_IT(huart->hdmarx, (uint32_t)&huart->Instance->DR,(uint32_t)tmp,huart->uartRcvQueue->buffer_datalen);
//		}
//		/* Enable the DMA transfer for the receiver request by setting the DMAR bit
//		  in the UART CR3 register */
//		huart->Instance->CR3 |= USART_CR3_DMAR;
//		/*Enable the UART IDLE interrupt*/
//		//__HAL_UART_CLEAR_FLAG(huart,UART_IT_IDLE);
//		temp_register = huart->Instance->SR;
//		temp_register = huart->Instance->DR;
//		__HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);
//		/* Process Locked */
//        __HAL_UNLOCK(huart);
//		return HAL_OK;
//	}else{/*The status is BUSY_RX*/
//	    getUartRecievedData(huart);
//        return HAL_BUSY;
//	}
//}
/******************************************************************************/
/*Function : uartDmaRcvCpltCallback(DMA_HandleTypeDef *hdma)*/
/*Brief    : Uart recieved callback function for Memory 0*/
/*Parameter: hdma point to the corresponding DMA Handle*/
/*retval   : None*/
/******************************************************************************/
static void uartDmaRcvCpltCallback(DMA_HandleTypeDef *hdma) {
	
	uint32_t *tmp;
	uint32_t *tmpa;
	UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
	if( 0x00000000 != (hdma->Instance->CR &0x00000001) )
	{
	  test_HaHa_EN++;
	}
	//for(x=0;x<100;x++){}
	huart->test_transfer_cpt++;
	//	  /* Current memory buffer used is Memory 0 */
	if(!huart->uartRcvQueue->last_frame){
	  huart->idle_occus = 0;
	  huart->poll_round_count=0;
	  if((hdma->Instance->CR & DMA_SxCR_CT) != DMA_SxCR_CT){
		huart->uartRcvQueue->buffer2_length = INIT_RCV_LENGTH;				
		if((CheckCanWriteNum(huart->rxFIFO) > huart->uartRcvQueue->buffer2_length)){
			WriteFIFO(huart->rxFIFO,huart->uartRcvQueue->pinbuffer2,huart->uartRcvQueue->buffer2_length);	
			huart->test_rcv_before += INIT_RCV_LENGTH;
			huart->uartRcvQueue->buffer2_length = 0;			
		}else
			huart->test_fifo_full++ ;	
	  }else if((hdma->Instance->CR & DMA_SxCR_CT) == DMA_SxCR_CT){
			huart->uartRcvQueue->buffer1_length = INIT_RCV_LENGTH;				
			if((CheckCanWriteNum(huart->rxFIFO) > huart->uartRcvQueue->buffer1_length)){
				WriteFIFO(huart->rxFIFO,huart->uartRcvQueue->pinbuffer1,huart->uartRcvQueue->buffer1_length);	
				huart->test_rcv_before += INIT_RCV_LENGTH;
 				huart->uartRcvQueue->buffer1_length = 0;	
			}else
				huart->test_fifo_full++ ;	
	  }
	}else{
		huart->uartRcvQueue->last_frame = 0;
		//huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAR);		
		/* Current memory buffer used is Memory 0 */
		if((huart->hdmarx->Instance->CR & DMA_SxCR_CT) != DMA_SxCR_CT){
			huart->uartRcvQueue->buffer1_length =  huart->uartRcvQueue->buffer_datalen - huart->hdmarx->Instance->NDTR;
			if((CheckCanWriteNum(huart->rxFIFO) > huart->uartRcvQueue->buffer1_length)){
			  WriteFIFO(huart->rxFIFO,huart->uartRcvQueue->pinbuffer1,huart->uartRcvQueue->buffer1_length);	
			//test_HaHa_else += (INIT_RCV_LENGTH - huart->hdmarx->Instance->NDTR);
			  huart->test_rcv_before += huart->uartRcvQueue->buffer1_length;
			  huart->uartRcvQueue->buffer1_length = 0;
			}else{
			  huart->test_fifo_full++ ;
			}
		}else if((huart->hdmarx->Instance->CR & DMA_SxCR_CT) == DMA_SxCR_CT) {
			huart->uartRcvQueue->buffer2_length = huart->uartRcvQueue->buffer_datalen - huart->hdmarx->Instance->NDTR;
			if((CheckCanWriteNum(huart->rxFIFO) > huart->uartRcvQueue->buffer2_length)){
			  WriteFIFO(huart->rxFIFO,huart->uartRcvQueue->pinbuffer2,huart->uartRcvQueue->buffer2_length);	
			  //test_HaHa_else += (INIT_RCV_LENGTH - huart->hdmarx->Instance->NDTR);
			  huart->test_rcv_before += huart->uartRcvQueue->buffer2_length;
			  huart->uartRcvQueue->buffer2_length = 0;	
			}else{
			  huart->test_fifo_full++ ;
			}
		}
		huart->test_idle_dma_targ++ ;
		tmp = (uint32_t*)huart->uartRcvQueue->pinbuffer1;
		tmpa = (uint32_t*)huart->uartRcvQueue->pinbuffer2;
		hdma->Instance->NDTR = INIT_RCV_LENGTH;
		hdma->Instance->M1AR = (uint32_t)tmpa;//SecondMemAddress;
		hdma->Instance->M0AR = (uint32_t)tmp;//DstAddress;
		hdma->Instance->CR |= (uint32_t)0x0001;		
	}	
}	

/******************************************************************************/
/*Function : uartRecieveCpltCallback(DMA_HandleTypeDef *hdma)*/
/*Brief    : Uart recieved callback function for Memory*/
/*Parameter: hdma point to the corresponding DMA Handle*/
/*retval   : None*/
/******************************************************************************/
#if 0
static void uartRecieveCpltCallback(DMA_HandleTypeDef *hdma) {
    UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
    uint32_t *tmp;
	hdma->Instance->CR &= (uint32_t)0xfffffffe;
    //printf("Rcvchar:%c\n\r",*( huart->uartRcvQueue->p_rear) );
    if(huart->uartRcvQueue->data_status == MEM_FULL) {
        /*When the circle buffer overlap,printf a message for a warning*/
        //printf("The circle buffer overlap!!!\n\r");
		__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
        huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAR);
        /* Check if a receive process is ongoing or not */
        if(huart->State == HAL_UART_STATE_BUSY_TX_RX) {
            huart->State = HAL_UART_STATE_BUSY_TX;
        } else {
            huart->State = HAL_UART_STATE_READY;
        }
    } else { /*When the buffer is not full*/
        targ++;
        huart->uartRcvQueue->data_length ++;
        /*Config the status of the Queue circle buffer*/
        if(huart->uartRcvQueue->p_rear == huart->uartRcvQueue->buffer_foot) {
            huart->uartRcvQueue->p_rear = huart->uartRcvQueue->buffer_head;
        } else { //huart->uartRcvQueue->p_rear != huart->uartRcvQueue->buffer_foot
            huart->uartRcvQueue->p_rear++;
        }
        /*Check if the buffer is full*/
        if((huart->uartRcvQueue->p_rear == huart->uartRcvQueue->buffer_foot) && (huart->uartRcvQueue->p_front == huart->uartRcvQueue->buffer_head) ) {
            huart->uartRcvQueue->data_status = MEM_FULL;
        } else if(huart->uartRcvQueue->p_rear + 1 == huart->uartRcvQueue->p_front) {
            huart->uartRcvQueue->data_status = MEM_FULL;
        } else {
            huart->uartRcvQueue->data_status = MEM_NOT_EMPTY;
        }
    }
    //printf("targ = %d",targ);
    //__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_HT_FLAG_INDEX(hdma));
    /* Clear the transfer complete flag */
	//__HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
//	printf("CR:%x\n\r",hdma->Instance->CR);
    //hdma->Instance->CR &= (uint32_t)0xfffffffe;
	if( 0x00000000 != (hdma->Instance->CR &0x00000001) )
	{
	  printf("DMA Didn't closedYYYYYYYYYYY\n\r");
	}
    hdma->Instance->M0AR = (uint32_t)huart->uartRcvQueue->p_rear;
    hdma->Instance->NDTR = 1;
	__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
    hdma->Instance->CR |= (uint32_t)0x00000001;
    /*Start the DMA request again*/
    //huart->Instance->CR3 |= USART_CR3_DMAR;
//    tmp = (uint32_t*)huart->uartRcvQueue->p_rear;
//    HAL_DMA_Start_IT(huart->hdmarx, (uint32_t)&huart->Instance->DR,(uint32_t)tmp,1);
}
#endif
#if 0
static void uartRecieveCpltCallback(DMA_HandleTypeDef *hdma) {
    UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
//	uint32_t length;
	//__HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
	if( 0x00000000 != (hdma->Instance->CR &0x00000001) )
	{
	  test_HaHa_EN++;
	  hdma->Instance->CR &= 0xfffffffe;
	} 
	//printf("DMA interrupt in\n\r");
	/*Check which is the current buffer*/
	if(MEM_0 == huart->uartRcvQueue->current_buffer ) {/*The current DMA buffer is MEM_0*/
	  /*Get the recieved data length*/
	  huart->uartRcvQueue->buffer1_length = huart->uartRcvQueue->buffer_datalen - huart->hdmarx->Instance->NDTR;
	  /*Check if buffer2 still exit some data remained,if so ,disable the DMA and */
	  if(0 != huart->uartRcvQueue->buffer2_length){
		__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
        huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAR);
        /* Check if a receive process is ongoing or not */
        if(huart->State == HAL_UART_STATE_BUSY_TX_RX) {
            huart->State = HAL_UART_STATE_BUSY_TX;
        } else {
            huart->State = HAL_UART_STATE_READY;
        }
		return ;
	  }
	  /*Start the next buffer's DMA*/
	  hdma->Instance->M0AR = (uint32_t)huart->uartRcvQueue->pinbuffer2;
	  hdma->Instance->NDTR = huart->uartRcvQueue->buffer_datalen;
	  __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
	  huart->uartRcvQueue->current_buffer = MEM_1;
	  hdma->Instance->CR |= (uint32_t)0x00000001;
	}else{
	  /*Get the recieved data length*/
	  huart->uartRcvQueue->buffer2_length = huart->uartRcvQueue->buffer_datalen - huart->hdmarx->Instance->NDTR;
	  /*Check if buffer1 still exit some data remained,if so ,disable the DMA and */
	  if(0 != huart->uartRcvQueue->buffer1_length){
		__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
        huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAR);
        /* Check if a receive process is ongoing or not */
        if(huart->State == HAL_UART_STATE_BUSY_TX_RX) {
            huart->State = HAL_UART_STATE_BUSY_TX;
        } else {
            huart->State = HAL_UART_STATE_READY;
        }
		return;
	  } 
	  /*Start the next buffer's DMA*/
	  hdma->Instance->M0AR = (uint32_t)huart->uartRcvQueue->pinbuffer1;
	  hdma->Instance->NDTR = huart->uartRcvQueue->buffer_datalen;
	  __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
	  huart->uartRcvQueue->current_buffer = MEM_0;
	  hdma->Instance->CR |= (uint32_t)0x00000001;	
	}
}	


/******************************************************************************/
/*Function : uartDmaRcvCpltCallback(DMA_HandleTypeDef *hdma)*/
/*Brief    : Uart recieved callback function for Memory 0*/
/*Parameter: hdma point to the corresponding DMA Handle*/
/*retval   : None*/
/******************************************************************************/
static void uartDmaRcvCpltCallback(DMA_HandleTypeDef *hdma) {
    UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
	if( 0x00000000 != (hdma->Instance->CR &0x00000001) )
	{
	  test_HaHa_EN++;
	}
	/*Get the recieved data length*/
	huart->uartRcvQueue->buffer1_length = huart->uartRcvQueue->buffer_datalen - huart->hdmarx->Instance->NDTR;
	/*Check if buffer2 still exit some data remained,if so ,disable the DMA and */
	if(0 != huart->uartRcvQueue->buffer2_length){
		__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
        huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAR);
        /* Check if a receive process is ongoing or not */
        if(huart->State == HAL_UART_STATE_BUSY_TX_RX) {
            huart->State = HAL_UART_STATE_BUSY_TX;
        } else {
            huart->State = HAL_UART_STATE_READY;
        }
		hdma->Instance->CR &= 0xfffffffe;
		return ;
	}else{
		__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
		/*Check if the EN bit is enabled*/
		if( 0x00000000 == (hdma->Instance->CR & 0x00000001) ){
		  hdma->Instance->CR |= 0x00000001;
		}
	}
}	

/******************************************************************************/
/*Function : uartDmaRcvM1CpltCallback(DMA_HandleTypeDef *hdma)*/
/*Brief    : Uart recieved callback function for Memory 1*/
/*Parameter: hdma point to the corresponding DMA Handle*/
/*retval   : None*/
/******************************************************************************/
static void uartDmaRcvM1CpltCallback(DMA_HandleTypeDef *hdma){
    UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
	if( 0x00000000 != (hdma->Instance->CR &0x00000001) )
	{
	  test_HaHa_EN++;
	}
	/*Get the recieved data length*/
	huart->uartRcvQueue->buffer2_length = huart->uartRcvQueue->buffer_datalen - huart->hdmarx->Instance->NDTR;
	/*Check if buffer2 still exit some data remained,if so ,disable the DMA and */
	if(0 != huart->uartRcvQueue->buffer1_length){
		__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
        huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAR);
        /* Check if a receive process is ongoing or not */
        if(huart->State == HAL_UART_STATE_BUSY_TX_RX) {
            huart->State = HAL_UART_STATE_BUSY_TX;
        } else {
            huart->State = HAL_UART_STATE_READY;
        }
		hdma->Instance->CR &= 0xfffffffe;
		return ;
	}else{
		__HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));
		/*Check if the EN bit is enabled*/
		if( 0x00000000 == (hdma->Instance->CR & 0x00000001) ){
		  hdma->Instance->CR |= 0x00000001;
		}
	}	
  }

#endif
/******************************************************************************/
/*Function : uartRecieveIdleCallback(UART_HandleTypeDef *huart)*/
/*Brief    : Uart recieved callback function for Idle interrupt*/
/*Parameter: huart point to the corresponding UART*/
/*retval   : None*/
/******************************************************************************/
void uartRecieveIdleCallback(UART_HandleTypeDef *huart){
///*Disable the recieved DMA*/
//  huart->hdmarx->Instance->CR &= UART_DMA_DISABLE;
//  while(1){
//  if( UART_DMA_ENABLE != (huart->hdmarx->Instance->CR & UART_DMA_ENABLE ) ){
//	  break;
//	}
//  }
// // huart->uartRcvQueue->idle_targ = 1;
///*Clear the idle line interrupt targ through a read to SR followed by a read to DR register*/
//  temp_register = huart->Instance->SR;
//  temp_register = huart->Instance->DR;
	/*Clear the idle line interrupt targ through a read to SR followed by a read to DR register*/
	temp_register = huart->Instance->SR;
	temp_register = huart->Instance->DR;
	
	huart->uartRcvQueue->idle_targ = 1;
	huart->idle_occus++;
//	if(huart->poll_round_count>10){
//		huart->poll_round_count = 0;
//		huart->idle_occus = 0; 
//		huart->uartRcvQueue->last_frame = 1; 
//		if(0 != huart->hdmarx->Instance->NDTR){
//			huart->hdmarx->Instance->CR &= UART_DMA_DISABLE;
//		}
//	}
}


/******************************************************************************/
/*Function : getUartRecievedData(UART_HandleTypeDef *huart)*/
/*Brief    : Get the recieved data and add some of them into the COM port Queue*/
/*Parameter: hdma point to the corresponding DMA Handle*/
/*retval   : */
/******************************************************************************/
//static uint8_t getUartRecievedData(UART_HandleTypeDef *huart) {
//    uint32_t tem_mscount;
//    uint8_t  tem_buffer[INIT_RCV_LENGTH];
//    uint16_t i;
//    uint32_t length;
//    uint8_t  serialNo;

//    if(USART6 == huart->Instance) {
//        serialNo = 0;
//    } else if(UART5 == huart->Instance) {
//        serialNo = 1;
//    } else if(UART7 == huart->Instance) {
//        serialNo = 2;
//    } else if(UART4 ==huart->Instance) {
//        serialNo = 3;
//    } else {
//        return 0xff;
//    }
//	//printf("get data in\n\r");
//	memset(tem_buffer,0,INIT_RCV_LENGTH);
//    if( MEM_0 == huart->uartRcvQueue->current_buffer) {
//	/*Check if the idle buffer has data*/
//	  if(0 != huart->uartRcvQueue->buffer2_length){
//		length = huart->uartRcvQueue->buffer2_length;
//		memcpy(tem_buffer,huart->uartRcvQueue->pinbuffer2,length);
//		huart->uartRcvQueue->buffer2_length = 0;
//		test_rcv_length += length;
//#ifdef MY_DEBUG_P
//		//printf("Serial %d length : %d\n\r",serialNo+1,length);
//		printf("test_rcv_length=%d\n\r",test_rcv_length);
//#endif
//	    addDataToClientQueue(tem_buffer,length,serialNo);
//		return 1;
//	  }
//    }else{/*The current buffer is MEM_1*/
//	  if(0 != huart->uartRcvQueue->buffer1_length){
//		length = huart->uartRcvQueue->buffer1_length;
//		memcpy(tem_buffer,huart->uartRcvQueue->pinbuffer1,length);
//		huart->uartRcvQueue->buffer1_length = 0;
//		test_rcv_length += length;
//#ifdef MY_DEBUG_P
//		//printf("Serial %d length : %d\n\r",serialNo+1,length);
//		printf("test_rcv_length=%d\n\r",test_rcv_length);
//#endif
//	    addDataToClientQueue(tem_buffer,length,serialNo);
//		return 1;
//	  }
//	}
//	return 0;
//  }

static uint8_t getUartRecievedData(UART_HandleTypeDef *huart/*,uint8_t bufferNo*/) {
	uint32_t tem_mscount;
    uint8_t  tem_buffer[2000];
    uint16_t i;
    uint32_t length;
    uint8_t  serialNo;

    if(USART6 == huart->Instance) {
        serialNo = 0;
    } else if(UART5 == huart->Instance) {
        serialNo = 1;
    } else if(UART7 == huart->Instance) {
        serialNo = 2;
    } else if(UART4 == huart->Instance) {
        serialNo = 3;
    } else {
        return 0xff;
    }
	//printf("get data in\n\r");	
	length=CheckCanReadNum(huart->rxFIFO);
	//printf("fifo read length=%d\n\r",length);	
	if(length > 2000) 		
		length = 2000;	
//	if(length > INIT_RCV_LENGTH){
	memset(tem_buffer,0,2000);
	//ReadFIFO(huart->rxFIFO,tem_buffer,length);	
	if(ReadFIFO(huart->rxFIFO,tem_buffer,length)){	
	  huart->test_rcv_len += length;
	}else{
	  length=0;
	}
	//huart->test_rcv_len += length;
//	#ifdef MY_DEBUG_P
	//printf("Serial %d length : %d\n\r",serialNo+1,length);
//	  printf("test_rcv1_length=%d\n\r",test_rcv_length);
//  #endif
	huart->test_chk_byte = tem_buffer[0];
//	for(i=0;i<length;i++){
//		if(tem_buffer[i] != huart->test_chk_byte){
//			if(huart->test_first_byte == 1){
//			  huart->test_the_error_byte=tem_buffer[i];
//			  huart->test_first_byte++;
//			}
//			huart->test_error_byte++;
//		}
//		if(huart->test_chk_byte == 255){
//			huart->test_chk_byte=0;
//		}else{
//			huart->test_chk_byte++;
//		}
//	}
	huart->test_can_get_in_buffer=huart->uartRcvQueue->buffer_datalen - huart->hdmarx->Instance->NDTR;
	if(0 < length){
	  addDataToClientQueue(tem_buffer,length,serialNo);
	  //printf("Serial%d Add client Queue\n\r",serialNo+1);
	  //__heapstats((__heapprt)fprintf,stderr);
	}else{
	
	}
//	if( CheckCanWriteNum(huart->pfifo) >= length ){
//		if(WriteFIFO(huart->pfifo,tem_buffer,length)){
//			huart->test_before += length;
//			//printf("recieved beforeFIFO=%d\n\r",length);
//		}else{
//			printf("Write FIFO error\n\r");
//		}
//	}	
  }
/******************************************************************************/
/*Function : getUartRecievedData(UART_HandleTypeDef *huart)*/
/*Brief    : Wait till timeout*/
/*Parameter: hdma point to the corresponding DMA Handle*/
/*retval   : */
/******************************************************************************/
static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status, uint32_t Timeout) {
    uint32_t timeout = 0;

    timeout = msCount + Timeout;

    /* Wait until flag is set */
    if(Status == RESET) {
        while(__HAL_UART_GET_FLAG(huart, Flag) == RESET) {
            /* Check for the Timeout */
            if(Timeout != HAL_MAX_DELAY) {
                if(msCount >= timeout) {
                    /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
                    __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
                    __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
                    __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
                    __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

                    huart->State= HAL_UART_STATE_READY;

                    /* Process Unlocked */
                    __HAL_UNLOCK(huart);

                    return HAL_TIMEOUT;
                }
            }
        }
    } else {
        while(__HAL_UART_GET_FLAG(huart, Flag) != RESET) {
            /* Check for the Timeout */
            if(Timeout != HAL_MAX_DELAY) {
                if( msCount >= timeout ) {
                    /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
                    __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
                    __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
                    __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
                    __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

                    huart->State= HAL_UART_STATE_READY;

                    /* Process Unlocked */
                    __HAL_UNLOCK(huart);

                    return HAL_TIMEOUT;
                }
            }
        }
    }
    return HAL_OK;
}

/********************************************************************************************************/
/*****************************************test Serial****************************************************/
/********************************************************************************************************/
void testSerial() {
    uint32_t tmp1;
    /*Init*/
	memset(test_String,'a',500);
    com1DmaInit(&Uart6Handle);
    com2DmaInit(&Uart5Handle);
    com3DmaInit(&Uart7Handle);
    com4DmaInit(&Uart4Handle);
    /*Transmit*/
//	halUartTransmitDma(&Uart6Handle);
//	printf("Change buffer number\n\r");
//	while(1)
//	{
//	  Blink_LED_RUN();
//	  HAL_UART_Transmit_DMA(&Uart6Handle,testString,20);
//	  halUartRecieveDma(&Uart6Handle);
//	  halUartRecieveDma(&Uart5Handle);
//	  halUartRecieveDma(&Uart7Handle);
//	  halUartRecieveDma(&Uart4Handle);
//	}
//	halUartTransmitDma1(&Uart6Handle);
//	while(1){
//	tmp1 = Uart6Handle.State;
//	if((tmp1 == HAL_UART_STATE_READY) || (tmp1 == HAL_UART_STATE_BUSY_RX))
//	{
    //HAL_UART_Transmit_DMA(&Uart7Handle, testString,20);
    //halUartTransmitDma(&Uart6Handle);
    //HAL_UART_Transmit_DMA(&Uart7Handle);
//	}
//	HAL_UART_Transmit_DMA(&Uart5Handle, testStrings,20);
//	HAL_UART_Transmit_DMA(&Uart6Handle,testStringss,20);
//	HAL_UART_Transmit_DMA(&Uart4Handle, testStringsss,20);
//    }
}

/********************************************************************************************************/
/*****************************************UART initialization********************************************/
/********************************************************************************************************/
/******************************************************************************/
/*Function : com1DmaInit(UART_HandleTypeDef *huart)*/
/*Brief    : DMA configuration for transmission request by peripheral of COM1(USART6)*/
/*Parameter: UART_HandleTypeDef : huart point to the correspinding USART*/
/*retval   : None*/
/******************************************************************************/
void com1DmaInit(UART_HandleTypeDef *huart) {
    static DMA_HandleTypeDef hdma_tx;
    static DMA_HandleTypeDef hdma_rx;
    uint32_t func_addr;

    /*##-3- Configure the DMA streams ##########################################*/
    /* Configure the DMA handler for Transmission process */
    hdma_tx.Instance                 = USART6_TX_DMA_STREAM;
    hdma_tx.Init.Channel             = USART6_TX_DMA_CHANNEL;
    hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_tx.Init.Mode                = DMA_NORMAL;
    hdma_tx.Init.Priority            = DMA_PRIORITY_HIGH;
    //hdma_tx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_tx.Init.MemBurst            = DMA_MBURST_INC8;
    hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC8;
    HAL_DMA_Init(&hdma_tx);
    /* Associate the initialized DMA handle to the UART handle */
    __HAL_LINKDMA(huart, hdmatx, hdma_tx);
    /* Configure the DMA handler for reception process */
    hdma_rx.Instance                 = USART6_RX_DMA_STREAM;
    hdma_rx.Init.Channel             = USART6_RX_DMA_CHANNEL;
    hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_rx.Init.Mode                = DMA_NORMAL;
    hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    //hdma_rx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
    hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4;

    HAL_DMA_Init(&hdma_rx);
    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_rx);

    /*Init the DMA memory status*/
    com1DmaStatus.current_mem = MEM_0;
    com1DmaStatus.mem0_length = 0;
    com1DmaStatus.mem1_length = 0;
	com1DmaStatus.mem_length  =  0;
    com1DmaStatus.pbuffer1    = com1dataBuffer1;
    com1DmaStatus.pbuffer2    = com1dataBuffer2;
	com1DmaStatus.pbuffer     = com1dataBuffer;
	com1DmaStatus.single_finish_targ = 1;
	com1DmaStatus.double_finish_targ = 0;
    //memset(com1dataBuffer2,'f',50);
    huart->comDmaStatus = &com1DmaStatus;
    /*Init the status of DMA recieving circulatory buffer*/
	com1RcvStatus.buffer_datalen = INIT_RCV_LENGTH;
	com1RcvStatus.pinbuffer1     = com1rcvBuffer1;
	com1RcvStatus.pinbuffer2     = com1rcvBuffer2;
	com1RcvStatus.current_buffer = MEM_0;
	com1RcvStatus.buffer1_length = 0;
	com1RcvStatus.buffer2_length = 0;
	com1RcvStatus.idle_targ = 0;
	huart->uartRcvQueue          = &com1RcvStatus;
	
	InitFIFO(&com1rxFIFO,com1rcvFIFO,COM_RCV_DATA_SIZE*10);
	huart->rxFIFO = &com1rxFIFO;
	InitFIFO(&com1FIFO,com1dataFIFO,COM_DATA_SIZE*10);
	huart->pfifo = &com1FIFO;
//    com1RcvStatus.data_length = 0;
//    com1RcvStatus.data_status = MEM_EMPTY;
//    com1RcvStatus.p_front     = com1rcvBuffer1;
//    com1RcvStatus.p_rear      = com1rcvBuffer1;
//    com1RcvStatus.buffer_head = com1rcvBuffer1;
//    com1RcvStatus.buffer_foot = com1rcvBuffer1 + COM_DATA_SIZE*COM_DATA_PACK_NUM -1;
//    huart->uartRcvQueue       = &com1RcvStatus;
	
	/*Test total data*/
	huart->test_after  = 0;
	huart->test_before = 0;
	huart->test_rcv_before =0;
	huart->test_error_byte =0;
	huart->test_error_transfer=0;
	huart->test_rcv_len =0;
	huart->first_time = 0;
	huart->test_fifo_full = 0;
	huart->test_fifo_full = 0;	
	huart->poll_round_count = 0;
	huart->idle_occus =0;
	huart->test_print_time =0;
	huart->test_total_get_length=0;
	huart->count_targ = 0;
	huart->test_idle_dma_targ = 0;
	huart->test_transfer_cpt = 0;
	huart->test_first_byte = 1;
	huart->test_can_get_in_buffer = 0;
	huart->test_lose_block=0;
}

/******************************************************************************/
/*Function : com2DmaInit(UART_HandleTypeDef *huart)*/
/*Brief    : DMA configuration for transmission request by peripheral of COM2(UART5)*/
/*Parameter: UART_HandleTypeDef : huart point to the correspinding USART*/
/*retval   : None*/
/******************************************************************************/
void com2DmaInit(UART_HandleTypeDef *huart) {
    static DMA_HandleTypeDef hdma_tx;
    static DMA_HandleTypeDef hdma_rx;

    /*##-3- Configure the DMA streams ##########################################*/
    /* Configure the DMA handler for Transmission process */
    hdma_tx.Instance                 = UART5_TX_DMA_STREAM;
    hdma_tx.Init.Channel             = UART5_TX_DMA_CHANNEL;
    hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_tx.Init.Mode                = DMA_NORMAL;
    hdma_tx.Init.Priority            = DMA_PRIORITY_HIGH;
    //hdma_tx1.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_tx.Init.MemBurst            = DMA_MBURST_INC8;
    hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC8;
    HAL_DMA_Init(&hdma_tx);

    /* Associate the initialized DMA handle to the UART handle */
    __HAL_LINKDMA(huart, hdmatx, hdma_tx);
    /* Configure the DMA handler for reception process */
    hdma_rx.Instance                 = UART5_RX_DMA_STREAM;
    hdma_rx.Init.Channel             = UART5_RX_DMA_CHANNEL;
    hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_rx.Init.Mode                = DMA_NORMAL;
    hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
	//hdma_rx1.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_rx.Init.MemBurst            = DMA_MBURST_INC8;
    hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC8;

    HAL_DMA_Init(&hdma_rx);
    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_rx);
    /*Init the DMA memory status*/
    com2DmaStatus.current_mem = MEM_0;
    com2DmaStatus.mem0_length = 0;
    com2DmaStatus.mem1_length = 0;
	com2DmaStatus.mem_length = 0;
    com2DmaStatus.pbuffer1    = com2dataBuffer1;
    com2DmaStatus.pbuffer2    = com2dataBuffer2;
	com2DmaStatus.pbuffer    = com2dataBuffer;
	com2DmaStatus.single_finish_targ = 1;
	com2DmaStatus.double_finish_targ = 0;
    huart->comDmaStatus = &com2DmaStatus;
    /*Init the status of DMA recieving circulatory buffer*/
//    com2RcvStatus.data_length = 0;
//    com2RcvStatus.data_status = MEM_EMPTY;
//    com2RcvStatus.p_front     = com2rcvBuffer1;
//    com2RcvStatus.p_rear      = com2rcvBuffer1;
//    com2RcvStatus.buffer_head = com2rcvBuffer1;
//    com2RcvStatus.buffer_foot = com2rcvBuffer1 + COM_DATA_SIZE*COM_DATA_PACK_NUM - 1;
//    huart->uartRcvQueue       = &com2RcvStatus;
	com2RcvStatus.buffer_datalen = INIT_RCV_LENGTH;
	com2RcvStatus.pinbuffer1     = com2rcvBuffer1;
	com2RcvStatus.pinbuffer2     = com2rcvBuffer2;
	com2RcvStatus.current_buffer = MEM_0;
	com2RcvStatus.buffer1_length = 0;
	com2RcvStatus.buffer2_length = 0;
	com2RcvStatus.idle_targ      = 0;
	huart->uartRcvQueue          = &com2RcvStatus;
	
	InitFIFO(&com2rxFIFO,com2rcvFIFO,COM_RCV_DATA_SIZE*10);
	huart->rxFIFO = &com2rxFIFO;
	InitFIFO(&com2FIFO,com2dataFIFO,COM_DATA_SIZE*10);
	huart->pfifo = &com2FIFO;
	
	huart->test_after  = 0;
	huart->test_before = 0;
	huart->test_rcv_before =0;
	huart->test_error_byte =0;
	huart->test_error_transfer=0;
	huart->test_rcv_len =0;
	huart->first_time = 0;
	huart->test_fifo_full = 0;
	huart->test_fifo_full = 0;	
	huart->poll_round_count = 0;
	huart->idle_occus =0;
	huart->test_print_time =0;
	huart->test_total_get_length=0;
	huart->count_targ = 0;
	huart->test_idle_dma_targ=0;
	huart->test_transfer_cpt=0;
	huart->test_first_byte = 1;
	huart->test_can_get_in_buffer = 0;
	huart->test_lose_block=0;
}

/******************************************************************************/
/*Function : com3DmaInit(UART_HandleTypeDef *huart)*/
/*Brief    : DMA configuration for transmission request by peripheral of COM3(UART7)*/
/*Parameter: UART_HandleTypeDef : huart point to the correspinding USART*/
/*retval   : None*/
/******************************************************************************/
void com3DmaInit(UART_HandleTypeDef *huart) {
    static DMA_HandleTypeDef hdma_tx;
    static DMA_HandleTypeDef hdma_rx;

    /*##-3- Configure the DMA streams ##########################################*/
    /* Configure the DMA handler for Transmission process */
    hdma_tx.Instance                 = UART7_TX_DMA_STREAM;
    hdma_tx.Init.Channel             = UART7_TX_DMA_CHANNEL;
    hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_tx.Init.Mode                = DMA_NORMAL;
    hdma_tx.Init.Priority            = DMA_PRIORITY_HIGH;
    //hdma_tx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	//hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
    hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
    HAL_DMA_Init(&hdma_tx);
    /* Associate the initialized DMA handle to the UART handle */
    __HAL_LINKDMA(huart, hdmatx, hdma_tx);
    /* Configure the DMA handler for reception process */
    hdma_rx.Instance                 = UART7_RX_DMA_STREAM;
    hdma_rx.Init.Channel             = UART7_RX_DMA_CHANNEL;
    hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_rx.Init.Mode                = DMA_NORMAL;
    hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
    hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4;
    HAL_DMA_Init(&hdma_rx);
    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_rx);
    /*Init the DMA memory status*/
    com3DmaStatus.current_mem = MEM_0;
    com3DmaStatus.mem0_length = 0;
    com3DmaStatus.mem1_length = 0;
	com3DmaStatus.mem_length = 0;
    com3DmaStatus.pbuffer1    = com3dataBuffer1;
    com3DmaStatus.pbuffer2    = com3dataBuffer2;
	com3DmaStatus.pbuffer    = com3dataBuffer;
	com3DmaStatus.single_finish_targ = 1;
	com3DmaStatus.double_finish_targ = 0;
    huart->comDmaStatus = &com3DmaStatus;
    /*Init the status of DMA recieving circulatory buffer*/
//    com3RcvStatus.data_length = 0;
//    com3RcvStatus.data_status = MEM_EMPTY;
//    com3RcvStatus.p_front     = com3rcvBuffer1;
//    com3RcvStatus.p_rear      = com3rcvBuffer1;
//    com3RcvStatus.buffer_head = com3rcvBuffer1;
//    com3RcvStatus.buffer_foot = com3rcvBuffer1 + COM_DATA_SIZE*COM_DATA_PACK_NUM -1;
//    huart->uartRcvQueue       = &com3RcvStatus;
	com3RcvStatus.buffer_datalen = INIT_RCV_LENGTH;
	com3RcvStatus.pinbuffer1     = com3rcvBuffer1;
	com3RcvStatus.pinbuffer2     = com3rcvBuffer2;
	com3RcvStatus.current_buffer = MEM_0;
	com3RcvStatus.buffer1_length = 0;
	com3RcvStatus.buffer2_length = 0;
	com3RcvStatus.idle_targ      = 0;
	huart->uartRcvQueue          = &com3RcvStatus;
	InitFIFO(&com3rxFIFO,com3rcvFIFO,COM_RCV_DATA_SIZE*10);
	huart->rxFIFO = &com3rxFIFO;
	InitFIFO(&com3FIFO,com3dataFIFO,COM_DATA_SIZE*10);
	huart->pfifo = &com3FIFO;
	
	huart->test_after  = 0;
	huart->test_before = 0;
	huart->test_rcv_before =0;
	huart->test_error_byte =0;
	huart->test_error_transfer=0;
	huart->test_rcv_len =0;
	huart->first_time = 0;
	huart->test_fifo_full = 0;
	huart->test_fifo_full = 0;	
	huart->poll_round_count = 0;
	huart->idle_occus =0;
	huart->test_print_time =0;
	huart->test_total_get_length=0;
	huart->count_targ = 0;
	huart->test_idle_dma_targ=0;
	huart->test_transfer_cpt=0;
	huart->test_first_byte = 1;
	huart->test_can_get_in_buffer = 0;
	huart->test_lose_block=0;
}

/******************************************************************************/
/*Function : com4DmaInit(UART_HandleTypeDef *huart)*/
/*Brief    : DMA configuration for transmission request by peripheral of COM4(UART4)*/
/*Parameter: UART_HandleTypeDef : huart point to the correspinding USART*/
/*retval   : None*/
/******************************************************************************/
void com4DmaInit(UART_HandleTypeDef *huart) {
    static DMA_HandleTypeDef hdma_tx;
    static DMA_HandleTypeDef hdma_rx;

    /*##-3- Configure the DMA streams ##########################################*/
    /* Configure the DMA handler for Transmission process */
    hdma_tx.Instance                 = UART4_TX_DMA_STREAM;
    hdma_tx.Init.Channel             = UART4_TX_DMA_CHANNEL;
    hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    //hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    hdma_tx.Init.Mode                = DMA_NORMAL;
    //hdma_tx.Init.Mode                = DMA_SxCR_PFCTRL;
    hdma_tx.Init.Priority            = DMA_PRIORITY_HIGH;
    //hdma_tx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    //hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_tx.Init.MemBurst            = DMA_MBURST_INC8;
    hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC8;
    HAL_DMA_Init(&hdma_tx);

    /* Associate the initialized DMA handle to the UART handle */
    __HAL_LINKDMA(huart, hdmatx, hdma_tx);
    /* Configure the DMA handler for reception process */
    hdma_rx.Instance                 = UART4_RX_DMA_STREAM;
    hdma_rx.Init.Channel             = UART4_RX_DMA_CHANNEL;
    hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_rx.Init.Mode                = DMA_NORMAL;
    hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_rx.Init.MemBurst            = DMA_MBURST_INC8;
    hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC8;

    HAL_DMA_Init(&hdma_rx);
    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_rx);
    /*Init the DMA memory status*/
    com4DmaStatus.current_mem = MEM_0;
    com4DmaStatus.mem0_length = 0;
    com4DmaStatus.mem1_length = 0;
	com4DmaStatus.mem_length = 0;
    com4DmaStatus.pbuffer1    = com4dataBuffer1;
    com4DmaStatus.pbuffer2    = com4dataBuffer2;
	com4DmaStatus.pbuffer    = com4dataBuffer;
	com4DmaStatus.single_finish_targ = 1;
	com4DmaStatus.double_finish_targ = 0;
    huart->comDmaStatus = &com4DmaStatus;
    /*Init the status of DMA recieving circulatory buffer*/
//    com4RcvStatus.data_length = 0;
//    com4RcvStatus.data_status = MEM_EMPTY;
//    com4RcvStatus.p_front     = com4rcvBuffer1;
//    com4RcvStatus.p_rear      = com4rcvBuffer1;
//    com4RcvStatus.buffer_head = com4rcvBuffer1;
//    com4RcvStatus.buffer_foot = com4rcvBuffer1 + COM_DATA_SIZE*COM_DATA_PACK_NUM - 1;
//    huart->uartRcvQueue       = &com4RcvStatus;
	com4RcvStatus.buffer_datalen = INIT_RCV_LENGTH;
	com4RcvStatus.pinbuffer1     = com4rcvBuffer1;
	com4RcvStatus.pinbuffer2     = com4rcvBuffer2;
	com4RcvStatus.current_buffer = MEM_0;
	com4RcvStatus.buffer1_length = 0;
	com4RcvStatus.buffer2_length = 0;
	com4RcvStatus.idle_targ = 0;
	huart->uartRcvQueue          = &com4RcvStatus;

	InitFIFO(&com4rxFIFO,com4rcvFIFO,COM_RCV_DATA_SIZE*10);
	huart->rxFIFO = &com4rxFIFO;
	InitFIFO(&com4FIFO,com4dataFIFO,COM_DATA_SIZE*10);
	huart->pfifo = &com4FIFO;
	
	huart->test_after  = 0;
	huart->test_before = 0;
	huart->test_rcv_before =0;
	huart->test_error_byte =0;
	huart->test_error_transfer=0;
	huart->test_rcv_len =0;
	huart->first_time = 0;
	huart->test_fifo_full = 0;
	huart->test_fifo_full = 0;	
	huart->poll_round_count = 0;
	huart->idle_occus =0;
	huart->test_print_time =0;
	huart->test_total_get_length=0;
	huart->count_targ = 0;
	huart->test_idle_dma_targ = 0;
    huart->test_transfer_cpt = 0;
	huart->test_first_byte = 1;
	huart->test_can_get_in_buffer = 0;
	huart->test_lose_block=0;
	/* Enable the transfer complete interrupt */
    //__HAL_DMA_ENABLE_IT(&hdma_tx, DMA_IT_TC);
    //__HAL_DMA_ENABLE_IT(huart->hdmatx, DMA_IT_TC);
}

/******************************************************************************/
/*Function : setComPortDirect(uint8_t serialNo)*/
/*Brief    : Set the uart directions to transmit mode when the chip is in RS485 mode*/
/*Parameter: serialNo: indicate which serial port we need to control*/
/*retval   : None*/
/******************************************************************************/
void setComPortDirect(uint8_t serialNo){
	if( serialNo == 0 ){
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);   //com_ctr1
	}else if( serialNo ==1 ){
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);   //COM_CTR2
	}else if( serialNo ==2 ){
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);  //COM_CTR3
	}else if( serialNo ==3 ){
	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_6,GPIO_PIN_SET);  //COM_CTR4
	}else{
	  //printf("Error:A valid serial number has transferred!\n\r");
	}	
}
/******************************************************************************/
/*Function : resetComPortDirect(uint8_t serialNo)*/
/*Brief    : Set the uart directions to recieve mode when the chip is in RS485 mode*/
/*Parameter: serialNo: indicate which serial port we need to control*/
/*retval   : None*/
/******************************************************************************/
void resetComPortDirect(uint8_t serialNo){
	if( serialNo == 0 ){
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);   //com_ctr1
	}else if( serialNo ==1 ){
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);   //COM_CTR2
	}else if( serialNo ==2 ){
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);  //COM_CTR3
	}else if( serialNo ==3 ){
	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_6,GPIO_PIN_RESET);  //COM_CTR4
	}else{
	  //printf("Error:A valid serial number has transferred!\n\r");
	}
}