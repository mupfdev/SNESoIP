/**
 * @file       WiFi.h
 * @brief      WiFi driver
 * @details    A driver to set up the WiFi connection
 * @ingroup    Firmware
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */
#pragma once

void InitWiFi(void);
void WaitForIP(void);
