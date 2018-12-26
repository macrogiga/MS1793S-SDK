#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"



extern volatile unsigned int SysTick_Count;

unsigned char *ble_mac_addr;


unsigned char pld_adv[] = {2,1,6, 13,0x09,'M','G','B','D','1','7','2','5','A','8','9','F', 9,0xff,0x11,0x02,0x01,0x08,0x9F,0xA8,0x25,0x17};//max 31 Byte

int main(void)
{
    unsigned long temp=0x200000;

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();

    Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    //pld_adv[9]
    ble_set_adv_data(pld_adv, sizeof(pld_adv)); 
    //ble_set_adv_type(2);
    SysTick_Count = 0;
    while(SysTick_Count < 5){}; //delay at least 5ms between radio_initBle() and ble_run...
    
    ble_run_interrupt_start(160); //160*0.625=100 ms
    
    while(1)
    {
        //key proc
    }
}
