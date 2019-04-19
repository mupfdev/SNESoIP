/**
 * @file       main.c
 * @brief      SNESoIP main program
 * @ingroup    SNESoIP
 * @defgroup   SNESoIP
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stdio.h>
#include <sys/socket.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SNES.h"
#include "WiFi.h"

static void _MainThread(void* pArg);

void app_main()
{
    InitWiFi();
    xTaskCreate(&_MainThread, "Main thread", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}

static void _MainThread(void* pArg)
{
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
