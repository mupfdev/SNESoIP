/**
 * @file       Terminal.c
 * @brief      Terminal interface
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
#include "Terminal.h"

/**
 * @struct  Terminal
 * @brief   Terminal data
 */
typedef struct Terminal_t
{
    bool bIsRunning;
    bool bHostConnected;

} Terminal;

/**
 * @var    _stTerminal
 * @brief  Terminal private data
 */
static Terminal _stTerminal;

static void _TerminalThread(void* pArg);
static bool _CheckCommand(char* pacRxBuffer, char* pacCommand);

/**
 * @fn     void InitTerminal(void)
 * @brief  Initialise Terminal
 */
void InitTerminal(void)
{
    ESP_LOGI("Term", "Initialise terminal");
    memset(&_stTerminal, 0, sizeof(struct Terminal_t));
    xTaskCreate(_TerminalThread, "TerminalThread", 4096, NULL, 3, NULL);
}

/**
 * @fn     void DeInitTerminal(void)
 * @brief  De-initialise/stop terminal
 */
void DeInitTerminal(void)
{
    _stTerminal.bIsRunning = false;
}

/**
 * @fn     bool IsHostConnected(void)
 * @brief  Check if the server has accepted a connection
 */
bool IsHostConnected(void)
{
    if (_stTerminal.bHostConnected)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @fn     void _TerminalThread(void* pArg)
 * @brief  Terminal thread
 * @param  pArg
 *         Unused
 */
static void _TerminalThread(void* pArg)
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
    stDestAddr.sin_port        = htons(TERMINAL_PORT);
    nAddrFamily                = AF_INET;
    nIPProtocol                = IPPROTO_IP;
    inet_ntoa_r(stDestAddr.sin_addr, acAddrStr, sizeof(acAddrStr) - 1);

    nListenSock = socket(nAddrFamily, SOCK_STREAM, nIPProtocol);
    if (0 > nListenSock)
    {
        ESP_LOGE("Term", "Unable to create socket: errno %d", errno);
    }
    else
    {
        ESP_LOGI("Term", "Socket created successfully.");
    }

    if (0 > setsockopt(nListenSock, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(int)))
    {
        ESP_LOGE("Term", "Unable to set socket option: errno %d", errno);
    }

    nErr = bind(nListenSock, (struct sockaddr*)&stDestAddr, sizeof(stDestAddr));
    if (0 != nErr)
    {
        ESP_LOGE("Term", "Couldn't bind name to socket: errno %d", errno);
    }
    else
    {
        ESP_LOGI("Term", "Name successfully bound to socket.");
    }

    _stTerminal.bIsRunning = true;
    while (_stTerminal.bIsRunning)
    {
        struct sockaddr_in stSourceAddr;
        uint uAddrLen;
        int  nSock;

        nErr = listen(nListenSock, 1);
        if (0 != nErr)
        {
            ESP_LOGE("Term", "Error occured during listen: errno %d", errno);
            break;
        }
        else
        {
            ESP_LOGI("Term", "Listening.");
        }

        uAddrLen = sizeof(stSourceAddr);
        nSock    = accept(nListenSock, (struct sockaddr*)&stSourceAddr, &uAddrLen);
        if (0 > nSock)
        {
            ESP_LOGE("Term", "Unable to accept connection: errno %d", errno);
            break;
        }
        else
        {
            char* pacGreeting =
                "   ____ _  __ ____ ____       ____ ___\r\n"
                "  / __// |/ // __// __/___   /  _// _ \\\r\n"
                " _\\ \\ /    // _/ _\\ \\ / _ \\ _/ / / ___/\r\n"
                "/___//_/|_//___//___/ \\___//___//_/\r\n"
                "  Connection established.\r\n";

            ESP_LOGI("Term", "Connection established");

            send(nSock, pacGreeting, strlen(pacGreeting), 0);
            _stTerminal.bHostConnected = true;
        }

        while (_stTerminal.bIsRunning)
        {
            int nLen = recv(nSock, acRxBuffer, sizeof(acRxBuffer) - 1, 0);
            // Error occured during receiving.
            if (nLen < 0)
            {
                ESP_LOGE("Term", "recv failed: errno %d", errno);
                break;
            }
            // Connection closed.
            else if (nLen == 0)
            {
                ESP_LOGI("Term", "Connection closed");
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
                    ESP_LOGI("Term", "Received %d bytes from %s: %s", nLen, acAddrStr, acRxBuffer);
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

        _stTerminal.bHostConnected = false;
        if (-1 != nSock)
        {
            ESP_LOGI("Term", "Shutting down socket and restarting.");
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
