/**
 * @file      server.c
 * @brief     SNESoIP server
 * @defgroup  Server SNESoIP server
 * @ingroup   Server
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
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
 * @struct  Server
 * @brief   Server data
 */
typedef struct Server_t
{
    struct sockaddr_storage stClientAddr;

    bool    bIsRunning;
    uint8_t u8NumClients;

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

/**
 * @var    _stServer
 * @brief  Server private data
 */
static Server _stServer = { 0 };

static void* _GetInAddr(struct sockaddr *stAddr);
static void  _ConnHandler(int nSock);
static int   _ConfigHandler(
    void*       pUser,
    const char* pacSection,
    const char* pacName,
    const char* pacValue);

int main(int argc, char* argv[])
{
    struct sockaddr_in      stServerAddr;

    int       nRet = EXIT_SUCCESS;
    Config    stConfig;
    char      pacIniFile[20];
    int       nPid;
    int       nSock;
    int       nNewSock;
    int       nSockOpt= 1;
    socklen_t nAddrSize;

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
        fprintf(stderr, "Unable to load %s\n", pacIniFile);
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
    printf(" IP: %s  Port: %u\n", stConfig.acAddr, stConfig.u16Port);
    puts(" Listening.\n");

    _stServer.bIsRunning = true;
    while (_stServer.bIsRunning)
    {
        nAddrSize = sizeof(_stServer.stClientAddr);

        nNewSock = accept(nSock, (struct sockaddr*)&_stServer.stClientAddr, &nAddrSize);
        if (-1 == nNewSock)
        {
            perror(strerror(errno));
            continue;
        }

        if (_stServer.u8NumClients >= stConfig.u8MaxClients)
        {
            puts("No open slots available.");
            continue;
        }

        nPid = fork();
        if (0 < nPid)
        {
            nRet = EXIT_FAILURE;
            perror(strerror(errno));
            continue;
        }
        else if (0 == nPid)
        {
            _ConnHandler(nNewSock);
        }
        else
        {
            close(nNewSock);
        }
    }

quit:
    close(nSock);
    return nRet;
}

/**
 * @fn     void _ConnHandler(int nSock)
 * @brief  Connection handler.
 */
static void _ConnHandler(int nSock)
{
    int  u8ClientID = _stServer.u8NumClients;
    char acIpAddr[15];
    (void)nSock;
    inet_ntop(
        _stServer.stClientAddr.ss_family,
        _GetInAddr((struct sockaddr*)&_stServer.stClientAddr),
        acIpAddr,
        sizeof(acIpAddr));

    printf(" (%u) %s connected\n", u8ClientID, acIpAddr);
    _stServer.u8NumClients += 1;

    // Do magic.

    printf(" (%u) %s disconnected\n", u8ClientID, acIpAddr);
    _stServer.u8NumClients -= 1;
}

static int _ConfigHandler(
    void*       pUser,
    const char* pacSection,
    const char* pacName,
    const char* pacValue)
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
