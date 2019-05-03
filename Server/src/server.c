/**
 * @file      server.c
 * @brief     SNESoIP server
 * @defgroup  Server SNESoIP server
 * @ingroup   Server
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "inih/ini.h"

/**
 * @struct  Client
 * @brief   Client data
 */
typedef struct Client_t
{
    uint16_t u16Data;

} Client;

/**
 * @struct  Server
 * @brief   Server data
 */
typedef struct Server_t
{
    struct sockaddr_storage stClientAddr;

    bool    bIsRunning;
    uint8_t u8NumClients;
    Client  astClient[];

} Server;

/**
 * @struct  Config
 * @brief   Server configuration
 */
typedef struct Config_t
{
    uint16_t u16Port;
    uint8_t  u8MaxClients;
    char     acAddr[15];

} Config;

static void* _ConnHandler(void* pSock);
static void  _IntHandler(int nSig);
static void* _GetInAddr(struct sockaddr *stAddr);
static int   _ConfigHandler(void* pUser, const char* pacSection, const char* pacName, const char* pacValue);

/**
 * @var    _stServer
 * @brief  Server private data
 */
static Server _stServer = { 0 };

int main(int argc, char* argv[])
{
    struct sockaddr_in stServerAddr;

    int       nRet = EXIT_SUCCESS;
    Config    stConfig;
    char      pacIniFile[20];
    int       nSock;
    int       nNewSock;
    int       nSockOpt= 1;
    socklen_t nAddrSize;

    signal(SIGINT, _IntHandler);

    if (argc > 1)
    {
        strncpy(pacIniFile, argv[1], 20);
    }
    else
    {
        strncpy(pacIniFile, "config.ini", 11);
    }

    if (0 > ini_parse(pacIniFile, _ConfigHandler, &stConfig))
    {
        fprintf(stderr, "Unable to load %s.\n", pacIniFile);
        return EXIT_FAILURE;
    }

    if (0 == stConfig.u8MaxClients)
    {
        fprintf(stderr, "Error: max_clients can't be set to zero.\n");
        return EXIT_FAILURE;
    }

    if (0 != (stConfig.u8MaxClients % 2))
    {
        fprintf(stderr, "Error: max_clients must be set to an even number.\n");
        return EXIT_FAILURE;
    }

    stServerAddr.sin_family      = AF_INET;
    stServerAddr.sin_port        = htons(stConfig.u16Port);
    stServerAddr.sin_addr.s_addr = inet_addr(stConfig.acAddr);

    nSock = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == nSock)
    {
        perror(strerror(errno));
    }

    if (-1 == setsockopt(nSock, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(int)))
    {
        nRet = EXIT_FAILURE;
        perror(strerror(errno));
        goto quit;
    }

    memset(stServerAddr.sin_zero, '\0', sizeof stServerAddr.sin_zero);

    nRet = bind(nSock, (struct sockaddr*)&stServerAddr, sizeof(stServerAddr));
    if (-1 == nSock)
    {
        perror(strerror(errno));
    }

    if (-1 == listen(nSock, 2))
    {
        nRet = EXIT_FAILURE;
        perror(strerror(errno));
        goto quit;
    }

    puts("");
    puts(" ███████╗███╗   ██╗███████╗███████╗ ██████╗ ██╗██████╗");
    puts(" ██╔════╝████╗  ██║██╔════╝██╔════╝██╔═══██╗██║██╔══██╗");
    puts(" ███████╗██╔██╗ ██║█████╗  ███████╗██║   ██║██║██████╔╝");
    puts(" ╚════██║██║╚██╗██║██╔══╝  ╚════██║██║   ██║██║██╔═══╝");
    puts(" ███████║██║ ╚████║███████╗███████║╚██████╔╝██║██║");
    puts(" ╚══════╝╚═╝  ╚═══╝╚══════╝╚══════╝ ╚═════╝ ╚═╝╚═╝");
    printf(" IP: \x1b[36m%s  \x1b[0mPort: \x1b[36m%u  \x1b[0mQuit: CTRL+c\n",
           stConfig.acAddr, stConfig.u16Port);
    puts(" Listening.\n");

    _stServer.bIsRunning = true;
    while (_stServer.bIsRunning)
    {
        int       nSockOpts = 1;
        pthread_t stThreadID;
        nAddrSize = sizeof(_stServer.stClientAddr);

        nNewSock = accept(nSock, (struct sockaddr*)&_stServer.stClientAddr, &nAddrSize);
        if (-1 == nNewSock)
        {
            perror(strerror(errno));
            continue;
        }

        if (setsockopt(nNewSock, SOL_SOCKET, SO_KEEPALIVE, (void *)&nSockOpts, sizeof(nSockOpts)))
        {
            perror(strerror(errno));
            continue;
        };

        if (_stServer.u8NumClients >= stConfig.u8MaxClients)
        {
            puts("No open slots available.");
            continue;
        }

        if (0 != pthread_create(&stThreadID, NULL, _ConnHandler, (void*)&nNewSock))
        {
            perror(strerror(errno));
            continue;
        }
    }

quit:
    close(nSock);
    return nRet;
}

/**
 * @fn     void* _ConnHandler(void* nSock)
 * @brief  Connection handler.
 * @todo   Add WRIO/RDIO to protocol description.
 * @details
 * @code{.unparsed}
 *
 * Present protocol description
 *
 * Stage 1:
 *
 * The client connects to the server and the server assigns a
 * consecutive ID to the client.  This ID correlates to the counter of
 * the currently connected clients.
 *
 * The server sends a 6-byte message where the last byte is the asigned
 * client ID:
 *
 *   +---+---+---+---+---+---+
 *   | H | e | l | l | o | 0 |
 *   +---+---+---+---+---+---+
 *
 * Stage 1 - First contact:
 *
 * If the client ID is an even number, the client assigns itself to the
 * console's first controller port.  And the server links the client to
 * the client with the next higher odd ID number.  This client
 * automatically assigns itself to the second controller port.  The
 * maximum number of clients can only be set to an even number.
 *
 * Stage 2 - Conversation:
 *
 *   Abbreviation legend
 *
 *   ID0: own client ID
 *   ID1: client ID of opponent
 *   DLB: controller data, low byte
 *   DHB: controller data, high byte
 *
 * From this point on, the server accepts the following commands:
 *
 * +-------------------+-------------------+---------------+
 * | Command           | Response          | Description   |
 * +-------------------+-------------------+---------------+
 * | +---+---+---+---+ | +---+---+---+---+ |               |
 * | | D |ID0|DLB|DLH| | | D |ID1|DLB|DLH| | Data exchange |
 * | +---+---+---+---+ | +---+---+---+---+ |               |
 * +-------------------+-------------------+---------------+
 * | +---+---+---+---+ | +---+---+---+---+ |      End      |
 * | |ID0| B | y | e | | | C | y | a |ID0| |  conversation |
 * | +---+---+---+---+ | +---+---+---+---+ |               |
 * +-------------------+-------------------+---------------+
 *
 * @endcode
 */
static void* _ConnHandler(void* pSock)
{
    uint8_t u8ClientID    = _stServer.u8NumClients;
    uint8_t u8OpponentID  = 0;
    bool    bIsConnected  = true;
    char    acTxBuffer[6] = { 'H', 'e', 'l', 'l', 'o', u8ClientID };
    char    acRxBuffer[4] = { 0 };
    char    acIpAddr[15]  = { 0 };
    int     nSock         = *(int*)pSock;
    int     nReceived     = 0;

    inet_ntop(
        _stServer.stClientAddr.ss_family,
        _GetInAddr((struct sockaddr*)&_stServer.stClientAddr),
        acIpAddr,
        sizeof(acIpAddr));

    printf(" (%u) %s connected.\n", u8ClientID, acIpAddr);
    _stServer.u8NumClients += 1;

    _stServer.astClient[u8ClientID].u16Data = 0xffff;
    if (0 == (u8ClientID % 2) || 0 == u8ClientID)
    {
        u8OpponentID = u8ClientID + 1;
    }
    else
    {
        u8OpponentID = u8ClientID - 1;
    }
    printf(" Player %u is now assigned to player %u.\n", u8ClientID, u8OpponentID);

    // Stage 1 - First contact:
    if (-1 == send(nSock, acTxBuffer, 6, 0))
    {
        perror(strerror(errno));
    }

    // Stage 2 - Conversation:
    while (bIsConnected)
    {
        int  nSize;
        char acCommand[2][4] = {
            { u8ClientID, 'B', 'y', 'e' },
            { 'D', u8OpponentID, 0, 0 }
        };
        char acResponse[2][4] = {
            { 'C', 'y', 'a', u8ClientID },
            { 'D', u8OpponentID, 0, 0 }
        };

        nSize = recv(nSock, acRxBuffer, 4, 0);
        nReceived += nSize;

        // Command received.
        if (nReceived >= 4)
        {
            // End conversation.
            if (0 == memcmp(&acCommand[0], &acRxBuffer, 4))
            {
                memcpy(acTxBuffer, acResponse[0], 4);
                if (-1 == send(nSock, acTxBuffer, 4, 0))
                {
                    perror(strerror(errno));
                }
                bIsConnected = false;
                continue;
            }
            // Data exchange.
            else if (0 == memcmp(&acCommand[1], &acRxBuffer, 2))
            {
                // Todo.
            }
            nSize     = 0;
            nReceived = 0;
        }
    }

    printf(" (%u) %s disconnected.\n", u8ClientID, acIpAddr);
    _stServer.u8NumClients -= 1;

    return 0;
}

/**
 * @fn     void _IntHandler(int nSig)
 * @brief  Interrupt handler.
 */
static void _IntHandler(int nSig)
{
    char c;

    signal(nSig, SIG_IGN);
    printf("\nDo you really wanna quit? [y/n] ");
    c = getchar();
    if ('y' == c || 'Y' == c)
    {
        exit(0);
    }
    else
    {
        signal(SIGINT, _IntHandler);
    }
    getchar();
}

/**
 * @fn     void *_GetInAddr(struct sockaddr *stAddr)
 * @brief  Get sockaddr, IPv4 or IPv6.
 */
static void* _GetInAddr(struct sockaddr *stAddr)
{
    if (AF_INET == stAddr->sa_family)
    {
        return &(((struct sockaddr_in*)stAddr)->sin_addr);
    }
    else
    {
	return &(((struct sockaddr_in6*)stAddr)->sin6_addr);
    }
}

/**
 * @brief  Configuration handler.
 */
static int _ConfigHandler(void* pUser, const char* pacSection, const char* pacName, const char* pacValue)
{
    Config* pstConfig = (Config*)pUser;

    #define MATCH(s, n) 0 == strcmp(pacSection, s) && 0 == strcmp(pacName, n)

    if (MATCH("General", "port"))
    {
        pstConfig->u16Port = atoi(pacValue);
    }
    else if (MATCH("General", "addr"))
    {
        strncpy(pstConfig->acAddr, pacValue, 15);
    }
    else if (MATCH("General", "max_clients"))
    {
        pstConfig->u8MaxClients = atoi(pacValue);
    }
    else
    {
        return 0;
    }

    return 1;
}
