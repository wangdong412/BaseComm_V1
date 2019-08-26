
/*
***********************************************************************************************************************
*                                Head Files
***********************************************************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ucos_ii.h"
#include "Usart.h"
#include "SysCommon.h"
#include "SerlComm.h"
#include "InterComm.h"
#include "BaseConfig.h"

/*
***********************************************************************************************************************
*                                Local Variable(s) Declaration
***********************************************************************************************************************
*/

/* Info. array */
static SerlCommInfo_t s_stSerlCommInfoArray[BASE_PORT_NUM];

/* Local buffer */
static uint8_t s_ucReq[IC_BUFF_LEN];
static uint8_t s_ucResp[IC_BUFF_LEN];

/*
***********************************************************************************************************************
*                                Local Function(s) Declaration
***********************************************************************************************************************
*/
static bool_t CheckRespTimeout( SerlCommInfo_t const *pstSerlCommInfo );

/*
***********************************************************************************************************************
*                                Global Functions Realization
***********************************************************************************************************************
*/

/*
***********************************************************************************************************************
* Function Name: SerlCommTask
*
* Description: Serial communication Task Function
*
* Input Parameter: pdata: thread parameters.
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
void SerlCommTask( void *pdata )
{
    SrcID_t emSrcID = SRC_ID_COM3;
    SerlCommInfo_t *pstSerlCommInfo = NULL;
    uint32_t uiReqLen = 0U;
    uint16_t usCOMID = (SYS_ID_COM3);
    uint32_t uiRespLen = 0U;
    uint32_t uiCurTick = 0U;
    int32_t iRet = -1;
    int32_t j = 0U;

    while( 1 )
    {
        //printf( "SerlCommTask\r\n" );

        for( emSrcID = SRC_ID_COM3; emSrcID <= SRC_ID_COM8; emSrcID++ )
        {
            pstSerlCommInfo = &s_stSerlCommInfoArray[emSrcID];
            usCOMID = (SYS_ID_COM3) + emSrcID - SRC_ID_COM3;

            if( IDLE_STATE == pstSerlCommInfo->emState )
            {
                /* Handle request */
                /* Get Request message: master data */
                uiReqLen = InterCommGetReqInfo( emSrcID, &s_ucReq[0], sizeof(s_ucReq));

                /* Test- Modbus RTU */
                #if 0
                if((SYS_ID_COM5) == usCOMID )
                {
                    s_ucReq[0] = 0x11;  /* Slave ID:      17 */
                    s_ucReq[1] = 0x01;  /* Function Code: 01 */
                    s_ucReq[2] = 0x00;  /* Start addr:    20 (20-1=19) */
                    s_ucReq[3] = 0x13;
                    s_ucReq[4] = 0x00;  /* Coil counts:   37 */
                    s_ucReq[5] = 0x25;
                    s_ucReq[6] = 0x0E;  /* CRC-16 */
                    s_ucReq[7] = 0x84;
                    uiReqLen = 8;
                }
                #endif

                if( uiReqLen > 0U )
                {
#if SERLCOMM_DEBUG
                    /* Test */
                    printf( "COM[%d] Req: ", usCOMID );
                    for( j = 0; j < uiReqLen; j++ )
                    {
                        printf( "%X ", s_ucReq[j] );
                    }
                    printf( "\r\n" );
#else
                    printf( "COM[%d] Reqlen %d\r\n", usCOMID, uiReqLen );
#endif

                    /* Send request message */
                    if( UsartWrite( GetUSARTxFromCOMID( usCOMID ), &s_ucReq[0], uiReqLen ) != -1 )
                    {
                        /* Clear receive buffer */
                        UsartClrRecvInfo( GetUSARTxFromCOMID( usCOMID ));

                        printf( "UsartWrite OK\r\n" );

                        /* Set timeout ticks */
                        uiCurTick = OSTimeGet();
                        if((MAX_TICKS) - uiCurTick < (WAIT_RESP_TO_TICKS))
                        {
                            pstSerlCommInfo->bOverflow = true;
                        }
                        else
                        {
                            pstSerlCommInfo->bOverflow = false;
                        }

                        pstSerlCommInfo->uiCurTick = uiCurTick;
                        pstSerlCommInfo->uiTickRespTO = uiCurTick + (WAIT_RESP_TO_TICKS);
                        pstSerlCommInfo->emState = BUSY_STATE;

                        /*printf( "OF %d, CT 0x%x, TTO 0x%x\r\n", pstSerlCommInfo->bOverflow, pstSerlCommInfo->uiCurTick,
                                pstSerlCommInfo->uiTickRespTO );*/
                    }
                    else
                    {
                        printf( "UsartWrite failed\r\n" );
                    }
                }
            }
            else
            {
                //OSTimeDlyHMSM( 0, 0, 3, 0 );  /* Test */

                /* Handle response */
                uiRespLen = 0U;
                iRet = UsartRead( GetUSARTxFromCOMID( usCOMID ), &s_ucResp[0], sizeof(s_ucResp));
                if( -1 == iRet )
                {
                    printf( "COM[%d] UsartRead failed.\r\n", usCOMID );
                }
                else
                {
                    uiRespLen = (uint32_t)iRet;
                }

                if( uiRespLen > 0 )
                {
#if SERLCOMM_DEBUG
                    /* Test */
                    printf( "COM[%d] Resp: ", usCOMID );
                    for( j = 0; j < uiRespLen; j++ )
                    {
                        printf( "%X ", s_ucResp[j] );
                    }
                    printf( "\r\n" );
#else
                    printf( "COM[%d] Resplen %d\r\n", usCOMID, uiRespLen );
#endif

                    /* Set response message */
                    InterCommSetRespInfo( emSrcID, &s_ucResp[0], uiRespLen );
                    uiRespLen = 0U;
                    pstSerlCommInfo->emState = IDLE_STATE;
                }
                else
                {
                    /* Check if timeout */
                    if( CheckRespTimeout( pstSerlCommInfo ))
                    {
                        pstSerlCommInfo->emState = IDLE_STATE;
                        printf( "COM[%d] timeout\r\n", usCOMID );
                    }
                }
            }
        }

        OSTimeDlyHMSM( 0, 0, 0, 100 );  /* Sleep */
    }

}

/*
***********************************************************************************************************************
* Function Name: SerlCommInit
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
* Author Date: wangdong 2018.04.02
***********************************************************************************************************************
*/
void SerlCommInit( void )
{
    uint32_t i = 0U;
    printf( "SerlCommInit\r\n" );

    /* 0-Configure Port, 1-COM3 Port ... 6-COM6 Port */
    for( i = 0U; i < (BASE_PORT_NUM); i++ )
    {
        /* Serial communication info. */
        memset( &s_stSerlCommInfoArray[i], 0, sizeof(SerlCommInfo_t));
    }

    /* Receive/Send buffer */
    memset( &s_ucReq[0], 0, sizeof(s_ucReq));
    memset( &s_ucResp[0], 0, sizeof(s_ucResp));

    return;
}

/*
***********************************************************************************************************************
*                                Local Functions Realization
***********************************************************************************************************************
*/
/*
***********************************************************************************************************************
* Function Name: CheckRespTimeout
*
* Description: Check if response timeout
*
* Input Parameter: pstSerlCommInfo: Comm. info.
*
* Output Parameter: None
*
* Return: true if timeout
*
* Note: None
*
* Author Date: wangdong 2018.04.01
***********************************************************************************************************************
*/

static bool_t CheckRespTimeout( SerlCommInfo_t const *pstSerlCommInfo )
{
    bool_t bTimeout = false;
    uint32_t uiCurTick = 0U;

    if( pstSerlCommInfo != NULL )
    {
        uiCurTick = OSTimeGet();

        if( !pstSerlCommInfo->bOverflow )
        {
            if( uiCurTick > pstSerlCommInfo->uiTickRespTO )
            {
                bTimeout = true;
            }
        }
        else
        {
            if((uiCurTick > pstSerlCommInfo->uiTickRespTO) && (uiCurTick < pstSerlCommInfo->uiCurTick))
            {
                bTimeout = true;
            }
        }
    }

    return bTimeout;
}

/*
***********************************************************************************************************************
                                 File End
***********************************************************************************************************************
*/

