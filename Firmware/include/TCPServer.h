/**
 * @file       TCPServer.h
 * @brief      TCP server
 * @ingroup    Firmware
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */
#pragma once

#include <stdbool.h>

#ifndef TCP_SERVER_PORT
#define TCP_SERVER_PORT 1994
#endif

#ifndef VERSION
#define VERSION "SNESoIP Rev. 3\r\n"
#endif

void InitTCPServer(void);
void DeInitTCPServer(void);
bool IsHostConnected(void);
