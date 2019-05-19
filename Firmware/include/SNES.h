/**
 * @file       SNES.h
 * @brief      SNES I/O driver
 * @details    A driver to interact with the console
 * @ingroup    Firmware
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 * @todo       Adjust default pin configuration
 */
#pragma once

#include "driver/gpio.h"

#ifdef USE_SNES_DEFAULT_CONFIG

/*
 * HSPI
 *   CS0  15
 *   SCLK 14
 *   MISO 12
 */
#define SNES_PORT0_CLOCK_BIT  GPIO_SEL_14 // !< Port 0 data clock bitmask
#define SNES_PORT0_CLOCK_PIN  GPIO_NUM_14 // !< Port 0 data clock pin
#define SNES_PORT0_LATCH_BIT  GPIO_SEL_15 // !< Port 0 data latch bitmask
#define SNES_PORT0_LATCH_PIN  GPIO_NUM_15 // !< Port 0 data latch pin
#define SNES_PORT0_DATA_BIT   GPIO_SEL_12 // !< Port 0 serial data bitmask
#define SNES_PORT0_DATA_PIN   GPIO_NUM_12 // !< Port 0 serial data pin

/*
 * VSPI
 *   CS0   5
 *   SCLK 18
 *   MISO 19
 */
#define SNES_PORT1_CLOCK_BIT  GPIO_SEL_18 // !< Port 1 data clock bitmask
#define SNES_PORT1_CLOCK_PIN  GPIO_NUM_18 // !< Port 1 data clock pin
#define SNES_PORT1_LATCH_BIT  GPIO_SEL_5  // !< Port 1 data latch bitmask
#define SNES_PORT1_LATCH_PIN  GPIO_NUM_5  // !< Port 1 data latch pin
#define SNES_PORT1_DATA_BIT   GPIO_SEL_19 // !< Port 1 serial data bitmask
#define SNES_PORT1_DATA_PIN   GPIO_NUM_19 // !< Port 1 serial data pin

#define SNES_INPUT_CLOCK_BIT  GPIO_SEL_25 // !< Input data clock bitmask
#define SNES_INPUT_CLOCK_PIN  GPIO_NUM_25 // !< Input data clock pin
#define SNES_INPUT_LATCH_BIT  GPIO_SEL_26 // !< Input data latch bitmask
#define SNES_INPUT_LATCH_PIN  GPIO_NUM_26 // !< Input data latch pin
#define SNES_INPUT_DATA_BIT   GPIO_SEL_27 // !< Input serial data bitmask
#define SNES_INPUT_DATA_PIN   GPIO_NUM_27 // !< Input serial data pin
#endif

void     InitSNES(void);
void     DeInitSNES(void);
uint16_t GetSNESInputData(void);
void     SendClock(void);
void     SendLatch(void);
