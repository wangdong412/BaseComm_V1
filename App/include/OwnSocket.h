
#ifndef OWN_SOCKET_H_
#define OWN_SOCKET_H_

/*
***********************************************************************************************************************
*                                Head Files
***********************************************************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "DataType.h"
#include "lwip/opt.h"
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/igmp.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"

/*
***********************************************************************************************************************
*                                Macro Definition
***********************************************************************************************************************
*/
/* Maximum queue length specifiable by listen.  */
#define SOMAXCONN	(128)

#define MAX_RETRY_CNT (3)


/*
***********************************************************************************************************************
*                                Data Structure Definition
***********************************************************************************************************************
*/

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct ifreq IFREQ;

/*
***********************************************************************************************************************
*                                Global Function Declaration
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

void CloseSocket( int32_t *piSocket );

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

void CloseConnSocket( int32_t *piSocket );

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

int32_t CreateListenSocket( uint16_t usListenPort, bool_t bNonBlock );

/*
***********************************************************************************************************************
* Function Name: SocketConnect
*
* Description: Connect to remote object
*
* Input Parameter: pcDestIPAddr: Destination IP address锛�
*                  usDestPort: Destination port锛�
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

int32_t SocketConnect( char_t const pcDestIPAddr[], uint16_t usDestPort, bool_t bNonBlock );

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

bool_t SetNonBlockMode( int32_t iFD );

#endif /* OWN_SOCKET_H_ */

/*
***********************************************************************************************************************
                                 File End
***********************************************************************************************************************
*/


