/*
***********************************************************************************************************************
*                                Head Files
***********************************************************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Usart.h"
#include "queue.h"
#include "delay.h"
#include "app.h"


/*
***********************************************************************************************************************
*                                Global Variable(s) Declaration
***********************************************************************************************************************
*/
uint32_t g_uiRxTOCnt[SERL_PORT_NUM];  /* Timer timeout count */

/*
***********************************************************************************************************************
*                                Local Variable(s) Declaration
***********************************************************************************************************************
*/

static SerlRecvInfo_t s_stSerlRecvInfo[SERL_PORT_NUM];

/*
***********************************************************************************************************************
*                                Local Function(s) Declaration
***********************************************************************************************************************
*/

static void SerlRecvIRQ( USART_TypeDef *pstUSARTx, SerlRecvInfo_t *pstRecvInfo, uint32_t *puiRxTOCnt );
static SerlRecvInfo_t* GetSerlRecvInfo( USART_TypeDef *pstUSARTx );

/*
***********************************************************************************************************************
*                                Global Functions Realization
***********************************************************************************************************************
*/

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
#if USART_DEBUG
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData( USART6, (uint8_t)ch );

  /* Loop until the end of transmission */
  while( USART_GetFlagStatus( USART6, USART_FLAG_TC ) == RESET )
  {}
#endif

  return ch;
}

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

void UsartInit( USART_TypeDef *pstUSARTx )
{
    bool_t bValid = true;
    GPIO_InitTypeDef stGPIOInit;
    NVIC_InitTypeDef stNVICInit;
    SerlRecvInfo_t *pstSerlRecvInfo = NULL;
    USART_InitTypeDef stUsartCfg;
    uint8_t ucIndex = 0;  /* Serial port index, USART1 - 0 */

    if( pstUSARTx != NULL )
    {
        if( USART1 == pstUSARTx )
        {
            /* PA9-TX, PA10-RX, PA8-RS485 Recv/Send Ctrl
             * Enable GPIOA clock */
            RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE );

            /* Enable USART1 clock */
            RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE );

            /* Changes the mapping of the specified pin */
            GPIO_PinAFConfig( GPIOA, GPIO_PinSource9,  GPIO_AF_USART1 );  /* PA9:  TX */
            GPIO_PinAFConfig( GPIOA, GPIO_PinSource10, GPIO_AF_USART1 );  /* PA10: RX */

            /* Configure USART Tx and Rx as alternate function push-pull */
            stGPIOInit.GPIO_Mode = GPIO_Mode_AF;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;  /* PA9, PA10 */
            GPIO_Init( GPIOA, &stGPIOInit );

            /* Initialize NVIC for USART */
            stNVICInit.NVIC_IRQChannel = USART1_IRQn;
            stNVICInit.NVIC_IRQChannelPreemptionPriority = NVIC_USART1_PRE;
            stNVICInit.NVIC_IRQChannelSubPriority = NVIC_USART1_SUB;
            stNVICInit.NVIC_IRQChannelCmd = ENABLE;

            /* Receive info. */
            pstSerlRecvInfo = &s_stSerlRecvInfo[0];

            /* RS485 Recv/Send Ctrl: 1 - send; 0 - receive */
            stGPIOInit.GPIO_Mode = GPIO_Mode_OUT;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_8;
            GPIO_Init( GPIOA, &stGPIOInit );    /* PA8 */

            GPIO_SetBits( GPIOA, GPIO_Pin_8 );

            ucIndex = 0;
        }
        else if( USART2 == pstUSARTx )
        {
            /* PD5-TX, PD6-RX, PD7-RS485 Recv/Send Ctrl
             * Enable GPIOD clock */
            RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE );

            /* Enable USART clock */
            RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE );

            /* Changes the mapping of the specified pin */
            GPIO_PinAFConfig( GPIOD, GPIO_PinSource5,  GPIO_AF_USART2 );  /* PD5: TX */
            GPIO_PinAFConfig( GPIOD, GPIO_PinSource6, GPIO_AF_USART2 );   /* PD6: RX */

            /* Configure USART Tx and Rx as alternate function push-pull */
            stGPIOInit.GPIO_Mode = GPIO_Mode_AF;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;  /* PD5, PD6 */
            GPIO_Init( GPIOD, &stGPIOInit );

            /* Initialize NVIC for USART */
            stNVICInit.NVIC_IRQChannel = USART2_IRQn;
            stNVICInit.NVIC_IRQChannelPreemptionPriority = NVIC_USART2_PRE;
            stNVICInit.NVIC_IRQChannelSubPriority = NVIC_USART2_SUB;
            stNVICInit.NVIC_IRQChannelCmd = ENABLE;

            /* Receive info. */
            pstSerlRecvInfo = &s_stSerlRecvInfo[1];

            /* RS485 Recv/Send Ctrl: 1 - send; 0 - receive */
            stGPIOInit.GPIO_Mode = GPIO_Mode_OUT;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_7;
            GPIO_Init( GPIOD, &stGPIOInit );    /* PD7 */

            GPIO_SetBits( GPIOD, GPIO_Pin_7 );

            ucIndex = 1;
        }
        else if( USART3 == pstUSARTx )
        {
            /* PD8-TX, PD9-RX, PE8-RS485 Recv/Send Ctrl
             * Enable GPIOD clock */
            RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE );

            /* Enable USART clock */
            RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE );

            /* Changes the mapping of the specified pin */
            GPIO_PinAFConfig( GPIOD, GPIO_PinSource8,  GPIO_AF_USART3 );  /* PD8: TX */
            GPIO_PinAFConfig( GPIOD, GPIO_PinSource9, GPIO_AF_USART3 );   /* PD9: RX */

            /* Configure USART Tx and Rx as alternate function push-pull */
            stGPIOInit.GPIO_Mode = GPIO_Mode_AF;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;   /* PD8, PD9 */
            GPIO_Init( GPIOD, &stGPIOInit );

            /* Initialize NVIC for USART */
            stNVICInit.NVIC_IRQChannel = USART3_IRQn;
            stNVICInit.NVIC_IRQChannelPreemptionPriority = NVIC_USART3_PRE;
            stNVICInit.NVIC_IRQChannelSubPriority = NVIC_USART3_SUB;
            stNVICInit.NVIC_IRQChannelCmd = ENABLE;

            /* Receive info. */
            pstSerlRecvInfo = &s_stSerlRecvInfo[2];

            /* RS485 Recv/Send Ctrl: 1 - send; 0 - receive */
            stGPIOInit.GPIO_Mode = GPIO_Mode_OUT;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_8;
            GPIO_Init( GPIOE, &stGPIOInit );    /* PE8 */

            GPIO_SetBits( GPIOE, GPIO_Pin_8 );

            ucIndex = 2;
        }
        else if( UART4 == pstUSARTx )
        {
            /* PC10-TX, PC11-RX, PE9-RS485 Recv/Send Ctrl
             * Enable GPIOC clock */
            RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE );

            /* Enable USART clock */
            RCC_APB1PeriphClockCmd( RCC_APB1Periph_UART4, ENABLE );

            /* Changes the mapping of the specified pin */
            GPIO_PinAFConfig( GPIOC, GPIO_PinSource10,  GPIO_AF_UART4 );  /* PC10: TX */
            GPIO_PinAFConfig( GPIOC, GPIO_PinSource11, GPIO_AF_UART4 );   /* PC11: RX */

            /* Configure USART Tx and Rx as alternate function push-pull */
            stGPIOInit.GPIO_Mode = GPIO_Mode_AF;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;  /* PC10, PC11 */
            GPIO_Init( GPIOC, &stGPIOInit );

            /* Initialize NVIC for USART */
            stNVICInit.NVIC_IRQChannel = UART4_IRQn;
            stNVICInit.NVIC_IRQChannelPreemptionPriority = NVIC_UART4_PRE;
            stNVICInit.NVIC_IRQChannelSubPriority = NVIC_UART4_SUB;
            stNVICInit.NVIC_IRQChannelCmd = ENABLE;

            /* Receive info. */
            pstSerlRecvInfo = &s_stSerlRecvInfo[3];

            /* RS485 Recv/Send Ctrl: 1 - send; 0 - receive */
            stGPIOInit.GPIO_Mode = GPIO_Mode_OUT;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_9;
            GPIO_Init( GPIOE, &stGPIOInit );    /* PE9 */

            GPIO_SetBits( GPIOE, GPIO_Pin_9 );

            ucIndex = 3;
        }
        else if( UART5 == pstUSARTx )
        {
            /* PC12-TX, PD02-RX, PG0-RS485 Recv/Send Ctrl
             * Enable GPIOC, GPIOD clock */
            RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOG, ENABLE );

            /* Enable USART clock */
            RCC_APB1PeriphClockCmd( RCC_APB1Periph_UART5, ENABLE );

            /* Changes the mapping of the specified pin */
            GPIO_PinAFConfig( GPIOC, GPIO_PinSource12,  GPIO_AF_UART5 );  /* PC12: TX */
            GPIO_PinAFConfig( GPIOD, GPIO_PinSource2, GPIO_AF_UART5 );    /* PD02: RX */

            /* Configure USART Tx and Rx as alternate function push-pull */
            stGPIOInit.GPIO_Mode = GPIO_Mode_AF;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_12;
            GPIO_Init( GPIOC, &stGPIOInit );    /* PC12 */

            stGPIOInit.GPIO_Pin = GPIO_Pin_2;
            GPIO_Init( GPIOD, &stGPIOInit );    /* PD02 */

            /* Initialize NVIC for USART */
            stNVICInit.NVIC_IRQChannel = UART5_IRQn;
            stNVICInit.NVIC_IRQChannelPreemptionPriority = NVIC_UART5_PRE;
            stNVICInit.NVIC_IRQChannelSubPriority = NVIC_UART5_SUB;
            stNVICInit.NVIC_IRQChannelCmd = ENABLE;

            /* Receive info. */
            pstSerlRecvInfo = &s_stSerlRecvInfo[4];

            /* RS485 Recv/Send Ctrl: 1 - send; 0 - receive */
            stGPIOInit.GPIO_Mode = GPIO_Mode_OUT;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_0;
            GPIO_Init( GPIOG, &stGPIOInit );    /* PG0 */

            GPIO_SetBits( GPIOG, GPIO_Pin_0 );

            ucIndex = 4;
        }
        else if( USART6 == pstUSARTx )
        {
            /* PC6-TX, PC7-RX, PE7-RS485 Recv/Send Ctrl
             * Enable GPIOC clock */
            RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE );

            /* Enable USART clock */
            RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART6, ENABLE );

            /* Changes the mapping of the specified pin */
            GPIO_PinAFConfig( GPIOC, GPIO_PinSource6,  GPIO_AF_USART6 );  /* PC6: TX */
            GPIO_PinAFConfig( GPIOC, GPIO_PinSource7, GPIO_AF_USART6 );   /* PC7: RX */

            /* Configure USART Tx and Rx as alternate function push-pull */
            stGPIOInit.GPIO_Mode = GPIO_Mode_AF;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  /* PC6, PC7 */
            GPIO_Init( GPIOC, &stGPIOInit );

            /* Initialize NVIC for USART */
            stNVICInit.NVIC_IRQChannel = USART6_IRQn;
            stNVICInit.NVIC_IRQChannelPreemptionPriority = NVIC_USART6_PRE;
            stNVICInit.NVIC_IRQChannelSubPriority = NVIC_USART6_SUB;
            stNVICInit.NVIC_IRQChannelCmd = ENABLE;

            /* Receive info. */
            pstSerlRecvInfo = &s_stSerlRecvInfo[5];

            /* RS485 Recv/Send Ctrl: 1 - send; 0 - receive */
            stGPIOInit.GPIO_Mode = GPIO_Mode_OUT;
            stGPIOInit.GPIO_Speed = GPIO_Speed_100MHz;
            stGPIOInit.GPIO_OType = GPIO_OType_PP;
            stGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
            stGPIOInit.GPIO_Pin = GPIO_Pin_7;
            GPIO_Init( GPIOE, &stGPIOInit );    /* PE7 */

            GPIO_SetBits( GPIOE, GPIO_Pin_7 );

            ucIndex = 5;
        }
        else
        {
            bValid = false;
        }

        if( bValid )
        {
            /* Receive info. */
            memset( pstSerlRecvInfo, 0, sizeof(SerlRecvInfo_t));

            if( ucIndex < SERL_PORT_NUM )
            {
                g_uiRxTOCnt[ucIndex] = 0U;
            }

            /* Attr: Default Baud... */
            stUsartCfg.USART_BaudRate = 9600;
            stUsartCfg.USART_WordLength = USART_WordLength_8b;
            stUsartCfg.USART_StopBits = USART_StopBits_1;
            stUsartCfg.USART_Parity = USART_Parity_No;
            stUsartCfg.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            stUsartCfg.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
            UsartSetAttr( pstUSARTx, &stUsartCfg );

            /* Interrupt */
            /* Enable receive interrupt */
            USART_ITConfig( pstUSARTx, USART_IT_RXNE, ENABLE );

            /* Configure the USART interrupt priority, and enable it */
            NVIC_Init( &stNVICInit );
        }
    }

    return;
}

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
void UsartSetAttr( USART_TypeDef *pstUSARTx, USART_InitTypeDef *pstUsartCfg )
{
    if((pstUSARTx != NULL) && (pstUsartCfg != NULL))
    {
        //printf( "UsartSetAttr\r\n" );

        /* Disable */
        USART_Cmd( pstUSARTx, DISABLE);

        /* Set */
        /* Enable the USART OverSampling by 8: used for float-point baud rate */
        USART_OverSampling8Cmd( pstUSARTx, ENABLE );

        /* Init */
        USART_Init( pstUSARTx, pstUsartCfg );

        /* Enable */
        USART_Cmd( pstUSARTx, ENABLE );
    }

    return;
}

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
int32_t UsartRead( USART_TypeDef *pstUSARTx, uint8_t pucBuff[], uint32_t uiCount )
{
    int32_t iRet = -1;
    uint32_t uiFirNum = 0U;
    uint16_t usOffset = 0U;
    SerlRecvInfo_t *pstRecvInfo = NULL;
    OS_CPU_SR cpu_sr;

    if((pstUSARTx != NULL) && (pucBuff != NULL))
    {
        pstRecvInfo = GetSerlRecvInfo( pstUSARTx );
        if( pstRecvInfo != NULL )
        {
            OS_ENTER_CRITICAL();

            /* Copy out */
            if( uiCount > (uint32_t)pstRecvInfo->usLen )
            {
                uiCount = (uint32_t)pstRecvInfo->usLen;
            }

            usOffset = pstRecvInfo->usReadOffset;
            if( (uint32_t)usOffset + uiCount <= (SERL_RECV_BUFF_LEN))
            {
                memcpy( &pucBuff[0], &pstRecvInfo->s_ucBuff[usOffset], uiCount );
            }
            else
            {
                uiFirNum = (SERL_RECV_BUFF_LEN) - (uint32_t)usOffset;
                memcpy( &pucBuff[0], &pstRecvInfo->s_ucBuff[usOffset], uiFirNum );
                memcpy( &pucBuff[uiFirNum], &pstRecvInfo->s_ucBuff[0], uiCount - uiFirNum );
            }

            pstRecvInfo->usReadOffset += uiCount;
            if( pstRecvInfo->usReadOffset >= (SERL_RECV_BUFF_LEN))
            {
                pstRecvInfo->usReadOffset %= (SERL_RECV_BUFF_LEN);
            }
            pstRecvInfo->usLen -= uiCount;
            iRet = (int32_t)uiCount;

            OS_EXIT_CRITICAL();
        }
    }

    return iRet;
}

/*
***********************************************************************************************************************
* Function Name: UsartWrite
*
* Description: Write: block, wait until all data were write successfully
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
int32_t UsartWrite( USART_TypeDef *pstUSARTx, uint8_t const pucData[], uint32_t uiCount )
{
    int32_t iRet = -1;
    uint32_t i = 0U;
    uint16_t usPin = 0U;
    GPIO_TypeDef *pstGPIOx = NULL;

    if((pstUSARTx != NULL) && (pucData != NULL))
    {
        if( USART1 == pstUSARTx )
        {
            /* PA8 */
            pstGPIOx = GPIOA;
            usPin = GPIO_Pin_8;
        }
        else if( USART2 == pstUSARTx )
        {
            /* PD7 */
            pstGPIOx = GPIOD;
            usPin = GPIO_Pin_7;
        }
        else if( USART3 == pstUSARTx )
        {
            /* PE8 */
            pstGPIOx = GPIOE;
            usPin = GPIO_Pin_8;
        }
        else if( UART4 == pstUSARTx )
        {
            /* PE9 */
            pstGPIOx = GPIOE;
            usPin = GPIO_Pin_9;
        }
        else if( UART5 == pstUSARTx )
        {
            /* PG0 */
            pstGPIOx = GPIOG;
            usPin = GPIO_Pin_0;
        }
        else if( USART6 == pstUSARTx )
        {
            /* PE7 */
            pstGPIOx = GPIOE;
            usPin = GPIO_Pin_7;
        }
        else  /* Invalid */
        {
            pstGPIOx = NULL;
            usPin = 0;
        }

        /* Test
        if( USART_GetFlagStatus( pstUSARTx, USART_FLAG_TC ) != RESET )
        {
            printf( "TC 1\r\n" );  // result is 1
        }*/

        if((pstGPIOx != NULL) && (usPin != 0U))
        {
            /* RS485 Recv/Send Ctrl: 1 - send */
            GPIO_SetBits( pstGPIOx, usPin );

            for( i = 0U; i < uiCount; i++ )
            {
                /* Loop until the end of transmission */
                while( RESET == USART_GetFlagStatus( pstUSARTx, USART_FLAG_TC ))
                {

                }

                USART_SendData( pstUSARTx, (uint16_t)pucData[i]);
            }

            /* Loop until the end of transmission */
            while( RESET == USART_GetFlagStatus( pstUSARTx, USART_FLAG_TC ))
            {

            }

            /* RS485 Recv/Send Ctrl: 0 - receive */
            GPIO_ResetBits( pstGPIOx, usPin );

            iRet = (int32_t)i;
        }
    }

    return iRet;
}

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
void UsartUpdateRxLen( USART_TypeDef *pstUSARTx )
{
    OS_CPU_SR cpu_sr;
    SerlRecvInfo_t *pstRecvInfo = NULL;

    if( pstUSARTx != NULL )
    {
        pstRecvInfo = GetSerlRecvInfo( pstUSARTx );
        if( pstRecvInfo != NULL )
        {
            OS_ENTER_CRITICAL();

            pstRecvInfo->usLen += pstRecvInfo->usNonFinLen;
            pstRecvInfo->usNonFinLen = 0;

            OS_EXIT_CRITICAL();
        }
    }

    return;
}

/*
***********************************************************************************************************************
* Function Name: UsartClrRecvInfo
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
void UsartClrRecvInfo( USART_TypeDef *pstUSARTx )
{
    SerlRecvInfo_t *pstRecvInfo = NULL;
    OS_CPU_SR cpu_sr;

    if( pstUSARTx != NULL )
    {
        pstRecvInfo = GetSerlRecvInfo( pstUSARTx );
        if( pstRecvInfo != NULL )
        {
            //printf( "UsartClrRecvInfo\r\n" );
            OS_ENTER_CRITICAL();

            pstRecvInfo->usNonFinLen = 0U;
            pstRecvInfo->usLen = 0U;
            pstRecvInfo->usReadOffset = 0U;
            pstRecvInfo->usStoreIndex = 0U;

            OS_EXIT_CRITICAL();
        }
    }

    return;
}

/*
***********************************************************************************************************************
* Function Name: USART1_IRQHandler
*
* Description: USART1 IRQ: Receive interrupt handler
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
void USART1_IRQHandler( void )
{
    SerlRecvIRQ( USART1, &s_stSerlRecvInfo[0], &g_uiRxTOCnt[0]);

    return;
}

/*
***********************************************************************************************************************
* Function Name: USART2_IRQHandler
*
* Description: USART2 IRQ: Receive interrupt handler
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
void USART2_IRQHandler( void )
{
    SerlRecvIRQ( USART2, &s_stSerlRecvInfo[1], &g_uiRxTOCnt[1]);

    return;
}

/*
***********************************************************************************************************************
* Function Name: USART3_IRQHandler
*
* Description: USART3 IRQ: Receive interrupt handler
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
void USART3_IRQHandler( void )
{
    SerlRecvIRQ( USART3, &s_stSerlRecvInfo[2], &g_uiRxTOCnt[2]);

    return;
}

/*
***********************************************************************************************************************
* Function Name: UART4_IRQHandler
*
* Description: UART4 IRQ: Receive interrupt handler
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
void UART4_IRQHandler( void )
{
    SerlRecvIRQ( UART4, &s_stSerlRecvInfo[3], &g_uiRxTOCnt[3]);

    return;
}

/*
***********************************************************************************************************************
* Function Name: UART5_IRQHandler
*
* Description: UART5 IRQ: Receive interrupt handler
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
void UART5_IRQHandler( void )
{
    SerlRecvIRQ( UART5, &s_stSerlRecvInfo[4], &g_uiRxTOCnt[4]);

    return;
}

/*
***********************************************************************************************************************
* Function Name: USART6_IRQHandler
*
* Description: USART6 IRQ: Receive interrupt handler
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
void USART6_IRQHandler( void )
{
    SerlRecvIRQ( USART6, &s_stSerlRecvInfo[5], &g_uiRxTOCnt[5]);

    return;
}

/*
***********************************************************************************************************************
*                                Local Functions Realization
***********************************************************************************************************************
*/

/*
***********************************************************************************************************************
* Function Name: SerlRecvIRQ
*
* Description: Receive new byte, when overflow, discard
*
* Input Parameter: pstUSARTx
*
* Output Parameter: None
*
* Return: Receive info., NULL - get failed
*
* Note: None
*
* Author Date: wangdong 2018.04.01
***********************************************************************************************************************
*/
static void SerlRecvIRQ( USART_TypeDef *pstUSARTx, SerlRecvInfo_t *pstRecvInfo, uint32_t *puiRxTOCnt )
{
    uint8_t ucData = 0U;
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();
    OSIntEnter();

    if( USART_GetITStatus( pstUSARTx, USART_IT_RXNE ) != RESET )  /* receive interrupt */
    {
        ucData = (uint8_t)USART_ReceiveData( pstUSARTx );
        //USART_ClearITPendingBit( pstUSARTx, USART_IT_RXNE );  /* auto cleared by read DR register */
        //printf( "r%x", ucData );

        *puiRxTOCnt = (TIMER_TO_CNTS);

        /* Store when not full, otherwise discard */
        if((pstRecvInfo->usLen + pstRecvInfo->usNonFinLen) < (SERL_RECV_BUFF_LEN))
        {
            pstRecvInfo->s_ucBuff[pstRecvInfo->usStoreIndex] = ucData;
            pstRecvInfo->usStoreIndex++;
            pstRecvInfo->usNonFinLen++;  // usLen

            if( pstRecvInfo->usStoreIndex >= (SERL_RECV_BUFF_LEN))
            {
                pstRecvInfo->usStoreIndex = 0U;
            }
        }
    }

    OSIntExit();
    OS_EXIT_CRITICAL();

    return;
}

/*
***********************************************************************************************************************
* Function Name: GetSerlRecvInfo
*
* Description: USART6 IRQ: Receive interrupt handler
*
* Input Parameter: pstUSARTx
*
* Output Parameter: None
*
* Return: Receive info., NULL - get failed
*
* Note: None
*
* Author Date: wangdong 2018.04.01
***********************************************************************************************************************
*/

static SerlRecvInfo_t* GetSerlRecvInfo( USART_TypeDef *pstUSARTx )
{
    SerlRecvInfo_t *pstRet = NULL;

    if( pstUSARTx != NULL )
    {
        if( USART1 == pstUSARTx )
        {
            pstRet = &s_stSerlRecvInfo[0];
        }
        else if( USART2 == pstUSARTx )
        {
            pstRet = &s_stSerlRecvInfo[1];
        }
        else if( USART3 == pstUSARTx )
        {
            pstRet = &s_stSerlRecvInfo[2];
        }
        else if( UART4 == pstUSARTx )
        {
            pstRet = &s_stSerlRecvInfo[3];
        }
        else if( UART5 == pstUSARTx )
        {
            pstRet = &s_stSerlRecvInfo[4];
        }
        else if( USART6 == pstUSARTx )
        {
            pstRet = &s_stSerlRecvInfo[5];
        }
        else
        {
            pstRet = NULL;
        }
    }

    return pstRet;
}
/*
***********************************************************************************************************************
                                 File End
***********************************************************************************************************************
*/
