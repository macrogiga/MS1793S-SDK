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


unsigned char *ble_mac_addr;

//const unsigned char AdvDat_Uart[21]=
//{
//    0x02,0x01,0x06,
//    0x11,0x07,0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x01,0x00,0x40,0x6e
//};

static u8 dev_adva[6] = {0x40, 0xf8, 0x28, 0x17, 0x67, 0xED};  //bluetooth address of peripheral

int main(void)
{
    unsigned long temp=0x200000;
    unsigned char *ft_val1 = (unsigned char *)(0x1FFFF804);
    unsigned char ft_value[2] = {0xc0, 0x12};
    unsigned char res = 0;

    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.
    
    BSP_Init();

#ifndef SWDDEBUG
    Write_Iwdg_ON(IWDG_Prescaler_256, 1000); //6.4s, fff-26.208s max
#endif
    
    SetBleIntRunningMode();
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
    
#ifdef CENTRAL
    res = mconn_SetConnTimeout(150); //1s
    res = mconn_SetConnInterval(16); //20ms
    //ble_set_adv_enableFlag(3);///
    ble_master_setTargetDeviceAddr(dev_adva);
#endif
    //ble_set_adv_data((unsigned char *)AdvDat_Uart, sizeof(AdvDat_Uart));
    
    if((*ft_val1 > 11) && (*ft_val1 < 28)){
        ft_value[1] = *ft_val1;
        mg_activate(0x53);
        mg_writeBuf(0x4, ft_value, 2);
        mg_activate(0x56);
    }

#if 1 //print Mac
    printf("\r\nMAC:%02x-%02x-%02x-%02x-%02x-%02x", ble_mac_addr[5],ble_mac_addr[4],ble_mac_addr[3],ble_mac_addr[2],ble_mac_addr[1],ble_mac_addr[0]);
#endif

    ble_run_interrupt_start(24);  //15 ms
    while(1){
        IrqMcuGotoSleepAndWakeup();
        
        //user proc
    }
}
