/**
 * @file       SNES.c
 * @brief      SNES I/O driver
 * @ingroup    SNESIO SNES I/O driver
 * @defgroup   SNESIO SNES I/O driver
 * @details
 * @code{.unparsed}
 *
 * Wiring diagram (default configuration):
 *   +-------------------+--------------+,
 *   |                   |                \
 *   | [VCC] [2] [3] [4] | [NC] [6] [GND] |
 *   |                   |                /
 *   +-------------------+--------------+'
 *
 * Important: The logic levels need to be converted using a
 * bidirectional logic level shifter such as the TXS0108E by Texas
 * Instruments.
 *
 *   +-------------+-----+--------- -+-------+-------+
 *   | Location    | Pin | Desc.     | Conn. | ESP32 |
 *   +-------------+-----+-----------+-------+-------+
 *   | SNES Port1  |  1  | +5V       |  +5V  |       |
 *   | SNES Port1  |  2  | Clock     | LShft |       |
 *   | SNES Port1  |  3  | Latch     | LShft |       |
 *   | SNES Port1  |  4  | Data      | LShft |       |
 *   | SNES Port1  |  6  | IOPort 6  | LShft |       |
 *   +-------------+-----+-----------+-------+-------+
 *   | SNES Port2  |  2  | Clock     | LShft |       |
 *   | SNES Port2  |  3  | Latch     | LShft |       |
 *   | SNES Port2  |  4  | Data      | LShft |       |
 *   | SNES Port2  |  6  | IOPort 7  | LShft |       |
 *   | SNES Port2  |  7  | GND       |  GND  |       |
 *   +-------------+-----+-----------+-------+-------+
 *   | SNES Input  |  1  | +5V       |  +5V  |       |
 *   | SNES Input  |  2  | Clock     | LShft |       |
 *   | SNES Input  |  3  | Latch     | LShft |       |
 *   | SNES Input  |  4  | Data      | LShft |       |
 *   | SNES Input  |  7  | GND       |  GND  |       |
 *   +-------------+-----+-----------+-------+-------+
 *
 * The IOPort in the wiring diagram above can be accessed trough bit 6
 * and 5 of the Joypad Programmable I/O Port.
 *
 * The SNESoIP uses the SNES controller ports as a power supply and
 * because VCC and GND is connected on both sides, can you save at least
 * one pin on each cable.  These pins can be used to gain access to the
 * usually unconnected pin 6 used by the IOPort.
 *
 * The SNESoIP uses the IOPort to establish bidirectional
 * communication.
 *
 * But because SNES controller connectors aren't designed to be reopened
 * again, I wrote some simple instructions how they can be modified to
 * fit the requirements.  See the main page of the documentation for
 * details.
 *
 * 4201h WRIO (Open-Collector Output) (W)
 *
 *   IOPort6 Port1 Pin 6
 *   IOPort7 Port2 Pin 6
 *
 *   Note: Due to the weak high-level, the raising "edge" is raising
 *   rather slowly, for sharper transitions one may need external
 *   pull-up resistors. Source: fullsnes by nocash
 *
 * 4213h RDIO (Input) (R)
 *
 *   When used as Input via 4213h, set the corresponding bits in 4201h
 *   to high.
 *
 * @endcode
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 * @todo       Determine if the IOPort needs to be connected to the
 *             logic level shifter due it's weak high-level.
 */

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SNES.h"

/**
 * @struct  SNESDriver
 * @brief   SNES I/O driver data
 */
typedef struct SNESDriver_t
{
    uint16_t u16Port1;
    uint16_t u16Port2;
    bool     bIOPortBit6;
    bool     bIOPortBit7;

} SNESDriver;

/**
 * @var    _stDriver
 * @brief  SNES I/O driver private data
 */
static SNESDriver _stDriver;

static void _SNESThread(void* pArg);

int InitSNES(void)
{
    memset(&_stDriver, 0, sizeof(struct SNESDriver_t));

    uint16_t u16Port1 = 0xffff;
    uint16_t u16Port2 = 0xffff;

    xTaskCreate(_SNESThread, "SNESThread", 4096, NULL, 2 | portPRIVILEGE_BIT, NULL);

    return 0;
}

int StartSNES(void)
{
    return 0;
}

static void _SNESThread(void* pArg)
{
    (void)pArg;
    vTaskDelete(NULL);
}
