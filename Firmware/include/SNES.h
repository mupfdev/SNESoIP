/**
 * @file       SNES.h
 * @brief      SNES I/O driver
 * @ingroup    SNESIO SNES I/O driver
 * @defgroup   SNESIO SNES I/O driver
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 * @todo       Adjust default pin configuration
 */
#pragma once

#include "driver/gpio.h"

#ifdef USE_SNES_DEFAULT_CONFIG
#define GPIO_SNES_PORT1_CLOCK  GPIO_NUM_27 // !< Port 1 data clock
#define GPIO_SNES_PORT1_LATCH  GPIO_NUM_25 // !< Port 1 data latch
#define GPIO_SNES_PORT1_DATA   GPIO_NUM_32 // !< Port 1 serial data
#define GPIO_SNES_PORT2_CLOCK  GPIO_NUM_22 // !< Port 2 data clock
#define GPIO_SNES_PORT2_LATCH  GPIO_NUM_21 // !< Port 2 data latch
#define GPIO_SNES_PORT2_DATA   GPIO_NUM_17 // !< Port 2 serial data
#define GPIO_SNES_INPUT_CLOCK  GPIO_NUM_26 // !< Input data clock
#define GPIO_SNES_INPUT_LATCH  GPIO_NUM_18 // !< Input data latch
#define GPIO_SNES_INPUT_DATA   GPIO_NUM_19 // !< Input serial data
#define GPIO_SNES_IOPORT_BIT6  GPIO_NUM_23 // !< I/O port bit 6
#define GPIO_SNES_IOPORT_BIT7  GPIO_NUM_5  // !< I/O port bit 7
#endif

void     InitSNES(void);
void     DeInitSNES(void);
uint16_t GetSNESInputData(void);
