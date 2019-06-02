/**
 * @file       Firmware.c
 * @brief      SNESoIP firmware
 * @defgroup   Firmware SNESoIP firmware
 * @ingroup    Firmware
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ExchangeClient.h"
//#include "IRC.h"
#include "SNES.h"
#include "Terminal.h"
#include "WiFi.h"

static void _MainThread(void* pArg);

void app_main()
{
    InitSNES();
    InitWiFi();
    WaitForIP();
    InitTerminal();
    //InitIRC();
    InitExchangeClient();

    xTaskCreate(_MainThread, "MainThread", 1024, NULL, 5, NULL);
}

static void _MainThread(void* pArg)
{
    (void)pArg;

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
