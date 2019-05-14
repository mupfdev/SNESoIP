/**
 * @file       Terminal.h
 * @brief      Terminal interface
 * @details    A terminal interface to access the device via TCP
 * @ingroup    Firmware
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */
#pragma once

#include <stdbool.h>

#ifndef TERMINAL_PORT
#define TERMINAL_PORT 23
#endif

#ifndef VERSION
#define VERSION "SNESoIP Rev. 3\r\n"
#endif

void InitTerminal(void);
void DeInitTerminal(void);
bool IsHostConnected(void);
