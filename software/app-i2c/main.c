#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"
#include "I2C.h"
#include "oled.h"


extern volatile unsigned int SysTick_Count;

unsigned char *ble_mac_addr;

#ifdef I2CMASTER
unsigned char DispLen=0,DispUpdate=0,DispStr[21];
#endif

int main(void)
{
    unsigned long temp=0x200000;
    unsigned char *ft_val1 = (unsigned char *)(0x1FFFF804);
    unsigned char ft_value[2] = {0xc0, 0x12};
    
    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();
    
    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    
    if((*ft_val1 > 11) && (*ft_val1 < 27)){
        ft_value[1] = *ft_val1;
        mg_activate(0x53);
        mg_writeBuf(0x4, ft_value, 2);
        mg_activate(0x56);
    }
    
    ble_run_interrupt_start(160*2); //320*0.625=200 ms
    
    while(1)
    {
        IrqMcuGotoSleepAndWakeup();
        
#ifdef I2CMASTER
        //user proc
        if (0xff == DispUpdate)
        {
            DispUpdate = 0;
            OLED_Clear();
        }
        else if (1 == DispUpdate)
        {
            DispUpdate = 0;
            OLED_DispStr(0, DispLen, (char*)DispStr);
        }
#endif
    }
}
