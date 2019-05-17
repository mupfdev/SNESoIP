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

/**
 * @typedef  eCtrlPort_t
 * @brief    SNES controller ports type
 * @enum     eCtrlPort;
 * @brief    SNES controller ports
 */
typedef enum eCtrlPort_t
{
    PORT_0 = 0,
    PORT_1,
    NUM_PORTS

} eCtrlPort;

/* VSPI (18 SCLK, 19 MISO, 23 MOSI)
 * HSPI (14 SCLK, 12 MISO, 13 MOSI)
 * Although SPI isn't used at this point.
 */
#ifdef USE_SNES_DEFAULT_CONFIG
#define GPIO_SNES_PORT0_CLOCK_BIT  GPIO_SEL_18 // !< Port 0 data clock bitmask
#define GPIO_SNES_PORT0_CLOCK_PIN  GPIO_NUM_18 // !< Port 0 data clock pin
#define GPIO_SNES_PORT0_LATCH_BIT  GPIO_SEL_19 // !< Port 0 data latch bitmask
#define GPIO_SNES_PORT0_LATCH_PIN  GPIO_NUM_19 // !< Port 0 data latch pin
#define GPIO_SNES_PORT0_DATA_BIT   GPIO_SEL_23 // !< Port 0 serial data bitmask
#define GPIO_SNES_PORT0_DATA_PIN   GPIO_NUM_23 // !< Port 0 serial data pin

#define GPIO_SNES_PORT1_CLOCK_BIT  GPIO_SEL_14 // !< Port 1 data clock bitmask
#define GPIO_SNES_PORT1_CLOCK_PIN  GPIO_NUM_14 // !< Port 1 data clock pin
#define GPIO_SNES_PORT1_LATCH_BIT  GPIO_SEL_12 // !< Port 1 data latch bitmask
#define GPIO_SNES_PORT1_LATCH_PIN  GPIO_NUM_12 // !< Port 1 data latch pin
#define GPIO_SNES_PORT1_DATA_BIT   GPIO_SEL_14 // !< Port 1 serial data bitmask
#define GPIO_SNES_PORT1_DATA_PIN   GPIO_NUM_14 // !< Port 1 serial data pin

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
void     UpdateData(eCtrlPort ePort);
void     SendClock(void);
void     SendLatch(void);
