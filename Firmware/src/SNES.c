/**
 * @file       SNES.c
 * @brief      SNES I/O driver
 * @ingroup    SNES I/O
 * @defgroup   SNES I/O
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <string.h>
#include <stdint.h>

#include "SNES.h"

/**
 * @struct  SNESDriver
 * @brief   
*/
typedef struct SNESDriver_t
{
    uint16_t u16Port0;
    uint16_t u16Port1;

} SNESDriver;

/**
 * @var    _stDriver
 * @brief  SNES I/O driver private data
 */
static SNESDriver _stDriver;

int InitSNES(void)
{
    memset(&_stDriver, 0, sizeof(struct SNESDriver_t));

    uint16_t u16Port0 = 0xffff;
    uint16_t u16Port1 = 0xffff;

    return 0;
}

int StartSNES(void)
{
    return 0;
}
