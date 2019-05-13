/**
 * @file       TCPServer.c
 * @brief      TCP server
 * @details    A TCP server to use as a terminal interface
 * @ingroup    Firmware
 * @author     Michael Fitzmayer
 * @copyright  "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "SNES.h"
#include "TCPServer.h"

/**
 * @struct  TCPServer
 * @brief   TCP server data
 */
typedef struct TCPServer_t
{
    bool bIsRunning;
    bool bHostConnected;

} TCPServer;

/**
 * @var    _stServer
 * @brief  TCP server private data
 */
static TCPServer _stServer;

static void _TCPServerThread(void* pArg);
static bool _CheckCommand(char* pacRxBuffer, char* pacCommand);

/**
 * @fn     void InitTCPServer(void)
 * @brief  Initialise TCP server
 */
void InitTCPServer(void)
{
    ESP_LOGI("tcpd", "Initialise TCP server.");
    memset(&_stServer, 0, sizeof(struct TCPServer_t));
    xTaskCreate(_TCPServerThread, "TCPServerThread", 4096, NULL, 3, NULL);
}

/**
 * @fn     void DeInitTCPServer(void)
 * @brief  De-initialise/stop TCP server
 */
void DeInitTCPServer(void)
{
    _stServer.bIsRunning = false;
}

/**
 * @fn     bool IsHostConnected(void)
 * @brief  Check if the server has accepted a connection
 */
bool IsHostConnected(void)
{
    if (_stServer.bHostConnected)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @fn     void _TCPServerThread(void* pArg)
 * @brief  TCP server
 * @param  pArg
 *         Unused
 */
static void _TCPServerThread(void* pArg)
{
    struct sockaddr_in stDestAddr;

    char acRxBuffer[128];
    char acAddrStr[128];
    int  nAddrFamily;
    int  nIPProtocol;
    int  nListenSock;
    int  nSockOpt;
    int  nErr;

    stDestAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    stDestAddr.sin_family      = AF_INET;
    stDestAddr.sin_port        = htons(TCP_SERVER_PORT);
    nAddrFamily                = AF_INET;
    nIPProtocol                = IPPROTO_IP;
    inet_ntoa_r(stDestAddr.sin_addr, acAddrStr, sizeof(acAddrStr) - 1);

    nListenSock = socket(nAddrFamily, SOCK_STREAM, nIPProtocol);
    if (0 > nListenSock)
    {
        ESP_LOGE("tcpd", "Unable to create socket: errno %d", errno);
    }
    else
    {
        ESP_LOGI("tcpd", "Socket created successfully.");
    }

    if (0 > setsockopt(nListenSock, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(int)))
    {
        ESP_LOGE("tcpd", "Unable to set socket option: errno %d", errno);
    }

    nErr = bind(nListenSock, (struct sockaddr*)&stDestAddr, sizeof(stDestAddr));
    if (0 != nErr)
    {
        ESP_LOGE("tcpd", "Couldn't bind name to socket: errno %d", errno);
    }
    else
    {
        ESP_LOGI("tcpd", "Name successfully bound to socket.");
    }

    _stServer.bIsRunning = true;
    while (_stServer.bIsRunning)
    {
        struct sockaddr_in stSourceAddr;
        uint uAddrLen;
        int  nSock;

        nErr = listen(nListenSock, 1);
        if (0 != nErr)
        {
            ESP_LOGE("tcpd", "Error occured during listen: errno %d", errno);
            break;
        }
        else
        {
            ESP_LOGI("tcpd", "Listening.");
        }

        uAddrLen = sizeof(stSourceAddr);
        nSock    = accept(nListenSock, (struct sockaddr*)&stSourceAddr, &uAddrLen);
        if (0 > nSock)
        {
            ESP_LOGE("tcpd", "Unable to accept connection: errno %d", errno);
            break;
        }
        else
        {
            ESP_LOGI("tcpd", "Connection established");
            send(nSock, "Connection established\r\n", 24, 0);
            _stServer.bHostConnected = true;
        }

        while (_stServer.bIsRunning)
        {
            int nLen = recv(nSock, acRxBuffer, sizeof(acRxBuffer) - 1, 0);
            // Error occured during receiving.
            if (nLen < 0)
            {
                ESP_LOGE("tcpd", "recv failed: errno %d", errno);
                break;
            }
            // Connection closed.
            else if (nLen == 0)
            {
                ESP_LOGI("tcpd", "Connection closed");
                break;
            }
            // Data received.
            else
            {
                // Get the sender's IP address as string.
                inet_ntoa_r(
                    ((struct sockaddr_in *)&stSourceAddr)->sin_addr.s_addr,
                    acAddrStr, sizeof(acAddrStr) - 1);

                // Null-terminate whatever we received and treat like a string.
                acRxBuffer[nLen] = 0;
                if (nLen > 2)
                {
                    ESP_LOGI("tcpd", "Received %d bytes from %s: %s", nLen, acAddrStr, acRxBuffer);
                }

                // Parse commands.
                if (_CheckCommand(acRxBuffer, "version"))
                {
                    char* pacVersion = VERSION;
                    send(nSock, pacVersion, strlen(pacVersion), 0);
                }
                else if (_CheckCommand(acRxBuffer, "input"))
                {
                    uint16_t u16InputData    = GetSNESInputData();
                    uint8_t  u8Index         = 15;
                    char     acInputData[18] = { 0 };
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

                    acInputData[16] = '\r';
                    acInputData[17] = '\n';
                    send(nSock, acInputData, strlen(acInputData), 0);
                }
            }
        }

        _stServer.bHostConnected = false;
        if (-1 != nSock)
        {
            ESP_LOGI("tcpd", "Shutting down socket and restarting.");
            shutdown(nSock, 0);
            close(nSock);
        }
    }

    vTaskDelete(NULL);
}

static bool _CheckCommand(char* pacRxBuffer, char* pacCommand)
{
    if (0 == strncmp(pacRxBuffer, pacCommand, strlen(pacCommand)))
    {
        return true;
    }
    else
    {
        return false;
    }
}
