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
    bool bIsRunning;

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
    ESP_LOGI("tcpd", "Initialise TCP client.");
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

    char acRXBuffer[128];
    char acAddrStr[128];
    int  nAddrFamily;
    int  nIPProtocol;
    int  nListenSock;
    int  nSockOpt;
    int  nErr;

    stDestAddr.sin_addr.s_addr = htonl("10.0.0.3");
    stDestAddr.sin_family      = AF_INET;
    stDestAddr.sin_port        = htons(1994);
    nAddrFamily                = AF_INET;
    nIPProtocol                = IPPROTO_IP;
    inet_ntoa_r(stDestAddr.sin_addr, acAddrStr, sizeof(acAddrStr) - 1);

    vTaskDelete(NULL);
}

/*
static void tcp_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    while (1) {

#ifdef CONFIG_EXAMPLE_IPV4
#else // IPV6
        struct sockaddr_in6 destAddr;
        inet6_aton(HOST_IP_ADDR, &destAddr.sin6_addr);
        destAddr.sin6_family = AF_INET6;
        destAddr.sin6_port = htons(PORT);
        addr_family = AF_INET6;
        ip_protocol = IPPROTO_IPV6;
        inet6_ntoa_r(destAddr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

        int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = connect(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Successfully connected");

        while (1) {
            int err = send(sock, payload, strlen(payload), 0);
            if (err < 0) {
                ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
                break;
            }

            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occured during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", rx_buffer);
            }

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}
*/
