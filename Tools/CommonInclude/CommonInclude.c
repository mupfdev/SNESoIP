/**
 * @file      CommonInclude.c
 * @brief     SNESoIP common include
 * @defgroup  CommonInclude SNESoIP common include
 * @ingroup   CommonInclude
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint32_t _StrnToUint32(char* pacStr, int nNum);
static int      _ParseIP(char* pacIn, uint16_t* pu16Out);

/**
 * @fn         uint8_t HexToUint8(uint8_t *s)
 * @brief      Convert hex to uint8_t.
 * @param      pu8Hex
 *             
 * @return     
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

/**
 * @fn         StrToIP(char* pacIp, uint8_t* pu8Dest)
 * @brief      Convert hex to uint8_t.
 * @param      pacIp
 *             
 * @param      pu8Dest
 *             
 * @attention  No string validation!
 */
void StrToIP(char* pacIp, uint8_t* pu8Dest)
{
    uint8_t u8Index = 0;
    char*   pacTemp;

    pacTemp = strtok((char *)pacIp, ".");

    while (! pacTemp)
    {
        pu8Dest[u8Index] = atoi(pacTemp);
        pacTemp          = strtok(NULL, ".");
        u8Index++;
    }
}

/**
 * @fn      uint32_t StrToUint32(char* pacStr)
 * @brief   Convert string to uint32_t.
 * @param   pacStr
 *          
 * @return  
 */
uint32_t StrToUint32(char* pacStr)
{
    char*    pacTemp = pacStr;
    uint32_t u32Num  = 0;

    while (*pacTemp != '\0')
    {
        u32Num++;
        pacTemp++;
    }

    return _StrnToUint32(pacStr, u32Num);
}

/**
 * @fn      bool IpIsValid(char* pacIn)
 * @brief   Check if a string is a valid IP
 * @param   pu8In
 *          
 * @return  
 * @retval  true  = IP is valid
 * @retval  false = IP is invalid
 */
bool IpIsValid(char* pacIn)
{
    uint8_t  u8Pos = 0;
    uint16_t u16Ip = 0;

    for (uint8_t u8Index = 0; pacIn[u8Index] != '\0'; u8Index++)
    {
        if ('.' == pacIn[u8Index])
        {
            u8Pos++;
            if (u8Pos > 3)
            {
                return false;
            }
            if ('.' == pacIn[u8Index + 1])
            {
                return false;
            }
        }
    }

    if (! _ParseIP(pacIn, &u16Ip))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @fn      _StrnToUint32(char* pacStr, int nNum)
 * @brief   
 * @param   pacStr
 *          
 * @param   nNum
 *          
 * @return  
 */
static uint32_t _StrnToUint32(char* pacStr, int nNum)
{
    uint32_t u32Sign  = 1;
    uint32_t u32Place = 1;
    uint32_t u32Ret   = 0;

    for (int nIndex = nNum - 1; nIndex >= 0; nIndex--, u32Place *= 10)
    {
        char c = pacStr[nIndex];
        switch(c)
        {
          case 45:
              if (0 == nIndex)
              {
                  u32Sign = -1;
              }
              else
              {
                  return -1;
              }
              break;
          default:
              if (48 <= c && 57 >= c)
              {
                  u32Ret += (c - 48) * u32Place;
              }
              else
              {
                  return -1;
              }
        }
    }

    return u32Sign * u32Ret;
}

/**
 * @fn      _ParseIP(char* pacIn, uint16_t* pu16Out)
 * @brief   
 * @param   pacIn
 *          
 * @param   pu16Out
 *          
 * @return  
 */
static int _ParseIP(char* pacIn, uint16_t* pu16Out)
{
    uint16_t u16Seg;
    uint16_t u16Exp;

    u16Seg = u16Exp = *pu16Out = 0;

    do
    {
        if ('.' == *pacIn)
        {
            *pu16Out = (*pu16Out << 8) + u16Seg;
            u16Seg = 0;
            u16Exp++;
        }
        else
        {
            u16Seg = 10 * u16Seg + (*pacIn - 0x30);
            if (0x30 > *pacIn || 0x39 < *pacIn || 255 < u16Seg)
            {
                return -1;
            }
        }
    }
    while ('\0' != *++pacIn);

    *pu16Out = (*pu16Out << 8) + u16Seg;

    if (3 != u16Exp)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}
