#include "BSP.h"
#include "mg_api.h"





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
    
    FLASH->ACR = FLASH_ACR_LATENCY_2 | FLASH_ACR_PRFTBE;
    
    RCC->CFGR &= ~RCC_CFGR_SW;
    
    RCC->CFGR |= 0x02;
    
    while( temp != 0x02 )
    {
        temp = RCC->CFGR>>2;
        temp &= 0x03;
    }
}



/*
GPA6    R
GPA7   G
GPB0   B
GPA1   Y
GPA4   W
*/
void PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE); //AF1,GPA6/7,B0, timer3@Ch1,2,3
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM14, ENABLE);  //AF4,GPA4   timer14@Ch1
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //AF2,GPA1   timer2@Ch2

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE); 
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_4|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource4,GPIO_AF_4);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0,GPIO_AF_1);

    TIM_TimeBaseStructure.TIM_Period = 255*100; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 1K
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //设置用来作为TIMx时钟频率除数的预分频值  480分频 100k
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位


    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);  
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1Init(TIM14, &TIM_OCInitStructure);
    
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); 
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); 
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable); 
    TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_ARRPreloadConfig(TIM14, ENABLE);
    
    TIM_Cmd(TIM2, ENABLE); 
    TIM_Cmd(TIM3, ENABLE); 
    TIM_Cmd(TIM14, ENABLE);  
    
    TIM2->BDTR |= 0x8000;
    TIM3->BDTR |= 0x8000;
    TIM14->BDTR |= 0x8000;
    
    SetLEDLum(0,0,0,100);//上电亮白光
    //SetLEDLum(0,0,0,0);//Yellow
    //SetLEDLum(r,g,b,101);//rgb
    SetLEDLum(0,0,100,101);
}


void BSP_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    SystemClk_HSIInit();
    PWM_Init();
    SysTick_Config(48000);
    
    //SPIM_Init(SPI2,0x06); //8Mhz
    SPIM_Init(SPI2,0x08); //6Mhz


    //IRQ - pa12
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    

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
}


/////////////////////Following functions are porting functions/////////////////////////////////
void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
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
extern void Led_getInfo(unsigned char* data);
void UpdateLEDValueAll(void) //porting function
{
    int t;
    unsigned char data[7];
    unsigned char EnableLED_Flag = 0;
//    unsigned char Led_R;
//    unsigned char Led_G;
//    unsigned char Led_B;
//    unsigned int Led_Y;
//    unsigned int Led_W;
    unsigned int Led_Lum_percent = 100;
        
    Led_getInfo(data);
    EnableLED_Flag = data[0];
    if(EnableLED_Flag == 0)
    {
        TIM_SetCompare1(TIM3,0); //R
        TIM_SetCompare2(TIM3,0); //B
        TIM_SetCompare3(TIM3,0); //G
        TIM_SetCompare1(TIM14,0); //Y
        TIM_SetCompare2(TIM2,0); //W
    }
    else
    {
        Led_Lum_percent = data[6];
        t = data[1] * Led_Lum_percent; TIM_SetCompare1(TIM3,t);
        t = data[2] * Led_Lum_percent; TIM_SetCompare2(TIM3,t);
        t = data[3] * Led_Lum_percent; TIM_SetCompare3(TIM3,t);
        t = data[4] * Led_Lum_percent; TIM_SetCompare1(TIM14,t); 
        t = data[5] * Led_Lum_percent; TIM_SetCompare2(TIM2,t); 
    }
    
}
