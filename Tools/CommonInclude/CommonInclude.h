/**
 * @file     CommonInclude.h
 * @brief    SNESoIP common include
 * @ingroup  CommonInclude
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

uint8_t  HexToUint8(uint8_t *pu8Hex);
void     StrToIP(char* pacIp, uint8_t* pu8Dest);
uint32_t StrToUint32(char* pacStr);
bool     IpIsValid(char* pacIn);
