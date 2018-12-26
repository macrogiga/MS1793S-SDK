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

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();
    
    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    
    
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
