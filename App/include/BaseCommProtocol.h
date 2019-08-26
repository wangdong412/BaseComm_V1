#ifndef BASE_COMM_PRO_H_
#define BASE_COMM_PRO_H_

/*
***********************************************************************************************************************
*                                Head Files
***********************************************************************************************************************
*/

#include "DataType.h"

/*
***********************************************************************************************************************
*                                Macro Definition
***********************************************************************************************************************
*/

/* Message Type Range */
#define BCP_TYPE_MIN         (1)
#define BCP_TYPE_MAX         (100)

/* Current Message Type */
#define BCP_TYPE_W_COM_CFG   (1)
#define BCP_TYPE_R_COM_CFG   (2)

/* Length */
#define BCP_CRC_LEN    (sizeof(uint32_t))
#define BCP_REQ_HEAD   (sizeof(BCPReqHead_t))
#define BCP_RESP_HEAD  (sizeof(BCPRespHead_t))

/* Error code */
#define BCP_E_OK       (0)
#define BCP_E_PARA     (1)
#define BCP_E_CRC      (2)
#define BCP_E_START    (3)
#define BCP_E_TYPE     (4)
#define BCP_E_LEN      (5)
#define BCP_E_COM_CNT  (6)
#define BCP_E_COM_CFG  (7)
#define BCP_E_COMID    (8)

/*
***********************************************************************************************************************
*                                Data Structure Definition
***********************************************************************************************************************
*/

/* Request head */
typedef struct BCPReqHeadTag
{
    uint16_t usStart;        /* 0xAA55 */
    uint16_t usType;         /* Request type */
    uint8_t ucReserve[2];    /* Reserve */
    uint16_t usDataAreaLen;  /* Data area length */
}BCPReqHead_t;

/* Response head */
typedef struct BCPRespHeadTag
{
    uint16_t usStart;        /* 0x55AA */
    uint16_t usType;         /* Request type */
    uint8_t ucECode;         /* Error code: 0-ok */
    uint8_t ucReserve;       /* Reserve */
    uint16_t usDataAreaLen;  /* Data area length */
}BCPRespHead_t;


#pragma pack(1)

/* Request msg: 2 - Read COM Cfg */
typedef struct BCPReqReadCOMCfgTag
{
    BCPReqHead_t stHead;
    uint16_t usCOMID;
    uint32_t uiCRC;
}BCPReqReadCOMCfg_t;

#pragma pack()

/*
***********************************************************************************************************************
*                                Global Function Declaration
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

uint32_t BCPParseReq( uint8_t const ucReq[], uint32_t uiReqLen, uint8_t ucResp[], uint32_t uiRespBuffLen );


#endif /* BASE_COMM_PRO_H_ */

/*
***********************************************************************************************************************
                                 File End
***********************************************************************************************************************
*/


