/**
**************************************************************************
@File    :  hal\src_code\cgi\src\veth_COM.c
@Author  :  Duke Lee
@Verdion :  V1.0
@Date    :  26-Jan-2015
@brief   :  cgi接口函数：veth_COM.c
**************************************************************************
**/

#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx_hal_conf.h"
#include "veth_common.h"
#include "veth_COM.h"

#define TEST_TIMEOUT

/***********************************************************************/
//函数名  ： comDemo()
//功能    ： web的串口配置项测试样例程序
//参数    ： None
//返回值  ： None
/***********************************************************************/
void comDemo() {
    static uint32_t Com_timeout = 0;
    static uint8_t  change_targ = 0;
    struct serial_config com;
#ifdef MY_DEBUG
    /*COM1 基本参数配置*/
    if(msCount - Com_timeout >= 30000) {
        if(0 == change_targ) {
            com.serial_bound_rate = 9600;
            com.serial_data_bit   = 8;
            com.serial_no         = 3;
            com.serial_parity     = 0;
            com.serial_stop_bit   = 2;
            setComProperty(&com);
            change_targ = 1;
        } else {
            com.serial_bound_rate = 115200;
            com.serial_data_bit   = 8;
            com.serial_no         = 3;
            com.serial_parity     = 0;
            com.serial_stop_bit   = 2;
            setComProperty(&com);
            change_targ = 0;
        }
        Com_timeout = msCount;
    }
#endif
}

/***********************************************************************/
//函数名  ： setComProperty()
//功能    ： web的串口配置项测试样例程序
//参数    ： None
//返回值  ： None
/***********************************************************************/
uint8_t setComProperty(struct serial_config *hcom) {
    UART_HandleTypeDef *huart = NULL;
    /**/
    switch(hcom->serial_no) {
    case 0:
        huart = &Uart6Handle;
        break;
    case 1:
        huart = &Uart5Handle;
        break;
    case 2:
        huart = &Uart7Handle;
        break;
    case 3:
        huart = &Uart4Handle;
        break;
    default :
        break;
    }
    /*Check if the huart exit*/
    if(huart == NULL) {
        return 0;
    }
    if(huart->Init.HwFlowCtl != UART_HWCONTROL_NONE) {
        /* Check the parameters */
        assert_param(IS_UART_HWFLOW_INSTANCE(huart->Instance));
    } else {
        /* Check the parameters */
        assert_param(IS_UART_INSTANCE(huart->Instance));
    }
    /*Get the corresponding parameters for the UART*/
    huart->Init.BaudRate = hcom->serial_bound_rate;
    huart->Init.Parity   = (hcom->serial_parity == 0)? UART_PARITY_NONE:
                           (hcom->serial_parity == 1)? UART_PARITY_EVEN:
                           (hcom->serial_parity == 2)? UART_PARITY_ODD:
                           UART_PARITY_NONE;  //serial_parity : 0:parity_none 1:parity_even 2:parity_odd
    huart->Init.StopBits = (hcom->serial_stop_bit == 1)?UART_STOPBITS_1:
                           UART_STOPBITS_2;
    huart->Init.WordLength = (hcom->serial_data_bit == 8)?UART_WORDLENGTH_8B:
                             UART_WORDLENGTH_9B;
    huart->Init.Mode = UART_MODE_TX | UART_MODE_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->State = HAL_UART_STATE_BUSY;
    /* Disable the peripheral */
    __HAL_UART_DISABLE(huart);
    /* Set the UART Communication parameters */
    setUartConfig(huart);
    //setUartMode(hcom->serial_no);
    /* Enable the peripheral */
    __HAL_UART_ENABLE(huart);
    /*Initialize the UART state*/
    huart->State= HAL_UART_STATE_READY;
    return 1;
}

/***********************************************************************/
//函数名  ： setComMode()
//功能    ： web的串口工作模式
//参数    ： None
//返回值  ： None
/***********************************************************************/
void setComMode(COM_MODE_T *pcom) {
    if( RS232_MODE == pcom->mode_com12 ) {
        setChipMode(0,RS232_MODE);
    }else if( RS485_MODE == pcom->mode_com12 ) {
        setChipMode(0,RS485_MODE);
    }else if( RS422_MODE == pcom->mode_com12 ) {
		setChipMode(0,RS422_MODE);
	}
    if( RS232_MODE == pcom->mode_com34 ) {
        setChipMode(1,RS232_MODE);
    }else if( RS485_MODE == pcom->mode_com34 ){
        setChipMode(1,RS485_MODE);
    }else if(RS422_MODE == pcom->mode_com34){
		setChipMode(1,RS422_MODE);
	}
}
/***********************************************************************/
//函数名  ： setChipMode(uint8_t chipNo，uint8_t mode)
//功能    ： 设置串口芯片的工作模式
//参数    ： None
//返回值  ： None
/***********************************************************************/
void setChipMode(uint8_t chipNo,uint8_t mode) {
    if( 0 == chipNo ) {
        if( RS232_MODE == mode ) {
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);  //mode1_2
            HAL_GPIO_WritePin(GPIOH,GPIO_PIN_7,GPIO_PIN_RESET);  //mode1_1
            HAL_GPIO_WritePin(GPIOH,GPIO_PIN_6,GPIO_PIN_SET);    //mode1_0
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);   //com_ctr1
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);   //COM_CTR2
        }else if( RS485_MODE == mode ) {
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);   //mode1_2
            HAL_GPIO_WritePin(GPIOH,GPIO_PIN_7,GPIO_PIN_SET);   //mode1_1
            HAL_GPIO_WritePin(GPIOH,GPIO_PIN_6,GPIO_PIN_SET); //mode1_0
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);   //com_ctr1
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);   //COM_CTR2
        }else if( RS422_MODE == mode ) {
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);  //mode1_2
            HAL_GPIO_WritePin(GPIOH,GPIO_PIN_7,GPIO_PIN_RESET);  //mode1_1
            HAL_GPIO_WritePin(GPIOH,GPIO_PIN_6,GPIO_PIN_RESET);    //mode1_0
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);   //com_ctr1
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);   //COM_CTR2
		}
    } else { //chipNo == 1
        if( RS232_MODE == mode ) {
            HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,GPIO_PIN_RESET);  //mode1_2
            HAL_GPIO_WritePin(GPIOG,GPIO_PIN_9,GPIO_PIN_RESET);  //mode1_1
            HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12,GPIO_PIN_SET);    //mode1_0
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);  //COM_CTR3
			HAL_GPIO_WritePin(GPIOG,GPIO_PIN_6,GPIO_PIN_RESET);  //COM_CTR4			
        }
        if( RS485_MODE == mode ) {
            HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,GPIO_PIN_SET);  //mode1_2
            HAL_GPIO_WritePin(GPIOG,GPIO_PIN_9,GPIO_PIN_SET);  //mode1_1
            HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12,GPIO_PIN_SET);//mode1_0
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);  //COM_CTR3
			HAL_GPIO_WritePin(GPIOG,GPIO_PIN_6,GPIO_PIN_RESET);  //COM_CTR4
        }else if( RS422_MODE == mode ){
			HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,GPIO_PIN_SET);  //mode1_2
            HAL_GPIO_WritePin(GPIOG,GPIO_PIN_9,GPIO_PIN_RESET);  //mode1_1
            HAL_GPIO_WritePin(GPIOG,GPIO_PIN_12,GPIO_PIN_RESET);//mode1_0
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);  //COM_CTR3
			HAL_GPIO_WritePin(GPIOG,GPIO_PIN_6,GPIO_PIN_RESET);  //COM_CTR4

		
		}
    }
}
