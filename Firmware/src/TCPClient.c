/**
 * @file       TCPClient.c
 * @brief      TCP client
 * @details    A TCP client to communicate with the main server
 * @ingroup    Firmware
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "TCPClient.h"

/**
 * @struct  TCPClient
 * @brief   TCP client data
 */
typedef struct TCPClient_t
{
    bool    bIsRunning;
    uint8_t u8Stage;
    uint8_t u8ClientID;
    uint8_t u8OpponentID;
    uint8_t u8IpAddr[4];

} TCPClient;

/**
 * @var    _stClient
 * @brief  TCP client private data
 */
static TCPClient _stClient;

static void _TCPClientThread(void* pArg);

/**
 * @fn     void InitTCPClient(void)
 * @brief  Initialise TCP client
 */
void InitTCPClient(void)
{
    ESP_LOGI("tcpc", "Initialise TCP client.");
    memset(&_stClient, 0, sizeof(struct TCPClient_t));
    xTaskCreate(_TCPClientThread, "TCPClientThread", 4096, NULL, 3, NULL);
}

/**
 * @fn     void _TCPClientThread(void* pArg)
 * @brief  TCP client
 * @param  pArg
 *         Unused
 */
static void _TCPClientThread(void* pArg)
{
    struct sockaddr_in stDestAddr;

    char acRxBuffer[5];
    char acAddrStr[128];
    int  nAddrFamily;
    int  nIPProtocol;
    int  nSock;
    int  nErr;

    stDestAddr.sin_addr.s_addr =  inet_addr("10.0.0.3");
    stDestAddr.sin_family      = AF_INET;
    stDestAddr.sin_port        = htons(1995);
    nAddrFamily                = AF_INET;
    nIPProtocol                = IPPROTO_IP;
    inet_ntoa_r(stDestAddr.sin_addr, acAddrStr, sizeof(acAddrStr) - 1);

    nSock = socket(nAddrFamily, SOCK_STREAM, nIPProtocol);
    if (0 > nSock)
    {
        ESP_LOGE("tcpc", "Unable to create socket: errno %d", errno);
    }
    else
    {
        ESP_LOGI("tcpc", "Socket created successfully.");
    }

    nErr = connect(nSock, (struct sockaddr *)&stDestAddr, sizeof(stDestAddr));
    if (0 != nErr)
    {
        ESP_LOGE("tcpc", "Unable to connect: errno %d", errno);
    }
    else
    {
        ESP_LOGI("tcpc", "Successfully connected");
    }

    _stClient.bIsRunning = true;
    while (_stClient.bIsRunning)
    {
        int  nLen         = 0;
        char acCommand[5] = { 0 };

        if (1 == _stClient.u8Stage)
        {
            memcpy(acCommand, "GetIP", 5);
            nErr = send(nSock, acCommand, 5, 0);
            if (0 > nErr)
            {
                ESP_LOGE("tcpc", "Error occured during sending: errno %d", errno);
                continue;
            }
            // DEBUG
            ESP_LOGI("tcpc", "Disconnecting.");
            _stClient.bIsRunning = false;
            continue;
        }

        nLen = recv(nSock, acRxBuffer, sizeof(acRxBuffer) - 1, 0);
        if (0 > nLen)
        {
            ESP_LOGE("tcpc", "Receive failed: errno %d", errno);
        }
        else
        {
            if (0 == _stClient.u8Stage)
            {
                char acCommand[4] = { 'H', 'e', 'l', 'o' };
                if (0 == memcmp(acCommand, &acRxBuffer, 4))
                {
                    ESP_LOGI("tcpc", "Helo received.");
                    _stClient.u8ClientID = acRxBuffer[4];
                    _stClient.u8Stage    = 1;
                    continue;
                }
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI("tcpc", "Shutting down socket.");
    shutdown(nSock, 0);
    close(nSock);

    vTaskDelete(NULL);
}
