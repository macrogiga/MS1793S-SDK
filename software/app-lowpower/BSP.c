#include "BSP.h"
#include "mg_api.h"


extern char GetConnectedStatus(void);
unsigned char SleepStatus = 2; //1-sleep, 2-stop, 0-running


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

void Sys_Stop(void)
{
    SCB->SCR |= 0x4;
    __WFI();
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
    
    //no used
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;//下拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化IO
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;//下拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);	//初始化IO
    
    //module led indicator PB0
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_5);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    
    SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource12);
    
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_SetPriority (EXTI0_1_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_SetPriority (EXTI4_15_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
}




/////////////////////Following functions are porting functions/////////////////////////////////


#define TOUT_STDBY 600 //1000/200*60*2 = 2 min, 600
extern unsigned int StandbyTimeout;
void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
}

void IrqMcuGotoSleepAndWakeup(void)
{
    if(ble_run_interrupt_McuCanSleep())
    {
        //to do MCU sleep and wakeup steps
        if (TOUT_STDBY < StandbyTimeout)
        {
            RCC_LSICmd(DISABLE);  //in STANDBY iwdg will cause reset
            
            radio_standby();
            SleepStatus = 2;
            Sys_Stop();
        }else{ //enter STOP to save power
#if 1 //STOP
            SleepStatus = 2;
            SCB->SCR |= 0x4;
            __WFI();
#endif
        }
    }
}

//////DO NOT REMOVE, used in ble lib///////
void SysClk8to48(void)
{
}
void SysClk48to8(void)
{
}

static char dis_int_count = 0;
void DisableEnvINT(void)
{ 
    //to disable int
    __ASM volatile("cpsid i");
    
    dis_int_count ++;
}

void EnableEnvINT(void)
{
    //to enable/recover int
    dis_int_count --;
    if(dis_int_count<=0) //protection purpose
    {
        dis_int_count = 0; //reset
        __ASM volatile("cpsie i");
    }
}

//api provide in blelib
//    EnableLED_Flag; Led_R; Led_G; Led_B; Led_Y; Led_W; Led_Lum_percent; 
void UpdateLEDValueAll(void) //porting function
{
}
