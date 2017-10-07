#ifndef I2C_H
#define I2C_H

#ifdef STM32L0
#include "stm32l0xx.h"
#else
#include "stm32f0xx.h"
#endif


void SetupI2cMaster(I2C_TypeDef *argI2c);
int I2cMasterBlockingRead(uint8_t nbytes, uint16_t address, uint8_t *dataptr);

#endif
