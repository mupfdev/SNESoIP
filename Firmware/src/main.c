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
#include <sys/socket.h>

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
    //InitSNES();

    xTaskCreate(_MainThread, "MainThread", 4096, NULL, 5, NULL);
}

static void _MainThread(void* pArg)
{
    (void)pArg;

    while (1)
    {
        //uint16_t u16InputData    = GetSNESInputData();
        uint16_t u16InputData    = 0xffff;
        uint8_t  u8Index         = 15;
        char     acInputData[17] = { 0 };
        for (uint8_t u8Bit = 0; u8Bit < 16; u8Bit++)
        {
            if ((u16InputData >> u8Bit) & 1)
            {
                acInputData[u8Index] = '1';
            }
            else
            {
                acInputData[u8Index] = '0';
            }
            u8Index--;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
