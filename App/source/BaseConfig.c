/*
***********************************************************************************************************************
*                                Head Files
***********************************************************************************************************************
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "BaseConfig.h"
#include "InterComm.h"
#include "BaseCommProtocol.h"
#include "SysCommon.h"
#include "ucos_ii.h"
#include "Flash_W25Q.h"

/*
***********************************************************************************************************************
*                                Local Variable(s) Declaration
***********************************************************************************************************************
*/

static BaseCOMCfgInfo_t s_stBaseCOMCfgInfo[SYS_COM_CNTS];
static uint8_t s_ucReq[512];   /* Request message buffer */
static uint8_t s_ucResp[512];  /* Response message buffer */


/*
***********************************************************************************************************************
*                                Local Function(s) Declaration
***********************************************************************************************************************
*/

static bool_t LoadCOMCfgFromFlash( uint8_t pucCfgBuff[], uint32_t uiBuffLen );
static bool_t StoreCOMCfgToFlash( uint8_t pucCfgData[], uint32_t uiCfgLen );
static void SetCOMPort( uint16_t usCOMID, BaseCOMCfg_t const *pstCOMCfg );

/*
***********************************************************************************************************************
*                                Global Functions Realization
***********************************************************************************************************************
*/

/*
***********************************************************************************************************************
* Function Name: ConfigTask
*
* Description: Config Task Function
*
* Input Parameter: pdata: thread parameters.
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.03.29
***********************************************************************************************************************
*/
void ConfigTask( void *pdata )
{
    int8_t i = 0U;
    bool_t bStoreFlash = false;
    uint32_t uiReqLen = 0U;
    uint32_t uiRespLen = 0U;

    while( 1 )
    {
        //printf( "ConfigTask\r\n" );

        /* Get request message */
        uiReqLen = InterCommGetReqInfo( SRC_ID_CFG, &s_ucReq[0], sizeof(s_ucReq));

        /* Test - Little - low addr low byte */
        #if 0
        /* Message Type 1: Config */
        /* Head: Start-0xAA55 Type-0x0001 Reserve-0x0000 DataLen-0x0007 */
        s_ucReq[0] = 0x55;
        s_ucReq[1] = 0xAA;
        s_ucReq[2] = 0x01;
        s_ucReq[3] = 0x00;
        s_ucReq[4] = 0x00;
        s_ucReq[5] = 0x00;
        s_ucReq[6] = 0x07;
        s_ucReq[7] = 0x00;

        /* Data: PortCnts-0x01 ID-0X0003*/
        s_ucReq[8] = 0x01;
        s_ucReq[9] = 0x03;
        s_ucReq[10] = 0x00;
        s_ucReq[11] = 0x08;  /* 19200 */
        s_ucReq[12] = 0x04;  /* 8 */
        s_ucReq[13] = 0x01;  /* 1 */
        s_ucReq[14] = 0x01;  /* None */

        /* CRC: 0 */
        s_ucReq[15] = 0;
        s_ucReq[16] = 0;
        s_ucReq[17] = 0;
        s_ucReq[18] = 0;
        uiReqLen = 19;
        #endif

        #if 0
        /* Message Type 2: Read Config */
        /* Head: Start-0xAA55 Type-0x0002 Reserve-0x0000 DataLen-0x0002 */
        s_ucReq[0] = 0x55;
        s_ucReq[1] = 0xAA;
        s_ucReq[2] = 0x02;
        s_ucReq[3] = 0x00;
        s_ucReq[4] = 0x00;
        s_ucReq[5] = 0x00;
        s_ucReq[6] = 0x02;
        s_ucReq[7] = 0x00;

        /* Data: PortID - 0x0000 All; 3-COM3... */
        s_ucReq[8] = 0x08;
        s_ucReq[9] = 0x00;

        /* CRC: 0 */
        s_ucReq[10] = 0;
        s_ucReq[11] = 0;
        s_ucReq[12] = 0;
        s_ucReq[13] = 0;
        uiReqLen = 14;
        #endif

        if( uiReqLen > 0U )
        {
#if BASECFG_DEBUG
            /* Test */
            printf( "CfgReq: " );
            for( i = 0; i < uiReqLen; i++ )
            {
                printf( "%X ", s_ucReq[i] );
            }
            printf( "\r\n" );
#else
            printf( "CfgReq len %d\r\n", uiReqLen );
#endif

            /* Parge request message */
            uiRespLen = BCPParseReq( &s_ucReq[0], uiReqLen, &s_ucResp[0], sizeof(s_ucResp));
            if( uiRespLen > 0U )
            {
#if BASECFG_DEBUG
                /* Test */
                printf( "CfgResp: " );
                for( i = 0; i < uiRespLen; i++ )
                {
                    printf( "%X ", s_ucResp[i]);
                }
                printf( "\r\n" );
#else
                printf( "CfgResp len %d\r\n", uiRespLen );
#endif

                /* Set response message */
                InterCommSetRespInfo( SRC_ID_CFG, &s_ucResp[0], uiRespLen );
            }
        }

        /* Set COM Port if has new config */
        for( i = 0; i < (SYS_COM_CNTS); i++ )
        {
            if( s_stBaseCOMCfgInfo[i].bNewCfg )
            {
                bStoreFlash = true;
                SetCOMPort( s_stBaseCOMCfgInfo[i].stCOMCfg.usID, &s_stBaseCOMCfgInfo[i].stCOMCfg );
                s_stBaseCOMCfgInfo[i].bNewCfg = false;
            }
        }

        /* Store to flash if needed */
        if( bStoreFlash )
        {
            if( StoreCOMCfgToFlash((uint8_t *)(&s_stBaseCOMCfgInfo[0]), (SYS_COM_CNTS)*sizeof(BaseCOMCfgInfo_t)))
            {
                printf( "Store flash ok\r\n" );
            }
            else
            {
                printf( "Store flash fail\r\n" );
            }

            bStoreFlash = false;
        }

        OSTimeDlyHMSM( 0, 0, 0, 100 );  /* Sleep */
    }
}

/*
***********************************************************************************************************************
* Function Name: BaseCfgInit
*
* Description: Initialize COM configure
*
* Input Parameter: None
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.03.26
***********************************************************************************************************************
*/
void BaseCfgInit( void )
{
    uint8_t i = 0;
    printf( "BaseCfgInit\r\n" );

    /* Initialize baseboard COM configuration */
    if( LoadCOMCfgFromFlash((uint8_t *)(&s_stBaseCOMCfgInfo[0]), (SYS_COM_CNTS)*sizeof(BaseCOMCfgInfo_t)))
    {
        printf( "Cfg: flash.\r\n" );
    }
    else
    {
        printf( "Cfg: default.\r\n" );

        /* Set default */
        for( i = 0; i < (SYS_COM_CNTS); i++ )
        {
            s_stBaseCOMCfgInfo[i].bNewCfg = false;
            s_stBaseCOMCfgInfo[i].stCOMCfg.usID = SYS_ID_COM3 + i;
            s_stBaseCOMCfgInfo[i].stCOMCfg.ucBaud = COM_BAUD_9600;
            s_stBaseCOMCfgInfo[i].stCOMCfg.ucDatabit = COM_DB_8;
            s_stBaseCOMCfgInfo[i].stCOMCfg.ucStopbit = COM_SB_1;
            s_stBaseCOMCfgInfo[i].stCOMCfg.ucParity = COM_PRITY_NONE;
        }
    }

    /* Set COM Port */
    for( i = 0; i < (SYS_COM_CNTS); i++ )
    {
        SetCOMPort( s_stBaseCOMCfgInfo[i].stCOMCfg.usID, &s_stBaseCOMCfgInfo[i].stCOMCfg );
    }

    /* Message buffer */
    memset( &s_ucReq[0], 0, sizeof(s_ucReq));
    memset( &s_ucResp[0], 0, sizeof(s_ucResp));

    return;
}

/*
***********************************************************************************************************************
* Function Name: BaseCfgGetBaseCOMCfg
*
* Description: Get base COM configuration
*
* Input Parameter: usCOMID: 3-COM3,...8-COM8
*
* Output Parameter: pstBaseCOMCfg: COM configuration.
*
* Return: Get result
*
* Note: None
*
* Author Date: wangdong 2018.03.29
***********************************************************************************************************************
*/
int32_t BaseCfgGetBaseCOMCfg( uint16_t usCOMID, BaseCOMCfg_t *pstBaseCOMCfg )
{
    int32_t iGetRet = (ECODE_PARA);  /* Get result */
    uint16_t usIndex = 0;
    BaseCOMCfgInfo_t *pstBaseCOMInfo = NULL;

    if(((usCOMID >= (SYS_COMID_MIN)) && (usCOMID <= (SYS_COMID_MAX))) &&
        (pstBaseCOMCfg != NULL))
    {
        usIndex =  usCOMID - (SYS_COMID_MIN);
        pstBaseCOMInfo = &s_stBaseCOMCfgInfo[usIndex];

        /* Copy out */
        memcpy( pstBaseCOMCfg, &pstBaseCOMInfo->stCOMCfg, sizeof(BaseCOMCfg_t));
        iGetRet = (ECODE_OK);
    }

    return iGetRet;
}

/*
***********************************************************************************************************************
* Function Name: BaseCfgSetBaseCOMCfg
*
* Description: Set base COM configuration
*
* Input Parameter: usCOMID: 3-COM3,...8-COM8;
*                  pstBaseCOMCfg: COM configuration to be set.
*
* Output Parameter: None
*
* Return: Set result
*
* Note: None
*
* Author Date: wangdong 2018.03.29
***********************************************************************************************************************
*/
int32_t BaseCfgSetBaseCOMCfg( uint16_t usCOMID, BaseCOMCfg_t const *pstBaseCOMCfg )
{
    int32_t iSetRet = (ECODE_PARA);  /* Get result */
    uint16_t usIndex = 0;
    BaseCOMCfgInfo_t *pstBaseCOMInfo = NULL;

    if(((usCOMID >= (SYS_COMID_MIN)) && (usCOMID <= (SYS_COMID_MAX))) &&
        (pstBaseCOMCfg != NULL))
    {
        usIndex =  usCOMID - (SYS_COMID_MIN);
        pstBaseCOMInfo = &s_stBaseCOMCfgInfo[usIndex];

        /* Copy in */
        memcpy( &pstBaseCOMInfo->stCOMCfg, pstBaseCOMCfg, sizeof(BaseCOMCfg_t));
        pstBaseCOMInfo->bNewCfg = true;
        iSetRet = (ECODE_OK);
    }

    return iSetRet;
}

/*
***********************************************************************************************************************
* Function Name: BaseCfgCheckBaseCOMCfg
*
* Description: Check base COM configuration
*
* Input Parameter: pstBCOMCfg: COM configuration array;
*                  ucCnts: array item counts.
*
* Output Parameter: None
*
* Return: Check result
*
* Note: None
*
* Author Date: wangdong 2018.03.29
***********************************************************************************************************************
*/
bool_t BaseCfgCheckBaseCOMCfg( BaseCOMCfg_t const pstBCOMCfg[], uint8_t ucCnts )
{
    return true;
}

/*
***********************************************************************************************************************
* Function Name: PlcCfgGetBasePort
*
* Description: Get baseboard port
*
* Input Parameter: uiCOMID: COMID of COM3-COM8
*
* Output Parameter: None
*
* Return: port, -1 - error
*
* Note: None
*
* Author Date: wangdong 2018.03.28
***********************************************************************************************************************
*/
int32_t PlcCfgGetBasePort( uint32_t uiCOMID )
{
    int32_t iBasePort = -1;

    if(( uiCOMID >= (SYS_ID_COM3)) && (uiCOMID <= (SYS_ID_COM8)))
    {
        iBasePort = BASE_PORT_COM3 + (uiCOMID - (SYS_ID_COM3));
    }

    return iBasePort;
}

/*
***********************************************************************************************************************
* Function Name: GetUSARTxFromCOMID
*
* Description: Get USARTx pointer from COMID
*
* Input Parameter: usCOMID: COMID of COM3-COM8
*
* Output Parameter: None
*
* Return: USARTx pointer, NULL - error
*
* Note: None
*
* Author Date: wangdong 2018.04.01
***********************************************************************************************************************
*/
USART_TypeDef* GetUSARTxFromCOMID( uint16_t usCOMID )
{
    USART_TypeDef *pstUSARTx = NULL;  /* USARTx */

    if((usCOMID >= (SYS_COMID_MIN)) && (usCOMID <= (SYS_COMID_MAX)))
    {
        switch( usCOMID )
        {
            case (SYS_ID_COM3):
            {
                pstUSARTx = USART1;
            }
            break;
            case (SYS_ID_COM4):
            {
                pstUSARTx = USART2;
            }
            break;
            case SYS_ID_COM5:
            {
                pstUSARTx = USART3;
            }
            break;
            case (SYS_ID_COM6):
            {
                pstUSARTx = UART4;
            }
            break;
            case (SYS_ID_COM7):
            {
                pstUSARTx = UART5;
            }
            break;
            case SYS_ID_COM8:
            {
                pstUSARTx = USART6;
            }
            break;
            default:
            {
                pstUSARTx = NULL;
            }
            break;
        }
    }

    return pstUSARTx;
}

/*
***********************************************************************************************************************
*                                Local Functions Realization
***********************************************************************************************************************
*/
/*
***********************************************************************************************************************
* Function Name: LoadCOMCfgFromFlash
*
* Description: Load all COM config info. from Flash
*
* Input Parameter: pucCfgBuff: COM configuration buffer;
*                  uiBuffLen: Buffer length.
*
* Output Parameter: None
*
* Return: Load result, true - success
*
* Note: None
*
* Author Date: wangdong 2018.03.29
***********************************************************************************************************************
*/

static bool_t LoadCOMCfgFromFlash( uint8_t pucCfgBuff[], uint32_t uiBuffLen )
{
    bool_t bRet = false;
    uint32_t uiFlashHead = 0U;

    if((pucCfgBuff != NULL) && (uiBuffLen > 0U))
    {
        /* Read head */
        bsp_W25QXX_Read((uint8_t *)&uiFlashHead, 0, sizeof(uint32_t));
        if((FLASH_DATA_HEAD) == uiFlashHead )
        {
            /* Flash has valid configuration */
            bsp_W25QXX_Read( &pucCfgBuff[0], sizeof(uint32_t), uiBuffLen );
            bRet = true;
        }
    }

    return bRet;
}

/*
***********************************************************************************************************************
* Function Name: StoreCOMCfgToFlash
*
* Description: Store all COM config info. to Flash
*
* Input Parameter: pucCfgData: COM configuration data;
*                  uiCfgLen: configuration data length.
*
* Output Parameter: None
*
* Return: Store result, true - success
*
* Note: None
*
* Author Date: wangdong 2018.05.15
***********************************************************************************************************************
*/

static bool_t StoreCOMCfgToFlash( uint8_t pucCfgData[], uint32_t uiCfgLen )
{
    bool_t bRet = false;
    uint32_t uiFlashHead = (FLASH_DATA_HEAD);

    if((pucCfgData != NULL) && (uiCfgLen > 0U))
    {
        /* Write head */
        bsp_W25QXX_Write((uint8_t *)&uiFlashHead, 0, sizeof(uint32_t));

        /* Write data */
        bsp_W25QXX_Write( &pucCfgData[0], sizeof(uint32_t), uiCfgLen );

        bRet = true;
    }

    return bRet;
}

/*
***********************************************************************************************************************
* Function Name: SetCOMPort
*
* Description: Set uart
*
* Input Parameter: usCOMID: ID of COM;
*                  pstCOMCfg: Config.
*
* Output Parameter: None
*
* Return: None
*
* Note: COM3-UART1, ... COM8-UART6
*
* Author Date: wangdong 2018.03.30
***********************************************************************************************************************
*/

static void SetCOMPort( uint16_t usCOMID, BaseCOMCfg_t const *pstCOMCfg )
{
    USART_InitTypeDef stUsartCfg;
    USART_TypeDef *pstUSARTx = NULL;

    if(((usCOMID >= (SYS_COMID_MIN)) && (usCOMID <= (SYS_COMID_MAX))) &&
       (pstCOMCfg != NULL))
    {
        /* Baud Rate */
        switch((COMBaud_t)pstCOMCfg->ucBaud )
        {
            case COM_BAUD_110:
            {
                stUsartCfg.USART_BaudRate = 110;
            }
            break;
            case COM_BAUD_300:
            {
                stUsartCfg.USART_BaudRate = 300;
            }
            break;
            case COM_BAUD_600:
            {
                stUsartCfg.USART_BaudRate = 600;
            }
            break;
            case COM_BAUD_1200:
            {
                stUsartCfg.USART_BaudRate = 1200;
            }
            break;
            case COM_BAUD_2400:
            {
                stUsartCfg.USART_BaudRate = 2400;
            }
            break;
            case COM_BAUD_4800:
            {
                stUsartCfg.USART_BaudRate = 4800;
            }
            break;
            case COM_BAUD_9600:
            {
                stUsartCfg.USART_BaudRate = 9600;
            }
            break;
            case COM_BAUD_19200:
            {
                stUsartCfg.USART_BaudRate = 19200;
            }
            break;
            case COM_BAUD_38400:
            {
                stUsartCfg.USART_BaudRate = 38400;
            }
            break;
            case COM_BAUD_57600:
            {
                stUsartCfg.USART_BaudRate = 57600;
            }
            break;
            case COM_BAUD_115200:
            {
                stUsartCfg.USART_BaudRate = 115200;
            }
            break;
            case COM_BAUD_230400:
            {
                stUsartCfg.USART_BaudRate = 230400;
            }
            break;
            default:
            {
                stUsartCfg.USART_BaudRate = 9600;  /* Default */
            }
            break;
        }

        /* Databit: only support 8/9 bits */
        switch((COMDataBit_t)pstCOMCfg->ucDatabit )
        {
            case COM_DB_8:
            {
                stUsartCfg.USART_WordLength = USART_WordLength_8b;
            }
            break;
            default:
            {
                stUsartCfg.USART_WordLength = USART_WordLength_8b;  /* Default */
            }
            break;
        }

        /* Stop bit */
        switch((COMStopBit_t)pstCOMCfg->ucStopbit )
        {
            case COM_SB_1:
            {
                stUsartCfg.USART_StopBits = USART_StopBits_1;
            }
            break;
            case COM_SB_2:
            {
                stUsartCfg.USART_StopBits = USART_StopBits_2;
            }
            break;
            default:
            {
                stUsartCfg.USART_StopBits = USART_StopBits_1;  /* Default */
            }
            break;
        }

        /* Parity */
        switch((COMParity_t)pstCOMCfg->ucParity )
        {
            case COM_PRITY_NONE:
            {
                stUsartCfg.USART_Parity = USART_Parity_No;
            }
            break;
            case COM_PRITY_EVEN:
            {
                stUsartCfg.USART_Parity = USART_Parity_Even;
            }
            break;
            case COM_PRITY_ODD:
            {
                stUsartCfg.USART_Parity = USART_Parity_Odd;
            }
            break;
            default:
            {
                stUsartCfg.USART_Parity = USART_Parity_No;  /* Default */
            }
            break;
        }

        /* Others */
        stUsartCfg.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        stUsartCfg.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

        /* Set */
        pstUSARTx = GetUSARTxFromCOMID( usCOMID );
        if( pstUSARTx != NULL )
        {
            UsartSetAttr( pstUSARTx, &stUsartCfg );
            UsartClrRecvInfo( pstUSARTx );
        }
    }

    return;
}

/*
***********************************************************************************************************************
                                 File End
***********************************************************************************************************************
*/

