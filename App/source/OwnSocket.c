
/*
***********************************************************************************************************************
*                                Head Files
***********************************************************************************************************************
*/

#include "includes.h"
#include "lwip/sockets.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "app.h"
#include "queue.h"
#include "lwip/arch.h"
#include "OwnSocket.h"

/*
***********************************************************************************************************************
*                                Local Variable(s) Declaration
***********************************************************************************************************************
*/

/*
***********************************************************************************************************************
*                                Local Function(s) Declaration
***********************************************************************************************************************
*/



/*
***********************************************************************************************************************
*                                Global Functions Realization
***********************************************************************************************************************
*/

/*
***********************************************************************************************************************
* Function Name: CloseSocket
*
* Description: Close socket
*
* Input Parameter: piSocket: pointer to the socket to be closed
*
* Output Parameter: piSocket: pointer to the socket has closed
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.03.25
***********************************************************************************************************************
*/

void CloseSocket( int32_t *piSocket )
{
    if( piSocket != NULL )
    {
        if( *piSocket != -1 )
        {
            /* Close the socket */
            if( close( *piSocket ) != -1 )
            {
                *piSocket = -1;
            }
            else
            {
                printf( "close error %d.\r\n", errno );
            }
        }
    }

    return;
}

/*
***********************************************************************************************************************
* Function Name: CloseConnSocket
*
* Description: Close connected socket
*
* Input Parameter: piSocket: pointer to the socket to be closed
*
* Output Parameter: piSocket: pointer to the socket has closed
*
* Return: Close result
*
* Note: None
*
* Author Date: wangdong 2018.03.25
***********************************************************************************************************************
*/

void CloseConnSocket( int32_t *piSocket )
{
    if( piSocket != NULL )
    {
        if( *piSocket != -1 )
        {
            /* Shutdown */
            if( -1 == shutdown( *piSocket, SHUT_WR ))
            {
                printf( "shutdown error %d.\r\n", errno );
            }

            CloseSocket( piSocket );
        }
    }

    return;
}

/*
***********************************************************************************************************************
* Function Name: CreateListenSocket
*
* Description: Create listen socket
*
* Input Parameter: usListenPort: Listen port;
*                  bNonBlock: non-block flag.
*
* Output Parameter: none
*
* Return: FD of listen socket, -1, failed
*
* Note: None
*
* Author Date: wangdong 2018.03.25
***********************************************************************************************************************
*/

int32_t CreateListenSocket( uint16_t usListenPort, bool_t bNonBlock )
{
    int32_t iLisSock = -1;
    int32_t iOptval = 1;
    SOCKADDR_IN stSockAddr;
    memset( &stSockAddr, 0, sizeof(SOCKADDR_IN));

    if( usListenPort != 0U )
    {
        /* Create Listen Socket */
        iLisSock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        if( iLisSock != -1 )
        {
            printf( "socket ok: LisFD %d.\r\n", iLisSock );

            if( bNonBlock )
            {
                if( !SetNonBlockMode( iLisSock ))
                {
                    printf( "SetNonBlockMode error.\r\n" );
                    CloseSocket( &iLisSock );
                }
            }

            if( iLisSock != -1 )
            {
                /* permit reuse addr: SO_REUSEADDR is enough for this application;
                 *                    SO_REUSEPORT is useful for multi-threads application,
                 *                       Linux will dispatch connect to multi-threads(each has a
                 *                       listen socket binded to the same ip address and port) */
                iOptval = 1;
                if( setsockopt( iLisSock, SOL_SOCKET, SO_REUSEADDR, &iOptval, sizeof(iOptval)) != -1 )
                {
                    /* Bind */
                    stSockAddr.sin_family = AF_INET;
                    stSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
                    stSockAddr.sin_port = htons( usListenPort );  /* Local Port */
                    if( bind( iLisSock, (SOCKADDR*)&stSockAddr, sizeof(SOCKADDR_IN)) != -1 )
                    {
                        /* Listen */
                        if( listen( iLisSock, 4 ) != -1 )  /* SOMAXCONN */
                        {
                            printf( "listen port %d ok.\r\n", usListenPort );
                        }
                        else
                        {
                            printf( "listen error %d.\r\n", errno );
                            CloseSocket( &iLisSock );
                        }
                    }
                    else
                    {
                        printf( "bind error %d.\r\n", errno );
                        CloseSocket( &iLisSock );
                    }
                }
                else
                {
                    printf( "setsockopt error %d.\r\n", errno );
                    CloseSocket( &iLisSock );
                }
            }
        }
        else
        {
            printf( "socket error %d.\r\n", errno );
        }
    }

    return iLisSock;
}

/*
***********************************************************************************************************************
* Function Name: Connect
*
* Description: Connect to remote object
*
* Input Parameter: pcDestIPAddr: Destination IP address;
*                  usDestPort: Destination port;
*                  bNonBlock: non-block flag.
*
* Output Parameter: None
*
* Return: Connected socket, -1 - connected failed
*
* Note: None
*
* Author Date: wangdong 2018.03.25
***********************************************************************************************************************
*/

int32_t SocketConnect( char_t const pcDestIPAddr[], uint16_t usDestPort, bool_t bNonBlock )
{
    int32_t iFDConnSock = -1;
    SOCKADDR_IN stSockAddr;
    memset( &stSockAddr, 0, sizeof(SOCKADDR_IN));

    if((pcDestIPAddr != NULL) &&
       (usDestPort != 0))
    {
        /* Convert "xxx.xxx.xxx.xxx" to uint32_t */
        if( inet_aton( &pcDestIPAddr[0], &stSockAddr.sin_addr ) != 0 )
        {
            /* Create Connect Socket */
            iFDConnSock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
            if( iFDConnSock != -1 )
            {
                if( bNonBlock )
                {
                    if( !SetNonBlockMode( iFDConnSock ))
                    {
                        /* SetNonBlockMode Failed: Record Info */
                        printf( "SetNonBlockMode error.\r\n" );
                        CloseSocket( &iFDConnSock );
                    }
                }

                if( iFDConnSock != -1 )
                {
                    /* Connect */
                    stSockAddr.sin_family = AF_INET;
                    stSockAddr.sin_port = htons( usDestPort );           /* Local Port */
                    if( connect( iFDConnSock, (SOCKADDR*)&stSockAddr, sizeof(SOCKADDR_IN)) != -1 )
                    {
                        printf( "Connect %s ok.\r\n", &pcDestIPAddr[0]);
                    }
                    else
                    {
                        if( errno != EINPROGRESS )
                        {
                            /* Connect failed */
                            printf( "connect %s error: %d.\r\n", &pcDestIPAddr[0], errno );
                            CloseSocket( &iFDConnSock );
                        }
                        else
                        {
                            /* Connect In Progress */
                            printf( "connect %s is in process.\r\n", &pcDestIPAddr[0]);
                        }
                    }
                }
            }
            else
            {
                printf( "socket error: %d.\r\n", errno );
            }
        }
        else
        {
            printf( "inet_pton failed: %s.\r\n", pcDestIPAddr );
        }
    }

    return iFDConnSock;
}

/*
***********************************************************************************************************************
* Function Name: SetNonBlockMode
*
* Description: Set non-block mode
*
* Input Parameter: iFD: FD of object
*
* Output Parameter: None
*
* Return: Set result
*
* Note: None
*
* Author Date: wangdong 2018.03.25
***********************************************************************************************************************
*/

bool_t SetNonBlockMode( int32_t iFD )
{
    bool_t bResult = false;
    int32_t iFlags = 0;

    if( iFD != -1 )
    {
        /* Get Flags */
        iFlags = fcntl( iFD, F_GETFL, 0 );
        if( iFlags != -1 )
        {
            if( fcntl( iFD, F_SETFL, iFlags | O_NONBLOCK ) != -1 )
            {
                bResult = true;
            }
            else
            {
                printf( "fcntl(set) failed %d.\r\n", errno );
                bResult = false;
            }
        }
        else
        {
            printf( "fcntl(get) failed %d.\r\n", errno );
            bResult = false;
        }
    }

    return bResult;
}

/*
***********************************************************************************************************************
*                                Local Functions Realization
***********************************************************************************************************************
*/


/*
***********************************************************************************************************************
                                 File End
***********************************************************************************************************************
*/

