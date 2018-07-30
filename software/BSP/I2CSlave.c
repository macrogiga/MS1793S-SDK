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
#include "I2C.h"

void I2CInitSlaveMode(I2C_TypeDef *I2Cx,unsigned char TarAddress) 
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    if(I2Cx==I2C1)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);  //i2c1 clk enable

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7;   //i2c1_scl  pb6  i2c1_sda  pb7
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_1);
        GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_1);
    }

    I2Cx->IC_ENABLE = 0x0;
    I2Cx->IC_CON = 0x122;
    I2Cx->IC_SAR = TarAddress>>1;//Address
    I2Cx->IC_SS_SCL_HCNT = 0x77;
    I2Cx->IC_SS_SCL_LCNT = 0x55;
    I2Cx->IC_RX_TL=0x0;//set Rx FIFO threshold level 
    I2Cx->IC_TX_TL=0x0;//set Tx FIFO threschold level 
    I2Cx->IC_INTR_MASK=0X24;//disable all inttrupt
    I2Cx->IC_ENABLE=0x1;

    //I2C NVIC
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
