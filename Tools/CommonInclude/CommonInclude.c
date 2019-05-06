/**
 * @file      CommonInclude.c
 * @brief     SNESoIP common include
 * @defgroup  CommonInclude SNESoIP common include
 * @ingroup   CommonInclude
 */

#include <stdint.h>
#include <string.h>

/**
 * @fn         uint8_t HexToUint8(uint8_t *s)
 * @brief      Convert hex to uint8_t.
 * @attention  No string validation!
 */
uint8_t HexToUint8(uint8_t *pu8Hex) {
    uint8_t u8Result;

    if (pu8Hex[0] > 0x39)
    {
        pu8Hex[0] -= 7;
    }
    u8Result = (pu8Hex[0] & 0xf) * 16;

    if (pu8Hex[1] > 0x39)
    {
        pu8Hex[1] -= 7;
    }

    u8Result += (pu8Hex[1] & 0xf);
    return u8Result;
}
