#ifndef __USART_H__
#define __USART_H__

/*
***********************************************************************************************************************
*                                Head Files
***********************************************************************************************************************
*/
#include "stm32f4xx.h"
#include "DataType.h"

/*
***********************************************************************************************************************
*                                Macro Definition
***********************************************************************************************************************
*/
#define USART_DEBUG          (0)

#define SERL_PORT_NUM        (6)    /* USART1-3, UART4-5, USART6 */

#define SERL_RECV_BUFF_LEN   (512)  /* Receive buffer length */

#define TIMER_TO_CNTS        (2)    /* 2*10=20ms */

/*
***********************************************************************************************************************
*                                Data Structure Definition
***********************************************************************************************************************
*/

/* Serial receive info. */
typedef struct SerlRecvInfoTag
{
    uint16_t usNonFinLen;   /* When single message finished, add to usLen */
    uint16_t usLen;         /* Length of not read data */
    uint16_t usReadOffset;  /* Index of the first byte to be read */
    uint16_t usStoreIndex;  /* Index of the first position to be write */
    uint8_t s_ucBuff[SERL_RECV_BUFF_LEN];  /* Buffer */
}SerlRecvInfo_t;

/*
***********************************************************************************************************************
*                                Global Function Declaration
***********************************************************************************************************************
*/

/*
***********************************************************************************************************************
* Function Name: UsartInit
*
* Description: Initialize pins
*
* Input Parameter: pstUSARTx: USARTx.
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.03.30
***********************************************************************************************************************
*/
void UsartInit( USART_TypeDef *pstUSARTx );

/*
***********************************************************************************************************************
* Function Name: UsartSetAttr
*
* Description: Set usart
*
* Input Parameter: pstUSARTx: USARTx;
*                  pstUsartCfg: Config.
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.03.30
***********************************************************************************************************************
*/
void UsartSetAttr( USART_TypeDef *pstUSARTx, USART_InitTypeDef *pstUsartCfg );

/*
***********************************************************************************************************************
* Function Name: UsartRead
*
* Description: Read
*
* Input Parameter: pstUSARTx: USARTx;
*                  pucBuff: Data buffer;
*                  uiCount: Buffer size.
*
* Output Parameter: None
*
* Return: Actually read length, -1 is failed
*
* Note: None
*
* Author Date: wangdong 2018.04.01
***********************************************************************************************************************
*/
int32_t UsartRead( USART_TypeDef *pstUSARTx, uint8_t pucBuff[], uint32_t uiCount );

/*
***********************************************************************************************************************
* Function Name: UsartWrite
*
* Description: Write
*
* Input Parameter: pstUSARTx: USARTx;
*                  pucData: Data need to be writed;
*                  uiCount: Data size.
*
* Output Parameter: None
*
* Return: Actually write length, -1 is failed
*
* Note: None
*
* Author Date: wangdong 2018.04.01
***********************************************************************************************************************
*/
int32_t UsartWrite( USART_TypeDef *pstUSARTx, uint8_t const pucData[], uint32_t uiCount );

/*
***********************************************************************************************************************
* Function Name: UsartUpdateRxLen
*
* Description: Update rx length
*
* Input Parameter: pstUSARTx: USARTx
*
* Output Parameter: None
*
* Return: None
*
* Note: None
*
* Author Date: wangdong 2018.04.08
***********************************************************************************************************************
*/
void UsartUpdateRxLen( USART_TypeDef *pstUSARTx );

/*
***********************************************************************************************************************
* Function Name: UsartClrRecvBuff
*
* Description: Clear receive buffer
*
* Input Parameter: pstUSARTx: USARTx
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
void UsartClrRecvInfo( USART_TypeDef *pstUSARTx );

/*
***********************************************************************************************************************
*                                Global Variable(s) Declaration
***********************************************************************************************************************
*/
extern uint32_t g_uiRxTOCnt[SERL_PORT_NUM];  /* Timer timeout count */

#endif /* __USART_H__ */

/*
***********************************************************************************************************************
                                 File End
***********************************************************************************************************************
*/
