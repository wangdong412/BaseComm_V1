#ifndef BASE_CONFIG_H_
#define BASE_CONFIG_H_

/*
***********************************************************************************************************************
*                                Head Files
***********************************************************************************************************************
*/

#include "DataType.h"
#include "Usart.h"

/*
***********************************************************************************************************************
*                                Macro Definition
***********************************************************************************************************************
*/
#define BASECFG_DEBUG     (0)

#define FLASH_DATA_HEAD   (0x12345678)

/* BaudRate */
typedef enum COMBaudTag
{
    COM_BAUD_110 = 1,
    COM_BAUD_300,
    COM_BAUD_600,
    COM_BAUD_1200,
    COM_BAUD_2400,
    COM_BAUD_4800,
    COM_BAUD_9600,  /* Default */
    COM_BAUD_19200,
    COM_BAUD_38400,
    COM_BAUD_57600,
    COM_BAUD_115200,
    COM_BAUD_230400
}COMBaud_t;

/* Data bit */
typedef enum COMDataBitTag
{
    COM_DB_5 = 1,
    COM_DB_6,
    COM_DB_7,
    COM_DB_8         /* Default */
}COMDataBit_t;

/* Stop bit */
typedef enum COMStopBitTag
{
    COM_SB_1 = 1,    /* Default */
    COM_SB_2
}COMStopBit_t;

/* Parity */
typedef enum COMParityTag
{
    COM_PRITY_NONE = 1,  /* Default */
    COM_PRITY_EVEN,
    COM_PRITY_ODD
}COMParity_t;

/*
***********************************************************************************************************************
*                                Data Structure Definition
***********************************************************************************************************************
*/

#pragma pack(1)

/* Baseboard COM configuration structure definition */
typedef struct BaseCOMCfgTag
{
    uint16_t usID;
    uint8_t ucBaud;
    uint8_t ucDatabit;
    uint8_t ucStopbit;
    uint8_t ucParity;
}BaseCOMCfg_t;

/* Baseboard COM configuration info. structure definition */
typedef struct BaseCOMCfgInfoTag
{
    bool_t bNewCfg;
    BaseCOMCfg_t stCOMCfg;
}BaseCOMCfgInfo_t;

#pragma pack()

/*
***********************************************************************************************************************
*                                Global Function Declaration
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
void ConfigTask( void *pdata );

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
void BaseCfgInit( void );

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
int32_t BaseCfgGetBaseCOMCfg( uint16_t usCOMID, BaseCOMCfg_t *pstBaseCOMCfg );

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
int32_t BaseCfgSetBaseCOMCfg( uint16_t usCOMID, BaseCOMCfg_t const *pstBaseCOMCfg );

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
bool_t BaseCfgCheckBaseCOMCfg( BaseCOMCfg_t const pstBCOMCfg[], uint8_t ucCnts );

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
int32_t PlcCfgGetBasePort( uint32_t uiCOMID );

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
USART_TypeDef* GetUSARTxFromCOMID( uint16_t usCOMID );

#endif /* BASE_CONFIG_H_ */

/*
***********************************************************************************************************************
                                 File End
***********************************************************************************************************************
*/
