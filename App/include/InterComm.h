#ifndef INTER_COMM_H_
#define INTER_COMM_H_

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
#define INTERCOMM_DEBUG  (0)

/* Port */
typedef enum BasePortTag
{
    BASE_PORT_CFG = 5500,  /* Used to config COM in baseboard */
    BASE_PORT_COM3,
    BASE_PORT_COM4,
    BASE_PORT_COM5,
    BASE_PORT_COM6,
    BASE_PORT_COM7,
    BASE_PORT_COM8
}BasePort_t;

/* Source ID */
typedef enum SrcIDTag
{
    SRC_ID_MIN = 0,
    SRC_ID_CFG = SRC_ID_MIN,
    SRC_ID_COM3,
    SRC_ID_COM4,
    SRC_ID_COM5,
    SRC_ID_COM6,
    SRC_ID_COM7,
    SRC_ID_COM8,
    SRC_ID_MAX = SRC_ID_COM8
}SrcID_t;

#define BASE_PORT_NUM    (7)
#define IC_BUFF_LEN      (512)

/*
***********************************************************************************************************************
*                                Data Structure Definition
***********************************************************************************************************************
*/

/* Request Info. */
typedef struct RecvInfoTag
{
    bool_t bDelay;
    uint8_t ucReserve[3];
    uint32_t uiDataLen;           /* Data length */
    uint8_t ucData[IC_BUFF_LEN];  /* Data */
}RecvInfo_t;

/* Request Info. */
typedef struct SrcReqInfoTag
{
    uint32_t uiDataLen;           /* Data length */
    uint8_t ucData[IC_BUFF_LEN];  /* Data */
}SrcReqInfo_t;

/* Response Info. */
typedef struct SrcRespInfoTag
{
    uint32_t uiDataLen;           /* Data length */
    uint8_t ucData[IC_BUFF_LEN];  /* Data */
}SrcRespInfo_t;


#pragma pack(1)

#pragma pack()

/*
***********************************************************************************************************************
*                                Global Function Declaration
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
void InterCommTask( void *pdata );

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
void InterCommInit( void );

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
void InterCommSetReqInfo( SrcID_t emSrcID, uint8_t const pucData[], uint32_t uiDataLen );

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
uint32_t InterCommGetReqInfo( SrcID_t emSrcID, uint8_t pucBuff[], uint32_t uiBuffLen );

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
void InterCommSetRespInfo( SrcID_t emSrcID, uint8_t const pucData[], uint32_t uiDataLen );

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
uint32_t InterCommGetRespInfo( SrcID_t emSrcID, uint8_t pucBuff[], uint32_t uiBuffLen );

#endif /* INTER_COMM_H_ */

/*
***********************************************************************************************************************
                                 File End
***********************************************************************************************************************
*/


