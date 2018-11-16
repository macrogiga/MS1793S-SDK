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

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();

#ifndef SWDDEBUG
    Write_Iwdg_ON(IWDG_Prescaler_256, 1000); //6.4s, fff-26.208s max
#endif
    
    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    
    SysTick_Count = 0;
    while(SysTick_Count < 5){}; //delay at least 5ms between radio_initBle() and ble_run...

#if 0 //print Mac
    printf("\r\nMAC:%02x-%02x-%02x-%02x-%02x-%02x", ble_mac_addr[5],ble_mac_addr[4],ble_mac_addr[3],ble_mac_addr[2],ble_mac_addr[1],ble_mac_addr[0]);
#endif

    ble_run_interrupt_start(160*2);  //320*0.625=200 ms
    while(1){
        IrqMcuGotoSleepAndWakeup();
        
        //user proc
    }
}
