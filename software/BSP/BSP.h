#ifndef _BSP_H_
#define _BSP_H_

#include "HAL_conf.h"
#include "spi.h"
#include "I2C.h"


void BSP_Init(void);
void SysClk8to48(void);
void SysClk48to8(void);

extern void mg_activate(unsigned char value);
extern unsigned char mg_writeBuf(unsigned char reg, unsigned char const *pBuf, unsigned char len);

unsigned int GetSysTickCount(void);
void IrqMcuGotoSleepAndWakeup(void);

void LED_ONOFF(unsigned char OnOff);//for ui use


#endif
