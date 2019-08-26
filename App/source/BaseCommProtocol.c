
/*
***********************************************************************************************************************
*                                Head Files
***********************************************************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BaseCommProtocol.h"
#include "SysCommon.h"
#include "BaseConfig.h"

/*
***********************************************************************************************************************
*                                Local Variable(s) Declaration
***********************************************************************************************************************
*/

static uint16_t const s_usReqStart = 0xAA55;
static uint16_t const s_usRespStart = 0x55AA;
static uint32_t const s_uiMinReqLen = (BCP_REQ_HEAD) + (BCP_CRC_LEN);
static uint32_t const s_uiMinRespLen = (BCP_RESP_HEAD) + (BCP_CRC_LEN);

/* CRC32 table */
static uint32_t s_uiCrc32Table[256] =
{
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
    0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
    0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
    0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
    0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
    0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
    0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
    0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
    0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
    0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
    0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
    0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
    0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
    0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
    0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
    0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
    0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
    0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
    0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
    0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
    0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
    0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
    0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
    0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
    0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
    0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
    0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
    0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
    0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
    0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
    0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
    0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
    0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
    0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
    0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
    0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
    0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
    0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
    0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
    0x2d02ef8dL
};

/*
***********************************************************************************************************************
*                                Local Function(s) Declaration
***********************************************************************************************************************
*/
static uint32_t SysCrc32Cal( uint32_t uiCrc, uint8_t const pucBuf[], uint32_t uiCount );
static uint32_t BCPMakeRespWriteCOMCfg( uint8_t ucRespBuff[], uint32_t uiBuffLen, uint8_t ucECode );
static uint32_t BCPMakeRespReadCOMCfg( uint8_t ucRespBuff[], uint32_t uiBuffLen, uint8_t ucECode,
                                       BaseCOMCfg_t const pstBCOMCfg[], uint8_t ucItemCnt );
static uint32_t BCPParseReqWriteCOMCfg( uint8_t const ucReq[], uint32_t uiReqLen,
                                        uint8_t ucResp[], uint32_t uiRespBuffLen );
static uint32_t BCPParseReqReadCOMCfg( uint8_t const ucReq[], uint32_t uiReqLen,
                                       uint8_t ucResp[], uint32_t uiRespBuffLen );
static uint8_t CheckReqCommonValid( uint8_t const ucReq[], uint32_t uiReqLen );

/*
***********************************************************************************************************************
*                                Global Functions Realization
***********************************************************************************************************************
*/

/*
***********************************************************************************************************************
* Function Name: BCPParseReq
*
* Description: Parse request
*
* Input Parameter: ucReq: request msg;
*                  uiReqLen: request msg length;
*                  ucResp: response msg buffer;
*                  uiRespBuffLen: response msg buffer length.
*
* Output Parameter: None
*
* Return: response msg length, 0 - error.
*
* Note: None
*
* Author Date: wangdong 2018.03.29
***********************************************************************************************************************
*/

uint32_t BCPParseReq( uint8_t const ucReq[], uint32_t uiReqLen, uint8_t ucResp[], uint32_t uiRespBuffLen )
{
    uint8_t ucECode = (BCP_E_PARA);
    uint32_t uiRespLen = 0U;
    BCPReqHead_t const *pstReqHead = NULL;

    if((ucReq != NULL) && (uiReqLen >= s_uiMinReqLen) &&
       (ucResp != NULL) && (uiRespBuffLen >= s_uiMinRespLen ))
    {
        ucECode = CheckReqCommonValid( &ucReq[0], uiReqLen );
        if((BCP_E_OK) == ucECode )
        {
            pstReqHead = (BCPReqHead_t const *)&ucReq[0];
            switch( pstReqHead->usType )
            {
                case BCP_TYPE_W_COM_CFG:
                {
                    uiRespLen = BCPParseReqWriteCOMCfg( &ucReq[0], uiReqLen, &ucResp[0], uiRespBuffLen );
                }
                break;
                case BCP_TYPE_R_COM_CFG:
                {
                    uiRespLen = BCPParseReqReadCOMCfg( &ucReq[0], uiReqLen, &ucResp[0], uiRespBuffLen );
                }
                break;
                default:
                {
                    uiRespLen = 0U;
                }
                break;
            }
        }
    }

    return uiRespLen;
}

/*
***********************************************************************************************************************
*                                Local Functions Realization
***********************************************************************************************************************
*/

/*
***********************************************************************************************************************
* Function Name: SysCrc32Cal
*
* Description: Calculate CRC32
*
* Input Parameter: uiCrc: base value;
*                  pucBuf: start addr of data;
*                  uiCount: data length.
*
* Output Parameter: None
*
* Return: CRC32
*
* Note: None
*
* Author Date: wangdong 2018.03.27
***********************************************************************************************************************
*/

static uint32_t SysCrc32Cal( uint32_t uiCrc, uint8_t const pucBuf[], uint32_t uiCount )
{
    uint32_t uiRet = 0U;

    if( pucBuf != NULL )
    {
        uiCrc = uiCrc ^ 0xffffffffL;
        while( uiCount-- )
        {
            uiCrc = s_uiCrc32Table[((int)uiCrc ^ (*pucBuf++)) & 0xff] ^ (uiCrc >> 8);
        }

        uiRet = uiCrc ^ 0xffffffffL;
    }

    return uiRet;
}

/*
***********************************************************************************************************************
* Function Name: BCPMakeRespWriteCOMCfg
*
* Description: Make response: Type 1 - write com cfg
*
* Input Parameter: ucRespBuff: response buffer;
*                  uiBuffLen: response buffer length;
*                  ucECode: error code.
*
* Output Parameter: None
*
* Return: Response length, 0-failed
*
* Note: None
*
* Author Date: wangdong 2018.03.28
***********************************************************************************************************************
*/

static uint32_t BCPMakeRespWriteCOMCfg( uint8_t ucRespBuff[], uint32_t uiBuffLen, uint8_t ucECode )
{
    uint32_t uiRespLen = 0U;
    uint32_t uiCRC = 0U;
    BCPRespHead_t *pstRespHead = NULL;

    if((ucRespBuff != NULL) &&
       (uiBuffLen >= s_uiMinRespLen))
    {
        uiRespLen = sizeof(BCPRespHead_t);
        pstRespHead = (BCPRespHead_t *)(&ucRespBuff[0]);
        memset( pstRespHead, 0, uiRespLen );

        /* Head */
        pstRespHead->usStart = s_usRespStart;
        pstRespHead->usType = (BCP_TYPE_W_COM_CFG);
        pstRespHead->ucECode = ucECode;
        pstRespHead->usDataAreaLen = 0U;

        /* CRC */
        uiCRC = SysCrc32Cal( 0, &ucRespBuff[0], uiRespLen );
        memcpy( &ucRespBuff[uiRespLen], &uiCRC, (BCP_CRC_LEN));
        uiRespLen += (BCP_CRC_LEN);
    }

    return uiRespLen;
}

/*
***********************************************************************************************************************
* Function Name: BCPMakeRespReadCOMCfg
*
* Description: Make response: Type 2 - read com cfg
*
* Input Parameter: ucRespBuff: response buffer;
*                  uiBuffLen: response buffer length;
*                  ucECode: error code;
*                  pstBCOMCfg: config array;
*                  ucItemCnt: array item counts.
*
* Output Parameter: None
*
* Return: Response length, 0-failed
*
* Note: None
*
* Author Date: wangdong 2018.03.28
***********************************************************************************************************************
*/

static uint32_t BCPMakeRespReadCOMCfg( uint8_t ucRespBuff[], uint32_t uiBuffLen, uint8_t ucECode,
                                       BaseCOMCfg_t const pstBCOMCfg[], uint8_t ucItemCnt )
{
    uint32_t uiIndex = 0U;
    uint32_t uiRespLen = 0U;
    uint32_t uiCRC = 0U;
    BCPRespHead_t *pstRespHead = NULL;

    if((ucRespBuff != NULL) &&
       (uiBuffLen >= s_uiMinRespLen) &&
       (pstBCOMCfg != NULL) &&
       ((ucItemCnt > 0) && (ucItemCnt <= (SYS_COM_CNTS))))
    {
        /* Calc req len */
        uiRespLen = s_uiMinRespLen + sizeof(uint8_t) + (ucItemCnt * sizeof(BaseCOMCfg_t));

        /* Check buffer length */
        if( uiBuffLen >= uiRespLen )
        {
            pstRespHead = (BCPRespHead_t *)(&ucRespBuff[0]);
            memset( pstRespHead, 0, sizeof(BCPRespHead_t));

            /* Head */
            pstRespHead->usStart = s_usRespStart;
            pstRespHead->usType = (BCP_TYPE_R_COM_CFG);
            pstRespHead->ucECode = ucECode;
            pstRespHead->usDataAreaLen = uiRespLen - s_uiMinReqLen;

            /* Data area */
            uiIndex = sizeof(BCPRespHead_t);

            /* COM Counts */
            memcpy( &ucRespBuff[uiIndex], &ucItemCnt, sizeof(uint8_t));
            uiIndex += sizeof(uint8_t);

            /* COM Config */
            memcpy( &ucRespBuff[uiIndex], &pstBCOMCfg[0], ucItemCnt * sizeof(BaseCOMCfg_t));
            uiIndex += (ucItemCnt * sizeof(BaseCOMCfg_t));

            /* CRC */
            uiCRC = SysCrc32Cal( 0, &ucRespBuff[0], uiIndex );
            memcpy( &ucRespBuff[uiIndex], &uiCRC, (BCP_CRC_LEN));
        }
        else
        {
            uiRespLen = 0U;
        }
    }

    return uiRespLen;
}

/*
***********************************************************************************************************************
* Function Name: BCPParseReqWriteCOMCfg
*
* Description: Parse request: Type 1 - write com cfg
*
* Input Parameter: ucReq: request msg;
*                  uiReqLen: request msg length;
*                  ucResp: response msg buffer;
*                  uiRespBuffLen: response msg buffer length.
*
* Output Parameter: None
*
* Return: response msg length, 0 - error.
*
* Note: None
*
* Author Date: wangdong 2018.03.29
***********************************************************************************************************************
*/

static uint32_t BCPParseReqWriteCOMCfg( uint8_t const ucReq[], uint32_t uiReqLen,
                                        uint8_t ucResp[], uint32_t uiRespBuffLen )
{
    uint32_t uiIndex = 0U;
    uint8_t ucCnts = 0U;
    uint8_t i = 0U;
    uint8_t ucECode = (BCP_E_PARA);
    int32_t iRet = 0U;
    uint32_t uiRespLen = 0U;
    BaseCOMCfg_t const *pstCOMCfg = NULL;

    if((ucReq != NULL) && (uiReqLen >= s_uiMinReqLen) &&
       (ucResp != NULL) && (uiRespBuffLen >= s_uiMinRespLen ))
    {
        ucECode = CheckReqCommonValid( &ucReq[0], uiReqLen );
        if((BCP_E_OK) == ucECode )
        {
            /* COM Counts */
            uiIndex = sizeof( BCPReqHead_t );
            memcpy( &ucCnts, &ucReq[uiIndex], sizeof(uint8_t));
            uiIndex += sizeof(uint8_t);

            /* Check COM Counts */
            if((ucCnts > 0) && (ucCnts <= (SYS_COM_CNTS)))
            {
                /* Check COM config */
                if( BaseCfgCheckBaseCOMCfg((BaseCOMCfg_t const *)(&ucReq[uiIndex]), ucCnts ))
                {
                    /* Request message is valid */
                    ucECode = (BCP_E_OK);

                    /* Set COM config */
                    for( i = 0; i < ucCnts; i++ )
                    {
                        pstCOMCfg = (BaseCOMCfg_t const *)(&ucReq[uiIndex]);

                        /* Store in local */
                        iRet = BaseCfgSetBaseCOMCfg( pstCOMCfg->usID, pstCOMCfg );
                        if( iRet != (ECODE_OK))
                        {
                            printf( "E-BaseCfgSetBaseCOMCfg: COMID %d, E-Code 0x%x.\r\n", pstCOMCfg->usID, iRet );
                        }
                        else
                        {
                            printf( "BaseCfgSetBaseCOMCfg OK: COMID %d.\r\n", pstCOMCfg->usID );
                        }

                        uiIndex += sizeof(BaseCOMCfg_t);
                    }/* for */
                }
                else
                {
                    ucECode = (BCP_E_COM_CFG);
                    printf( "E-Req: COM config invalid.\r\n" );
                }
            }
            else
            {
                ucECode = (BCP_E_COM_CNT);
                printf( "E-Req: COM counts-%d invalid.\r\n", ucCnts );
            }
        }

        /* Make response */
        uiRespLen = BCPMakeRespWriteCOMCfg( &ucResp[0], uiRespBuffLen, ucECode );
    }

    return uiRespLen;
}

/*
***********************************************************************************************************************
* Function Name: BCPParseReqReadCOMCfg
*
* Description: Parse request: Type 2 - read com cfg
*
* Input Parameter: ucReq: request msg;
*                  uiReqLen: request msg length;
*                  ucResp: response msg buffer;
*                  uiRespBuffLen: response msg buffer length.
*
* Output Parameter: None
*
* Return: response msg length, 0 - error.
*
* Note: None
*
* Author Date: wangdong 2018.03.29
***********************************************************************************************************************
*/

static uint32_t BCPParseReqReadCOMCfg( uint8_t const ucReq[], uint32_t uiReqLen,
                                       uint8_t ucResp[], uint32_t uiRespBuffLen )
{
    uint32_t uiIndex = 0U;
    uint16_t usCOMID = 0U;
    uint16_t usStartCOMID = 0U;
    uint8_t ucCnts = 0U;
    uint8_t i = 0U;
    uint8_t ucECode = (BCP_E_PARA);
    uint32_t uiRespLen = 0U;
    BaseCOMCfg_t stCOMCfg[SYS_COM_CNTS] = { NULL };

    if((ucReq != NULL) && (uiReqLen >= s_uiMinReqLen) &&
       (ucResp != NULL) && (uiRespBuffLen >= s_uiMinRespLen ))
    {
        ucECode = CheckReqCommonValid( &ucReq[0], uiReqLen );
        if((BCP_E_OK) == ucECode )
        {
            /* COMID */
            uiIndex = sizeof( BCPReqHead_t );
            memcpy( &usCOMID, &ucReq[uiIndex], sizeof(uint16_t));
            uiIndex += sizeof(uint16_t);

            /* Check COMID */
            if(((SYS_ID_COMALL) == usCOMID ) ||
               ((usCOMID >= (SYS_COMID_MIN)) && (usCOMID <= (SYS_COMID_MAX))))
            {
                /* Request message is valid */
                ucECode = (BCP_E_OK);

                /* Get COMCfg according to the request COMID */
                if((SYS_ID_COMALL) == usCOMID )
                {
                    ucCnts = (SYS_COM_CNTS);
                    usStartCOMID = (SYS_COMID_MIN);
                }
                else
                {
                    ucCnts = 1;
                    usStartCOMID = usCOMID;
                }

                /* Get COMCfg */
                for( i = 0U; i < ucCnts; i++ )
                {
                    if( BaseCfgGetBaseCOMCfg( usStartCOMID+i, &stCOMCfg[i] ) != (ECODE_OK))
                    {
                        printf( "BaseCfgGetBaseCOMCfg failed: COMID-%d.\r\n", usStartCOMID+i );
                    }
                }
            }
            else
            {
                ucECode = (BCP_E_COMID);
                printf( "E-Req: COMID-%d invalid.\r\n", usCOMID );
            }
        }

        /* Make response */
        uiRespLen = BCPMakeRespReadCOMCfg( &ucResp[0], uiRespBuffLen, ucECode, &stCOMCfg[0], ucCnts );
    }

    return uiRespLen;
}

/*
***********************************************************************************************************************
* Function Name: CheckReqCommonValid
*
* Description: CRC, Start, Type, Data length
*
* Input Parameter: ucReq: response msg;
*                  uiReqLen: response msg length.
*
* Output Parameter: None
*
* Return: Error code
*
* Note: None
*
* Author Date: wangdong 2018.03.29
***********************************************************************************************************************
*/

static uint8_t CheckReqCommonValid( uint8_t const ucReq[], uint32_t uiReqLen )
{
    uint8_t ucECode = (BCP_E_PARA);
    uint32_t uiCRC = 0U;
    uint32_t uiReqCRC = 0U;
    uint32_t uiIndex = 0U;
    BCPReqHead_t const *pstReqHead = NULL;

    if((ucReq != NULL) && (uiReqLen >= s_uiMinReqLen ))
    {
        /* CRC check */
        uiIndex = uiReqLen - (BCP_CRC_LEN);
        uiCRC = SysCrc32Cal( 0, &ucReq[0], uiIndex );
        memcpy( &uiReqCRC, &ucReq[uiIndex], (BCP_CRC_LEN));
        if( uiCRC != uiReqCRC )  /* Test */
        {
            ucECode = (BCP_E_CRC);
            printf( "E-Req: CRC 0x%x != 0x%x.\r\n", uiReqCRC, uiCRC );
        }
        else
        {
            pstReqHead = (BCPReqHead_t const *)&ucReq[0];

            /* Start */
            if( pstReqHead->usStart != s_usReqStart )
            {
                ucECode = (BCP_E_START);
                printf( "E-Req: Start 0x%x.\r\n", pstReqHead->usStart );
            }
            else
            {
                /* Type */
                if((pstReqHead->usType < (BCP_TYPE_MIN)) || (pstReqHead->usType > (BCP_TYPE_MAX)))
                {
                    ucECode = (BCP_E_TYPE);
                    printf( "E-Req: Type %d.\r\n", pstReqHead->usType );
                }
                else
                {
                    /* DataArea Length */
                    if( pstReqHead->usDataAreaLen + s_uiMinReqLen != uiReqLen )
                    {
                        ucECode = (BCP_E_LEN);
                        printf( "E-Req: DataAreaLen %d, ReqLen %d.\r\n", pstReqHead->usDataAreaLen, uiReqLen );
                    }
                    else
                    {
                        ucECode = (BCP_E_OK);
                    }
                }
            }
        }
    }

    return ucECode;
}


/*
***********************************************************************************************************************
                                 File End
***********************************************************************************************************************
*/

