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

#ifdef USE_SNES_DEFAULT_CONFIG
#define SNES_PORT1_CLOCK  0
#define SNES_PORT1_LATCH  1
#define SNES_PORT1_DATA   2
#define SNES_PORT2_CLOCK  3
#define SNES_PORT2_LATCH  4
#define SNES_PORT2_DATA   5
#define SNES_INPUT_CLOCK  6
#define SNES_INPUT_LATCH  7
#define SNES_INPUT_DATA   8
#define SNES_IOPORT_BIT6  9
#define SNES_IOPORT_BIT7  10
#endif

int InitSNES(void);
int StartSNES(void);
