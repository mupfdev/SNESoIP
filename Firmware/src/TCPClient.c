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
    ESP_LOGI("Client", "Initialise TCP client.");
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

    char acRxBuffer[11];
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
        ESP_LOGE("Client", "Unable to create socket: errno %d", errno);
    }
    else
    {
        ESP_LOGI("Client", "Socket created successfully.");
    }

    nErr = connect(nSock, (struct sockaddr *)&stDestAddr, sizeof(stDestAddr));
    if (0 != nErr)
    {
        ESP_LOGE("Client", "Unable to connect: errno %d", errno);
    }
    else
    {
        ESP_LOGI("Client", "Successfully connected");
    }

    _stClient.bIsRunning = true;
    while (_stClient.bIsRunning)
    {
        int  nLen;
        char acCommand[7] = { 0 };

        if (1 == _stClient.u8Stage)
        {
            ESP_LOGI("Client", "Requesting IP address");
            memcpy(acCommand, "GetIP\r\n", 7);
            nErr = send(nSock, acCommand, 7, 0);
        }
        if (2 == _stClient.u8Stage)
        {
            memcpy(acCommand, "Bye\r\n", 5);
            nErr = send(nSock, acCommand, 5, 0);
        }
        if (0 > nErr)
        {
            ESP_LOGE("Client", "Error occured during sending: errno %d", errno);
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
            if (0 == _stClient.u8Stage)
            {
                char acCommand[7] = { 'H', 'e', 'l', 'l', 'o' };
                if (0 == memcmp(acCommand, &acRxBuffer, 5))
                {
                    ESP_LOGI("Client", "Client ID received: %d", acRxBuffer[5]);
                    _stClient.u8ClientID = acRxBuffer[5];
                    _stClient.u8Stage    = 1;
                }
            }
            else if (1 == _stClient.u8Stage)
            {
                char acCommand[4] = { 'I', 'P', 'O', 'K' };
                if (0 == memcmp(acCommand, &acRxBuffer, 4))
                {
                    _stClient.u8OpponentID = acRxBuffer[4];
                    _stClient.u8IpAddr[0]  = acRxBuffer[5];
                    _stClient.u8IpAddr[1]  = acRxBuffer[6];
                    _stClient.u8IpAddr[2]  = acRxBuffer[7];
                    _stClient.u8IpAddr[3]  = acRxBuffer[8];

                    ESP_LOGI("Client", "IP from player %d received: %d.%d.%d.%d",
                             _stClient.u8OpponentID,
                             _stClient.u8IpAddr[0],
                             _stClient.u8IpAddr[1],
                             _stClient.u8IpAddr[2],
                             _stClient.u8IpAddr[3]);
                    _stClient.u8Stage = 2;
                }
                else
                {
                    ESP_LOGI("Client", "Waiting for opponent");
                }
            }
            else if (2 == _stClient.u8Stage)
            {
                char acCommand[5] = { 'C', 'y', 'a', '\r', '\n' };
                if (0 == memcmp(acCommand, &acRxBuffer, 5))
                {
                    _stClient.bIsRunning = false;
                    continue;
                }
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI("Client", "Shutting down socket.");
    shutdown(nSock, 0);
    close(nSock);

    vTaskDelete(NULL);
}
