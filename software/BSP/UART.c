/*
    Copyright (c) 2018 Macrogiga Electronics Co., Ltd.

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.
*/
#include "HAL_conf.h"


u32 BaudRate = 115200;

/********************************************************************************************************
**函数信息 ：UartInit(UART_TypeDef* UARTx, u32 baudrate)                      
**功能描述 ：初始化串口
**输入参数 ：UART_TypeDef* UARTx ，选择UART1、UART2、UART3
**输出参数 ：无
********************************************************************************************************/
void UartInit(UART_TypeDef* UARTx, u32 baudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    if(UART1 == UARTx){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);
    
        NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_0);
        GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_0);

        UART_InitStructure.UART_BaudRate = baudRate;
        UART_InitStructure.UART_WordLength = UART_WordLength_8b;
        UART_InitStructure.UART_StopBits = UART_StopBits_1;
        UART_InitStructure.UART_Parity = UART_Parity_No;
        UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
        UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

        UART_Init(UART1, &UART_InitStructure);
        UART_ITConfig(UART1, UART_IT_RXIEN, ENABLE);
        UART_ITConfig(UART1, UART_IT_TXIEN, DISABLE);
        UART_Cmd(UART1, ENABLE);

        //UART1_TX  
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        //UART1_RX
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        UART_ITConfig(UART1, UART_IT_RXIEN, ENABLE);
        
        NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

    }
    else if(UART2 == UARTx)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART2, ENABLE);

        GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_1);
        GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_1);
        GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1);
        GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1);

        NVIC_InitStructure.NVIC_IRQChannel = UART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        NVIC_SetPriority (UART2_IRQn, (1<<__NVIC_PRIO_BITS) - 2);

        UART_InitStructure.UART_BaudRate = baudRate;
        UART_InitStructure.UART_WordLength = UART_WordLength_8b;
        UART_InitStructure.UART_StopBits = UART_StopBits_1;
        UART_InitStructure.UART_Parity = UART_Parity_No;
        UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
        UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

        UART_Init(UART2, &UART_InitStructure);
        UART_ITConfig(UART2, UART_IT_RXIEN, ENABLE);
        UART_Cmd(UART2, ENABLE);

    }

}

void ChangeBaudRate(void)
{
    UART_Cmd(UART1, DISABLE);
    UartInit(UART1, BaudRate); //初始化串口1
}
