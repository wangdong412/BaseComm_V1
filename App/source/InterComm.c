
/*
***********************************************************************************************************************
*                                Head Files
***********************************************************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lwip/arch.h"
#include "InterComm.h"
#include "OwnSocket.h"
#include "SysCommon.h"
#include "os_cpu.h"

/*
***********************************************************************************************************************
*                                Local Variable(s) Declaration
***********************************************************************************************************************
*/

/* Socket FD: Listen/Connect */
static int32_t s_iFDListen[BASE_PORT_NUM];
static int32_t s_iFDConn[BASE_PORT_NUM];

/* Local receive info. / send buffer */
static RecvInfo_t s_stRecvInfoArray[BASE_PORT_NUM];
static uint8_t s_ucSendBuff[IC_BUFF_LEN];

/* Info. Array: Request/Response */
static SrcReqInfo_t s_stReqInfoArray[BASE_PORT_NUM];
static SrcRespInfo_t s_stRespInfoArray[BASE_PORT_NUM];

/*
***********************************************************************************************************************
*                                Local Function(s) Declaration
***********************************************************************************************************************
*/
static void ClrLocalRecvInfoArray( void );
static void ClrLocalRecvInfo( uint32_t uiIndex );
static void AcceptNewConnect( void );

static bool_t ReadReqMsg( void );
static void ReReadReqMsg( void );
static int32_t ReadAppData( int32_t iConnFD, uint8_t pucRecvBuff[], uint32_t uiBuffLen );

static void SendRespMsg( void );
static int32_t SendAppData( int32_t iConnFD, uint8_t const pucData[], uint32_t uiDataLen );

/*
***********************************************************************************************************************
*                                Global Functions Realization
***********************************************************************************************************************
*/

/*
***********************************************************************************************************************
* Function Name: InterCommTask
*
* Description: Internal communication Task Function
*
* Input Parameter: pdata: thread parameters.
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.04.01
***********************************************************************************************************************
*/
void InterCommTask( void *pdata )
{
    uint32_t i = 0U;

    while( 1 )
    {
        //printf( "InterCommTask\r\n" );

        /* Accept */
        AcceptNewConnect();

        /* Clear local receive info. */
        ClrLocalRecvInfoArray();

        /* Receive req msg from CPU board: config / master data,
         * and Set to Config module / Serial module */
        if( ReadReqMsg())
        {
            /* Delay: 20ms
             * 512B, 10/100Mbps, just need 0.39ms */
            OSTimeDlyHMSM( 0, 0, 0, 20 );

            /* Re-read */
            ReReadReqMsg();

            /* Set request info. */
            for( i = 0U; i < (BASE_PORT_NUM); i++ )
            {
                if( s_stRecvInfoArray[i].uiDataLen > 0u )
                {
                    InterCommSetReqInfo((SrcID_t)i, &s_stRecvInfoArray[i].ucData[0], s_stRecvInfoArray[i].uiDataLen );
                    s_stRecvInfoArray[i].uiDataLen = 0U;
                }
            }
        }

        /* Get resp msg from Config module / Serial module,
         * and send to CPU board */
        SendRespMsg();

        OSTimeDlyHMSM( 0, 0, 0, 100 );
    }
}

/*
***********************************************************************************************************************
* Function Name: InterCommInit
*
* Description: Initialize
*
* Input Parameter: None
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.04.01
***********************************************************************************************************************
*/
void InterCommInit( void )
{
    uint32_t i = 0U;

    /* Server */
    /* 0-Configure Port, 1-COM3 Port ... 6-COM6 Port */
    for( i = 0U; i < (BASE_PORT_NUM); i++ )
    {
        /* Listen port, non-block */
        s_iFDListen[i] = CreateListenSocket( BASE_PORT_CFG+i, true );
        if( -1 == s_iFDListen[i])
        {
            printf( "CreateListenSocket[%d] failed.\r\n", i );
        }

        /* Connect socket: accepted */
        s_iFDConn[i] = -1;

        /* Receive info. */
        memset( &s_stRecvInfoArray[i], 0, sizeof(RecvInfo_t));

        /* Req & Resp info. */
        memset( &s_stReqInfoArray[i], 0, sizeof(SrcReqInfo_t));
        memset( &s_stRespInfoArray[i], 0, sizeof(SrcRespInfo_t));
    }

    /* Receive/Send buffer */
    memset( &s_ucSendBuff[0], 0, sizeof(s_ucSendBuff));

    return;
}

/*
***********************************************************************************************************************
* Function Name: InterCommSetReqInfo
*
* Description: Set request info.
*
* Input Parameter: emSrcID: Source ID;
*                  pucData: Data;
*                  uiDataLen: Data length.
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.04.02
***********************************************************************************************************************
*/
void InterCommSetReqInfo( SrcID_t emSrcID, uint8_t const pucData[], uint32_t uiDataLen )
{
    OS_CPU_SR cpu_sr;
	
    if((emSrcID <= SRC_ID_MAX) &&
       (pucData != NULL) &&
       ((uiDataLen > 0) && (uiDataLen <= (IC_BUFF_LEN))))
    {
        OS_ENTER_CRITICAL();

        /* Copy in if empty */
        if( 0U == s_stReqInfoArray[emSrcID].uiDataLen )
        {
            memcpy( &s_stReqInfoArray[emSrcID].ucData[0], &pucData[0], uiDataLen );
            s_stReqInfoArray[emSrcID].uiDataLen = uiDataLen;
        }

        OS_EXIT_CRITICAL();
    }

    return;
}

/*
***********************************************************************************************************************
* Function Name: InterCommGetReqInfo
*
* Description: Get request info.
*
* Input Parameter: emSrcID: Source ID;
*                  pucBuff: Buffer;
*                  uiBuffLen: Buffer length.
*
* Output Parameter: None
*
* Return: Data length
*
* Note: None
*
* Author Date: wangdong 2018.04.02
***********************************************************************************************************************
*/
uint32_t InterCommGetReqInfo( SrcID_t emSrcID, uint8_t pucBuff[], uint32_t uiBuffLen )
{
    uint32_t uiDataLen = 0U;
    OS_CPU_SR cpu_sr;

    if(( emSrcID <= SRC_ID_MAX ) &&
       ( pucBuff != NULL ) &&
       ( uiBuffLen > 0 ))
    {
        OS_ENTER_CRITICAL();

        /* Copy out if has data */
        if(( s_stReqInfoArray[emSrcID].uiDataLen > 0U) &&
           ( uiBuffLen >= s_stReqInfoArray[emSrcID].uiDataLen ))
        {
            memcpy( &pucBuff[0], &s_stReqInfoArray[emSrcID].ucData[0], s_stReqInfoArray[emSrcID].uiDataLen );
            uiDataLen = s_stReqInfoArray[emSrcID].uiDataLen;
            s_stReqInfoArray[emSrcID].uiDataLen = 0U;
        }

        OS_EXIT_CRITICAL();
    }

    return uiDataLen;
}

/*
***********************************************************************************************************************
* Function Name: InterCommSetRespInfo
*
* Description: Set response info.
*
* Input Parameter: emSrcID: Source ID;
*                  pucData: Data;
*                  uiDataLen: Data length.
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.04.02
***********************************************************************************************************************
*/
void InterCommSetRespInfo( SrcID_t emSrcID, uint8_t const pucData[], uint32_t uiDataLen )
{
    OS_CPU_SR cpu_sr;

    if((emSrcID <= SRC_ID_MAX) &&
       (pucData != NULL) &&
       ((uiDataLen > 0) && (uiDataLen <= (IC_BUFF_LEN))))
    {
        OS_ENTER_CRITICAL();

        /* Copy in if empty */
        if( 0U == s_stRespInfoArray[emSrcID].uiDataLen )
        {
            memcpy( &s_stRespInfoArray[emSrcID].ucData[0], &pucData[0], uiDataLen );
            s_stRespInfoArray[emSrcID].uiDataLen = uiDataLen;
        }

        OS_EXIT_CRITICAL();
    }

    return;
}

/*
***********************************************************************************************************************
* Function Name: InterCommGetRespInfo
*
* Description: Set request info.
*
* Input Parameter: emSrcID: Source ID;
*                  pucBuff: Buffer;
*                  uiBuffLen: Buffer length.
*
* Output Parameter: None
*
* Return: Data length
*
* Note: None
*
* Author Date: wangdong 2018.04.02
***********************************************************************************************************************
*/
uint32_t InterCommGetRespInfo( SrcID_t emSrcID, uint8_t pucBuff[], uint32_t uiBuffLen )
{
    uint32_t uiDataLen = 0U;
    OS_CPU_SR cpu_sr;

    if(( emSrcID <= SRC_ID_MAX ) &&
       ( pucBuff != NULL ) &&
       ( uiBuffLen > 0 ))
    {
        OS_ENTER_CRITICAL();

        /* Copy out if has data */
        if(( s_stRespInfoArray[emSrcID].uiDataLen > 0U) &&
           ( uiBuffLen >= s_stRespInfoArray[emSrcID].uiDataLen ))
        {
            memcpy( &pucBuff[0], &s_stRespInfoArray[emSrcID].ucData[0], s_stRespInfoArray[emSrcID].uiDataLen );
            uiDataLen = s_stRespInfoArray[emSrcID].uiDataLen;
            s_stRespInfoArray[emSrcID].uiDataLen = 0U;
        }

        OS_EXIT_CRITICAL();
    }

    return uiDataLen;
}

/*
***********************************************************************************************************************
*                                Local Functions Realization
***********************************************************************************************************************
*/

/*
***********************************************************************************************************************
* Function Name: ClrLocalRecvInfoArray
*
* Description: Initialize
*
* Input Parameter: None
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.04.01
***********************************************************************************************************************
*/

static void ClrLocalRecvInfoArray( void )
{
    uint32_t i = 0U;

    for( i = 0U; i < (BASE_PORT_NUM); i++ )
    {
        s_stRecvInfoArray[i].bDelay = false;
        s_stRecvInfoArray[i].uiDataLen = 0U;
    }

    return;
}

/*
***********************************************************************************************************************
* Function Name: ClrLocalRecvInfo
*
* Description: Clear
*
* Input Parameter: uiIndex - array index
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.04.01
***********************************************************************************************************************
*/

static void ClrLocalRecvInfo( uint32_t uiIndex )
{
    if( uiIndex < (BASE_PORT_NUM))
    {
        s_stRecvInfoArray[uiIndex].bDelay = false;
        s_stRecvInfoArray[uiIndex].uiDataLen = 0U;
    }

    return;
}

/*
***********************************************************************************************************************
* Function Name: AcceptNewConnect
*
* Description: Listen new connect request
*
* Input Parameter: None
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.04.02
***********************************************************************************************************************
*/

static void AcceptNewConnect( void )
{
    struct timeval stTV;     /* Time out */
    fd_set  fsFd;            /* Socket Queue */
    bool_t bFDEmpty = true;  /* Socket Queue Empty Flag */
    int32_t iMaxFDP = 0;     /* Max FDP of the socket queue */
    SOCKADDR_IN stAddr;      /* Remote Address */
    uint32_t uiLen = 0;      /* Address Length */
    int32_t iResult = 0;
    uint32_t i = 0U;

    /* Update Socket Queue */
    FD_ZERO( &fsFd );
    for( i = 0U; i < (BASE_PORT_NUM); i++ )
    {
        if( s_iFDListen[i] != -1 )
        {
            FD_SET( s_iFDListen[i], &fsFd );  /* Add Valid Listen Socket */
            bFDEmpty = false;
            if( s_iFDListen[i] > iMaxFDP )
            {
                iMaxFDP = s_iFDListen[i];
            }
        }
    }/* for */

    if( !bFDEmpty )
    {
        stTV.tv_sec = 0U;
        stTV.tv_usec = 0U;
        iResult = select( iMaxFDP+1, &fsFd, NULL, NULL, &stTV );
        if( -1 == iResult )
        {
            /* Select error */
            printf( "Accept select error: %d.\r\n", errno );
        }
        else
        {
            if( iResult > 0 )
            {
                /* Has new connect */
                for( i = 0U; i < (BASE_PORT_NUM); i++ )
                {
                    if( FD_ISSET( s_iFDListen[i], &fsFd ))
                    {
                        /* Has new connect request */
                        /* check if connect before */
                        if( s_iFDConn[i] < 0 )
                        {
                            /* not connect before */
                            uiLen = sizeof( SOCKADDR_IN );
                            memset( &stAddr, 0, uiLen );
                            s_iFDConn[i] = accept( s_iFDListen[i], (SOCKADDR*)&stAddr, (socklen_t*)&uiLen );
                            if( s_iFDConn[i] != -1 )
                            {
                                printf( "accept ok: LisFD %d, Remote IP 0x%x.\r\n", s_iFDListen[i], stAddr.sin_addr.s_addr );

                                SetNonBlockMode( s_iFDConn[i]);
                                ClrLocalRecvInfo( i );
                            }
                            else
                            {
                                printf( "accept error[%d]: LisFD %d.\r\n", errno, s_iFDListen[i]);
                            }
                        }
                        else
                        {
                            printf( "accept: has connect before, LisFD %d, index %d.\r\n", i, s_iFDListen[i]);
                        }
                    }
                }
            }
        }
    }

    return;
}

/*
***********************************************************************************************************************
* Function Name: ReadReqMsg
*
* Description: Read request message: network
*
* Input Parameter: None
*
* Output Parameter: None
*
* Return: Delay flag
*
* Note: None
*
* Author Date: wangdong 2018.04.02
***********************************************************************************************************************
*/

static bool_t ReadReqMsg( void )
{
    struct timeval stTV;     /* Time out */
    fd_set  fsFd;            /* Socket Queue */
    bool_t bFDEmpty = true;  /* Socket Queue Empty Flag */
    int32_t iMaxFDP = 0;     /* Max FDP of the socket queue */
    int32_t iResult = 0;
    uint32_t i = 0U;
    int32_t iReadLen = 0;
    uint32_t uiIndex = 0U;
    RecvInfo_t *pstRecvInfo = NULL;
    bool_t bDelay = false;

    /* Update Socket Queue */
    FD_ZERO( &fsFd );
    for( i = 0U; i < (BASE_PORT_NUM); i++ )
    {
        if( s_iFDConn[i] != -1 )
        {
            FD_SET( s_iFDConn[i], &fsFd );  /* Add Valid Listen Socket */
            bFDEmpty = false;
            if( s_iFDConn[i] > iMaxFDP )
            {
                iMaxFDP = s_iFDConn[i];
            }
        }
    }/* for */

    if( !bFDEmpty )
    {
        stTV.tv_sec = 0U;
        stTV.tv_usec = 0U;
        iResult = select( iMaxFDP+1, &fsFd, NULL, NULL, &stTV );
        if( -1 == iResult )
        {
            /* Select error */
            printf( "Read select error.\r\n" );
        }
        else
        {
            if( iResult > 0 )
            {
                /* Has new data to be received */
                for( i = 0U; i < (BASE_PORT_NUM); i++ )
                {
                    if( FD_ISSET( s_iFDConn[i], &fsFd ))
                    {
                        /* Receive data */
                        pstRecvInfo = &s_stRecvInfoArray[i];
                        uiIndex = pstRecvInfo->uiDataLen;

                        iReadLen = ReadAppData( s_iFDConn[i], &pstRecvInfo->ucData[uiIndex], (IC_BUFF_LEN) - uiIndex );
                        if( -1 == iReadLen )
                        {
                            printf( "ReadAppData error, Disconnect %d index %d\r\n", s_iFDConn[i], i );
                            CloseConnSocket( &s_iFDConn[i]);  /* Disconnect */
                            ClrLocalRecvInfo( i );
                        }
                        else
                        {
                            if( iReadLen > 0 )
                            {
                                pstRecvInfo->uiDataLen += (uint32_t)iReadLen;
                                pstRecvInfo->bDelay = true;
                                bDelay = true;
                            }
                        }
                    }
                }
            }
        }
    }

    return bDelay;
}

/*
***********************************************************************************************************************
* Function Name: ReReadReqMsg
*
* Description: Re-Read request message(network): used to confirm received a complete message
*
* Input Parameter: None
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.04.02
***********************************************************************************************************************
*/

static void ReReadReqMsg( void )
{
    uint32_t i = 0U;
    uint32_t uiIndex = 0U;
    int32_t iReadLen = 0;
    RecvInfo_t *pstRecvInfo = NULL;

    /* Re-read */
    for( i = 0U; i < (BASE_PORT_NUM); i++ )
    {
        if( s_stRecvInfoArray[i].bDelay )
        {
            pstRecvInfo = &s_stRecvInfoArray[i];

            /* Receive data */
            uiIndex = pstRecvInfo->uiDataLen;
            iReadLen = ReadAppData( s_iFDConn[i], &pstRecvInfo->ucData[uiIndex], (IC_BUFF_LEN) - uiIndex );
            if( -1 == iReadLen )
            {
                printf( "ReReadReqMsg error, Disconnect %d index %d\r\n", s_iFDConn[i], i );
                CloseConnSocket( &s_iFDConn[i]);  /* Disconnect */
                ClrLocalRecvInfo( i );
            }
            else
            {
                if( iReadLen > 0 )
                {
                    pstRecvInfo->uiDataLen += iReadLen;
                }
            }

            pstRecvInfo->bDelay = false;
        }
    }

    return;
}

/*
***********************************************************************************************************************
* Function Name: ReadAppData
*
* Description: Read application data
*
* Input Parameter: iConnFD: FD of connected socket;
*                  pucRecvBuff: Receive buffer;
*                  uiBuffLen: Buffer length.
*
* Output Parameter: None
*
* Return: Read length, -1 read failed
*
* Note: None
*
* Author Date: wangdong 2018.04.02
***********************************************************************************************************************
*/

static int32_t ReadAppData( int32_t iConnFD, uint8_t pucRecvBuff[], uint32_t uiBuffLen )
{
    int32_t iRet = 0;
    int32_t iReadLen = 0;
    uint8_t ucRetryCnts = 0U;
    uint32_t uiIndex = 0U;

    if((iConnFD != -1) &&
       (pucRecvBuff != NULL) &&
       (uiBuffLen > 0U))
    {
        while( ucRetryCnts < (uint8_t)(MAX_RETRY_CNT))
        {
            iRet = recv( iConnFD, &pucRecvBuff[uiIndex], uiBuffLen-uiIndex, 0 );
            if( -1 == iRet )
            {
                /* Error Handling */
                if( EWOULDBLOCK == errno )
                {
                    /* Non-block socket: EAGAIN/EWOULDBLOCK - Read done */
                    break;
                }
                else
                {
                    /* Receive error */
                    printf( "recv error: %d.\r\n", errno );
                    iReadLen = -1;
                    break;
                }
            }
            else if( 0 == iRet )
            {
                /* Has disconnected */
                printf( "recv error - Disconnected.\r\n" );
                iReadLen = -1;
                break;
            }
            else
            {
                /* recv ok */
                uiIndex += (uint32_t)iRet;
                iReadLen = (int32_t)uiIndex;

                if( uiIndex == uiBuffLen )
                {
                    break;  /* No space */
                }
                else
                {
                    ucRetryCnts++;  /* retry until read done */
                }
            }
        }/* while */
    }

    return iReadLen;
}

/*
***********************************************************************************************************************
* Function Name: SendRespMsg
*
* Description: Send response message: network
*
* Input Parameter: None
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.04.02
***********************************************************************************************************************
*/

static void SendRespMsg( void )
{
    uint32_t i = 0U;
    uint32_t uiDataLen = 0U;
    int32_t iSendLen = 0U;

    /* Get response message and send */
    for( i = 0U; i < (BASE_PORT_NUM); i++ )
    {
        /* Get response message */
        uiDataLen = InterCommGetRespInfo((SrcID_t)i, &s_ucSendBuff[0], sizeof(s_ucSendBuff));
        if( uiDataLen > 0U )
        {
            /* Send */
            iSendLen = SendAppData( s_iFDConn[i], &s_ucSendBuff[0], uiDataLen );
            if( -1 == iSendLen )
            {
                printf( "SendAppData error, disconnect %d %d\r\n", s_iFDConn[i], i );
                CloseConnSocket( &s_iFDConn[i]);  /* Disconnect */
            }
            else
            {
                if( iSendLen == (int32_t)uiDataLen )
                {
                    printf( "SendAppData OK.\r\n" );
                }
            }
        }
    }/* for */
}

/*
***********************************************************************************************************************
* Function Name: SendAppData
*
* Description: Read application data
*
* Input Parameter: iConnFD: FD of connected socket;
*                  pucData: Data to be send;
*                  uiDataLen: Data length.
*
* Output Parameter: None
*
* Return: Send length, -1 send failed
*
* Note: None
*
* Author Date: wangdong 2018.04.02
***********************************************************************************************************************
*/

static int32_t SendAppData( int32_t iConnFD, uint8_t const pucData[], uint32_t uiDataLen )
{
    int32_t iRet = -1;
    int32_t iSendLen = 0U;
    uint32_t uiIndex = 0U;
    uint8_t ucRetryCnts = 0U;   /* Retry Counts */

    if(( iConnFD != -1 ) &&
       ( pucData != NULL ) &&
       ( uiDataLen > 0 ))
    {
        /* Send */
        while( ucRetryCnts < (MAX_RETRY_CNT))
        {
            iRet = send( iConnFD, &pucData[uiIndex], uiDataLen - uiIndex, 0 );
            if( -1 == iRet )
            {
                /* Error Handling */
                if( EWOULDBLOCK == errno )  /* Not Ready */
                {
                    ucRetryCnts++;
                }
                else
                {
                    /* Send Error */
                    printf( "send error: %d.\r\n", errno );
                    iSendLen = -1;
                    break;
                }
            }
            else
            {
                uiIndex += (uint32_t)iRet;
                iSendLen = (int32_t)uiIndex;
                if( uiIndex == uiDataLen )
                {
                    break;  /* Send ok */
                }
                else
                {
                    ucRetryCnts++;
                }
            }
        }/* while */
    }

    return iSendLen;
}

/*
***********************************************************************************************************************
                                 File End
***********************************************************************************************************************
*/

