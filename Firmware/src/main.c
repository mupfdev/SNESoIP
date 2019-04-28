/**
 * @file       main.c
 * @brief      SNESoIP main program
 * @ingroup    SNESoIP SNESoIP main
 * @defgroup   SNESoIP SNESoIP main
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SNES.h"
#include "TCPServer.h"
#include "WiFi.h"

static void _MainThread(void* pArg);

void app_main()
{
    InitWiFi();
    WaitForIP();
    InitTCPServer();
    InitSNES();

    xTaskCreate(_MainThread, "MainThread", 4096, NULL, 5, NULL);
}

static void _MainThread(void* pArg)
{
    (void)pArg;

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
