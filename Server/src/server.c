/**
 * @file      server.c
 * @brief     SNESoIP server
 * @details   Proof-of-concept IP exchange server to use with the
 *            SNESoIP v3.
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

#include <CommonInclude.h>
#include "inih/ini.h"

/**
 * @enum   eCommand
 * @brief  Client/Server commands
 */
typedef enum eCommand_t
{
    C_HELO = 0,
    C_BYE,
    C_CYA,
    C_GETIP,
    C_NONE,
    C_NUM
} eCommand;

/**
 * @struct  Client
 * @brief   Client data
 */
typedef struct Client_t
{
    uint16_t u16Data;
    uint8_t  au8IP[4];

} Client;

/**
 * @struct  Config
 * @brief   Server configuration
 */
typedef struct Config_t
{
    uint16_t u16Port;
    uint8_t  u8MaxClients;
    uint8_t  u8Verbose;
    char     acAddr[15];

} Config;

/**
 * @struct  Server
 * @brief   Server data
 */
typedef struct Server_t
{
    struct sockaddr_storage stClientAddr;

    bool    bIsRunning;
    uint8_t u8NumClients;
    Config  stConfig;
    Client  astClient[];

} Server;

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

    if (0 > ini_parse(pacIniFile, _ConfigHandler, &_stServer.stConfig))
    {
        fprintf(stderr, "Unable to load %s.\n", pacIniFile);
        return EXIT_FAILURE;
    }

    if (0 == _stServer.stConfig.u8MaxClients)
    {
        fprintf(stderr, "Error: max_clients can't be set to zero.\n");
        return EXIT_FAILURE;
    }

    if (0 != (_stServer.stConfig.u8MaxClients % 2))
    {
        fprintf(stderr, "Error: max_clients must be set to an even number.\n");
        return EXIT_FAILURE;
    }

    stServerAddr.sin_family      = AF_INET;
    stServerAddr.sin_port        = htons(_stServer.stConfig.u16Port);
    stServerAddr.sin_addr.s_addr = inet_addr(_stServer.stConfig.acAddr);

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
           _stServer.stConfig.acAddr, _stServer.stConfig.u16Port);
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

        if (_stServer.u8NumClients >= _stServer.stConfig.u8MaxClients)
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
 * @todo   Implement WRIO/RDIO usage.
 * @details
 * @code{.unparsed}
 *
 * Current protocol description
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
 *   +---+---+---+---+---+
 *   | H | e | l | o | 0 |
 *   +---+---+---+---+---+
 *
 * Stage 1 - First contact:
 *
 * If the client ID is an even number, the client assigns itself to the
 * console's first controller port.  And the server links the client to
 * the client with the next higher odd ID number.  This client
 * automatically assigns itself to the second controller port.  The
 * maximum number of clients can only be set to an even number.
 *
 * Stage 2 - Exchange:
 *
 *   Abbreviation legend
 *
 *   ID0:   own client ID
 *   ID1:   client ID of opponent
 *   DLB:   controller data, low byte
 *   DHB:   controller data, high byte
 *   IHH:
 *   IHL:   IP address.
 *   ILH:   e. g. 127.0.0.1 = IHH.IHL.ILH.ILL
 *   ILL:
 *   Empty: ignored
 *
 * From this point on, the server accepts the following commands with a
 * max. total length of 5 bytes:
 *
 * +-----------------------+-----------------------+---------------+
 * | Command               | Response              |  Description  |
 * +-----------------------+-----------------------+---------------+
 * | +---+---+---+---+---+ | +---+---+---+---+---+ |      End      |
 * | |ID0| B | y | e |   | | | C | y | a |ID0|   | |  conversation |
 * | +---+---+---+---+---+ | +---+---+---+---+---+ |               |
 * +-----------------------+-----------------------+---------------+
 * | +---+---+---+---+---+ | +---+---+---+---+---+ |    Request    |
 * | | G | e | t | I | P | | |ID1|IHH|IHL|ILH|ILL| | IP address of |
 * | +---+---+---+---+---+ | +---+---+---+---+---+ |    opponent   |
 * +-----------------------+ If not available:     |               |
 * |                       | +---+---+---+---+---+ |               |
 * |                       | |ID1| N | o | n | e | |               |
 * |                       | +---+---+---+---+---+ |               |
 * +-----------------------+-----------------------+---------------+
 *
 * Stage 3 - Direct contact
 *
 * As soon as both clients have a valid IP address, they start a direct
 * data exchange via UDP.  How this has to look like still has to be
 * specified.
 *
 * @endcode
 */
static void* _ConnHandler(void* pSock)
{
    uint8_t u8ClientID    = _stServer.u8NumClients;
    uint8_t u8OpponentID  = 0;
    bool    bIsConnected  = true;
    char    acRxBuffer[5] = { 0 };
    char    acTxBuffer[5] = { 0 };
    char    acIpAddr[15]  = { 0 };
    int     nSock         = *(int*)pSock;
    int     nReceived     = 0;

    inet_ntop(
        _stServer.stClientAddr.ss_family,
        _GetInAddr((struct sockaddr*)&_stServer.stClientAddr),
        acIpAddr,
        sizeof(acIpAddr));

    if (IpIsValid(acIpAddr))
    {
        printf(" (%u) %s connected.\n", u8ClientID, acIpAddr);
        StrToIP(acIpAddr, _stServer.astClient[u8ClientID].au8IP);
    }
    else
    {
        perror("Error: Invalid IP address.\n");
        return 0;
    }

    _stServer.u8NumClients += 1;

    if (0 == (u8ClientID % 2) || 0 == u8ClientID)
    {
        u8OpponentID = u8ClientID + 1;
    }
    else
    {
        u8OpponentID = u8ClientID - 1;
    }
    printf(" Player %u is now assigned to player %u.\n", u8ClientID, u8OpponentID);

    char acCommand[C_NUM][5] = {
        { 'H', 'e', 'l', 'o', u8ClientID },
        { u8ClientID, 'B', 'y', 'e', 0 },
        { 'C', 'y', 'a', u8ClientID, 0 },
        { 'G', 'e', 't', 'I', 'P' },
        { u8OpponentID, 'N', 'o', 'n', 'e' }
    };

    // Stage 1 - First contact:
    memcpy(acTxBuffer, acCommand[C_HELO], 5);
    if (-1 == send(nSock, acTxBuffer, 5, 0))
    {
        perror(strerror(errno));
    }

    // Stage 2 - Conversation:
    while (bIsConnected)
    {
        int  nSize;

        nSize = recv(nSock, acRxBuffer, 5, 0);
        nReceived += nSize;

        // Full command received.
        if (nReceived >= 5)
        {
            memset(acTxBuffer, 0, sizeof(acTxBuffer));

            // End conversation.
            if (0 == memcmp(&acCommand[C_BYE], &acRxBuffer, 5))
            {
                printf(" (%u) initiated a disconnect.\n", u8ClientID);
                memcpy(acTxBuffer, acCommand[C_CYA], 5);

                if (-1 == send(nSock, acTxBuffer, 5, 0))
                {
                    perror(strerror(errno));
                }
                bIsConnected = false;
                continue;
            }
            // IP request.
            else if(0 == memcmp(&acCommand[C_GETIP], &acRxBuffer, 5))
            {
                printf(" (%u) requested his opponent's IP address: ", u8ClientID);
                if (0 == _stServer.astClient[u8OpponentID].au8IP[0])
                {
                    puts("IP not available.");
                    memcpy(acTxBuffer, acCommand[C_NONE], 5);
                }
                else
                {
                    char acGr1[4];
                    char acGr2[4];
                    char acGr3[4];
                    char acGr4[4];

                    sprintf(acGr1, "%u", _stServer.astClient[u8OpponentID].au8IP[0]);
                    sprintf(acGr2, "%u", _stServer.astClient[u8OpponentID].au8IP[1]);
                    sprintf(acGr3, "%u", _stServer.astClient[u8OpponentID].au8IP[2]);
                    sprintf(acGr4, "%u", _stServer.astClient[u8OpponentID].au8IP[3]);
                    printf("%s.%s.%s.%s sent.\n", acGr1, acGr2, acGr3, acGr4);

                    acTxBuffer[0] = u8OpponentID;
                    for (uint8_t u8Index = 1; u8Index < 5; u8Index++)
                    {
                        acTxBuffer[u8Index] =
                            _stServer.astClient[u8OpponentID].au8IP[u8Index - 1];
                    }
                }

                if (-1 == send(nSock, acTxBuffer, 5, 0))
                {
                    perror(strerror(errno));
                }
            }

            nSize     = 0;
            nReceived = 0;
        }
    }

    printf(" (%u) disconnected.\n", u8ClientID);
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
    else if (MATCH("General", "verbose"))
    {
        pstConfig->u8Verbose = atoi(pacValue);
    }
    else
    {
        return 0;
    }

    return 1;
}
