#ifndef _I2C_H_
#define _I2C_H_

#include "HAL_conf.h"


void I2CInitMasterMode(I2C_TypeDef *I2Cx,unsigned int uiI2C_speed,unsigned char TarAddress);
void I2CInitSlaveMode(I2C_TypeDef *I2Cx,unsigned char TarAddress);
void I2C_Send(u8 *pdat, u8 len);

#endif
