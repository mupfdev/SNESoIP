/**
 * @file       SNES.h
 * @brief      SNES I/O driver
 * @ingroup    Firmware
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 * @todo       Adjust default pin configuration
 */
#pragma once

#include "driver/gpio.h"

#ifdef USE_SNES_DEFAULT_CONFIG
#define GPIO_SNES_INPUT_CLOCK_BIT  GPIO_SEL_25 // !< Input data clock bitmask
#define GPIO_SNES_INPUT_CLOCK_PIN  GPIO_NUM_25 // !< Input data clock pin
#define GPIO_SNES_INPUT_LATCH_BIT  GPIO_SEL_26 // !< Input data latch bitmask
#define GPIO_SNES_INPUT_LATCH_PIN  GPIO_NUM_26 // !< Input data latch pin
#define GPIO_SNES_INPUT_DATA_BIT   GPIO_SEL_27 // !< Input serial data bitmask
#define GPIO_SNES_INPUT_DATA_PIN   GPIO_NUM_27 // !< Input serial data pin
#endif

void     InitSNES(void);
void     DeInitSNES(void);
uint16_t GetSNESInputData(void);
void     SendClock(void);
void     SendLatch(void);
