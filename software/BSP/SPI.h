#ifndef __SPI_H__
#define __SPI_H__

#include "HAL_conf.h"



void SPIM_Init(SPI_TypeDef* SPIx, unsigned short spi_baud_div);

#endif
