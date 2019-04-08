#include <string.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "iwdg.h"
#include "mg_api.h"


/*
PB6 - UART_TX
PB7 - UART_RX
PA13 - UART_RTS
PA14 - UART_CTS
*/
/************If debug, please add macro SWDDEBUG to disable PA13/14 MUX
in menu Project->Options for Target...->C/C++->Processor Symbol->define**********/

extern volatile unsigned int SysTick_Count;

unsigned char *ble_mac_addr;



int main(void)
{
    unsigned long temp=0x200000;
    unsigned char *ft_val1 = (unsigned char *)(0x1FFFF804);
    unsigned char ft_value[2] = {0xc0, 0x12};

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();

#ifndef SWDDEBUG
    Write_Iwdg_ON(IWDG_Prescaler_256, 1000); //6.4s, fff-26.208s max
#endif
    
    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);

    if((*ft_val1 > 11) && (*ft_val1 < 27)){
        ft_value[1] = *ft_val1;
        mg_activate(0x53);
        mg_writeBuf(0x4, ft_value, 2);
        mg_activate(0x56);
    }

#if 0 //print Mac
    printf("\r\nMAC:%02x-%02x-%02x-%02x-%02x-%02x", ble_mac_addr[5],ble_mac_addr[4],ble_mac_addr[3],ble_mac_addr[2],ble_mac_addr[1],ble_mac_addr[0]);
#endif

    ble_run_interrupt_start(160*2);  //320*0.625=200 ms
    while(1){
        IrqMcuGotoSleepAndWakeup();
        
        //user proc
    }
}
