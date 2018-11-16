#include "BSP.h"
#include "mg_api.h"





extern volatile unsigned int SysTick_Count;
extern unsigned int RxTimeout;
extern unsigned int TxTimeout;

unsigned char SleepStop = 1; //01-sleep, 02-stop
unsigned char SleepStatus = 0;

/********************************************************************************************************
**函数信息 ：SystemClk_HSIInit (void)                        
**功能描述 ：系统时钟初始化函数，初始化之前先复位所有时钟
**输入参数 ：无
**输出参数 ：无
********************************************************************************************************/
void SystemClk_HSIInit(void)
{//HSI=48M
    unsigned char temp = 0;
    
    RCC->CR |= RCC_CR_HSION;
    while( !(RCC->CR&RCC_CR_HSIRDY) );
    
    RCC->CR &= ~(1<<20);
    
    RCC->CR &= ~(1<<2);
    
    RCC->CFGR = RCC_CFGR_PPRE1_1;
    
    FLASH->ACR = FLASH_ACR_LATENCY_1 | FLASH_ACR_PRFTBE;
    
    RCC->CFGR &= ~RCC_CFGR_SW;
    
    RCC->CFGR |= 0x02;
    
    while( temp != 0x02 )
    {
        temp = RCC->CFGR>>2;
        temp &= 0x03;
    }
}


void LED_ONOFF(unsigned char onFlag)//module indicator,GPB0
{
    if(onFlag){
        GPIOB->BRR = GPIO_Pin_0;  //low, on
    }else{
        GPIOB->BSRR = GPIO_Pin_0; //high, off
    }
}


void BSP_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    SystemClk_HSIInit();
    SysTick_Config(48000);
    
    //SPIM_Init(SPI2,0x06); //8Mhz
    SPIM_Init(SPI2,0x08); //6Mhz


    //IRQ - pa12
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;    //PA.0
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;//下拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化IO
    
    //module led indicator PB0
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_5);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource12);
    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_SetPriority (EXTI4_15_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
    
#ifdef USE_UART
    UartInit(UART1, 115200);
    NVIC_SetPriority (UART1_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
    
#ifndef SWDDEBUG
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource13,GPIO_AF_5);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource14,GPIO_AF_5);
    //RTS
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //CTS
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_13);
#endif
#endif
}


/////////////////////Following functions are porting functions/////////////////////////////////
void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
#ifdef USE_UART
    if ((SleepStop)&&
        (TxTimeout < SysTick_Count)&&
        (RxTimeout < SysTick_Count))
    {
        if(SleepStop == 1){//sleep
            SCB->SCR &= 0xfb;
            __WFE();
        }else{ //stop
            SCB->SCR |= 0x4;
            __WFI();
            
            RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
#ifndef SWDDEBUG
            GPIO_ResetBits(GPIOA, GPIO_Pin_13);
#endif
        }
    }
#endif
}
void IrqMcuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
    if(ble_run_interrupt_McuCanSleep() == 0) return;

#ifdef USE_UART
    if ((SleepStop)&&
        (TxTimeout < SysTick_Count)&&
        (RxTimeout < SysTick_Count))
    {
        if(SleepStop == 1){//sleep
            SleepStatus = 1;
            SCB->SCR &= 0xFB;
            __WFE();
        }else{ //stop
            SleepStatus = 2;
            SCB->SCR |= 0x4;
            __WFI();
#ifndef SWDDEBUG
            GPIO_ResetBits(GPIOA, GPIO_Pin_13);
#endif
        }
    }
#endif
}

//////DO NOT REMOVE, used in ble lib///////
void SysClk8to48(void)
{
}
void SysClk48to8(void)
{
}

void DisableEnvINT(void)
{
}

void EnableEnvINT(void)
{
}

//api provide in blelib
//    EnableLED_Flag; Led_R; Led_G; Led_B; Led_Y; Led_W; Led_Lum_percent; 
void UpdateLEDValueAll(void) //porting function
{
}
