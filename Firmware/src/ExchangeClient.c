/**
 * @file       ExchangeClient.c
 * @brief      IP exchange client
 * @details    A TCP client to interact with the IP exchange server
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
#include "lwip/netdb.h"
#include "ExchangeClient.h"

/**
 * @struct  ExchangeClient
 * @brief   ExchangeClient data
 */
typedef struct ExchangeClient_t
{
    bool    bIsRunning;
    uint8_t u8Stage;
    uint8_t u8ClientID;
    uint8_t u8OpponentID;
    uint8_t u8IpAddr[4];

} ExchangeClient;

/**
 * @var    _stExchangeClient
 * @brief  IP exchange client private data
 */
static ExchangeClient _stExchangeClient;

static void _ExchangeClientThread(void* pArg);

/**
 * @fn     void InitExchangeClient(void)
 * @brief  Initialise IP exchange client
 */
void InitExchangeClient(void)
{
    ESP_LOGI("ExchangeClient", "Initialise IP exchange client.");
    memset(&_stExchangeClient, 0, sizeof(struct ExchangeClient_t));
    xTaskCreate(_ExchangeClientThread, "ExchangeClientThread", 4096, NULL, 3, NULL);
}

/**
 * @fn     void _ExchangeClientThread(void* pArg)
 * @brief  IP exchange client thread
 * @param  pArg
 *         Unused
 */
static void _ExchangeClientThread(void* pArg)
{
    struct sockaddr_in stDestAddr;

    char acRxBuffer[11];
    char acAddrStr[128];
    int  nAddrFamily;
    int  nIPProtocol;
    int  nSock;
    int  nErr;

    stDestAddr.sin_addr.s_addr = inet_addr("10.0.0.3");
    stDestAddr.sin_family      = AF_INET;
    stDestAddr.sin_port        = htons(54350);
    nAddrFamily                = AF_INET;
    nIPProtocol                = IPPROTO_IP;
    inet_ntoa_r(stDestAddr.sin_addr, acAddrStr, sizeof(acAddrStr) - 1);

    nSock = socket(nAddrFamily, SOCK_STREAM, nIPProtocol);
    if (0 > nSock)
    {
        ESP_LOGE("ExchangeClient", "Unable to create socket: errno %d", errno);
    }
    else
    {
        ESP_LOGI("ExchangeClient", "Socket created successfully.");
    }

    nErr = connect(nSock, (struct sockaddr *)&stDestAddr, sizeof(stDestAddr));
    if (0 != nErr)
    {
        ESP_LOGE("ExchangeClient", "Unable to connect: errno %d", errno);
    }
    else
    {
        ESP_LOGI("ExchangeClient", "Successfully connected");
    }

    _stExchangeClient.bIsRunning = true;
    while (_stExchangeClient.bIsRunning)
    {
        int  nLen;
        char acCommand[7] = { 0 };

        if (1 == _stExchangeClient.u8Stage)
        {
            ESP_LOGI("ExchangeClient", "Requesting IP address");
            memcpy(acCommand, "GetIP\r\n", 7);
            nErr = send(nSock, acCommand, 7, 0);
        }
        if (2 == _stExchangeClient.u8Stage)
        {
            memcpy(acCommand, "Bye\r\n", 5);
            nErr = send(nSock, acCommand, 5, 0);
        }
        if (0 > nErr)
        {
            ESP_LOGE("ExchangeClient", "Error occured during sending: errno %d", errno);
            continue;
        }

        nLen = recv(nSock, acRxBuffer, sizeof(acRxBuffer) - 1, MSG_DONTWAIT);
        if (0 > nLen)
        {
            if (EAGAIN != errno)
            {
                ESP_LOGE("Client", "Receive failed: errno %d", errno);
            }
        }
        else
        {
            if (0 == _stExchangeClient.u8Stage)
            {
                char acCommand[7] = { 'H', 'e', 'l', 'l', 'o' };
                if (0 == memcmp(acCommand, &acRxBuffer, 5))
                {
                    ESP_LOGI("ExchangeClient", "Client ID received: %d", acRxBuffer[5]);
                    _stExchangeClient.u8ClientID = acRxBuffer[5];
                    _stExchangeClient.u8Stage    = 1;
                }
            }
            else if (1 == _stExchangeClient.u8Stage)
            {
                char acCommand[4] = { 'I', 'P', 'O', 'K' };
                if (0 == memcmp(acCommand, &acRxBuffer, 4))
                {
                    _stExchangeClient.u8OpponentID = acRxBuffer[4];
                    _stExchangeClient.u8IpAddr[0]  = acRxBuffer[5];
                    _stExchangeClient.u8IpAddr[1]  = acRxBuffer[6];
                    _stExchangeClient.u8IpAddr[2]  = acRxBuffer[7];
                    _stExchangeClient.u8IpAddr[3]  = acRxBuffer[8];

                    ESP_LOGI("ExchangeClient", "IP from player %d received: %d.%d.%d.%d",
                             _stExchangeClient.u8OpponentID,
                             _stExchangeClient.u8IpAddr[0],
                             _stExchangeClient.u8IpAddr[1],
                             _stExchangeClient.u8IpAddr[2],
                             _stExchangeClient.u8IpAddr[3]);
                    _stExchangeClient.u8Stage = 2;
                }
                else
                {
                    ESP_LOGI("ExchangeClient", "Waiting for opponent");
                }
            }
            else if (2 == _stExchangeClient.u8Stage)
            {
                char acCommand[5] = { 'C', 'y', 'a', '\r', '\n' };
                if (0 == memcmp(acCommand, &acRxBuffer, 5))
                {
                    _stExchangeClient.bIsRunning = false;
                    continue;
                }
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI("ExchangeClient", "Shutting down socket.");
    shutdown(nSock, 0);
    close(nSock);

    vTaskDelete(NULL);
}
