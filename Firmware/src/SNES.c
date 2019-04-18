/**
 * @file       SNES.c
 * @ingroup    SNES I/O
 * @defgroup   SNES I/O
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stdint.h>
#include <SNES.h>

typedef struct SNESDriver_t
{
    uint16_t u16Port0;
    uint16_t u16Port1;
} SNESDriver;

static SNESDriver stDriver = { 0 };

int Init_SNES(void)
{
    uint16_t u16Port0 = 0xffff;
    uint16_t u16Port1 = 0xffff;

    return 0;
}

int Start_SNES(void)
{
    return 0;
}
