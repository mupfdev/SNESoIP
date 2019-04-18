/**
 * @file       main.c
 * @ingroup    SNESoIP
 * @defgroup   SNESoIP
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stdio.h>
#include <sys/socket.h>

#include "main.h"
#include "SNES.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "sdkconfig.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

static EventGroupHandle_t _hWiFi_EventGroup;
/* The event group allows multiple bits for each event, but we only care
   about one event - are we connected to the AP with an IP? */
const static int _cnConnectedBit = BIT0;

static void      _MainThread(void* pArg);
static void      _Init_WiFi(void);
static esp_err_t _WiFi_EventHandler(void* pCTX, system_event_t* stEvent);

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    _Init_WiFi();
    xTaskCreate(&_MainThread, "Main thread", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}

static void _MainThread(void* pArg)
{
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void _Init_WiFi(void)
{
    tcpip_adapter_init();
    _hWiFi_EventGroup = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_loop_init(_WiFi_EventHandler, NULL));
    wifi_init_config_t stCFG = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&stCFG));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t stWiFiConfig =
    {
        .sta =
        {
            .ssid     = "foobar",
            .password = "123456",
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &stWiFiConfig));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static esp_err_t _WiFi_EventHandler(void* pCTX, system_event_t* stEvent)
{
    (void)pCTX;

    switch (stEvent->event_id)
    {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(_hWiFi_EventGroup, _cnConnectedBit);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            /* This is a workaround as ESP32 WiFi libs don't currently
             auto-reassociate. */
            esp_wifi_connect();
            xEventGroupClearBits(_hWiFi_EventGroup, _cnConnectedBit);
            break;
        default:
            break;
    }

    return ESP_OK;
}
