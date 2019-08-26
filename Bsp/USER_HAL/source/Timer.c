/* Includes ------------------------------------------------------------------*/
#include "Timer.h"
#include "queue.h"
#include "delay.h"
#include "app.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  * @param  
  * @retval 
  */
void TIM9_Init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	/* Enable clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM9, ENABLE );

	/* Initialize */
	TIM_TimeBaseInitStructure.TIM_Period = arr;     //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseInit( TIM9, &TIM_TimeBaseInitStructure ); //初始化定时器

	/* Enable update interrupt */
	TIM_ITConfig( TIM9, TIM_IT_Update, ENABLE);

	/* Enable Timer */
	TIM_Cmd( TIM9, ENABLE );
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIM9_PRE;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIM9_SUB;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM10_Init(u16 arr,u16 psc)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    /* Enable clock */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM10, ENABLE );

    /* Initialize */
    TIM_TimeBaseInitStructure.TIM_Period = arr;     //自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;  //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit( TIM10, &TIM_TimeBaseInitStructure ); //初始化定时器

    /* Enable update interrupt */
    TIM_ITConfig( TIM10, TIM_IT_Update, ENABLE);

    /* Enable Timer */
    TIM_Cmd( TIM10, ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIM10_PRE;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIM10_SUB;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM11_Init(u16 arr,u16 psc)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    /* Enable clock */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM11, ENABLE );

    /* Initialize */
    TIM_TimeBaseInitStructure.TIM_Period = arr;     //自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;  //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit( TIM11, &TIM_TimeBaseInitStructure ); //初始化定时器

    /* Enable update interrupt */
    TIM_ITConfig( TIM11, TIM_IT_Update, ENABLE);

    /* Enable Timer */
    TIM_Cmd( TIM11, ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_TRG_COM_TIM11_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIM11_PRE;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIM11_SUB;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM12_Init(u16 arr,u16 psc)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    /* Enable clock */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM12, ENABLE );

    /* Initialize */
    TIM_TimeBaseInitStructure.TIM_Period = arr;                     // 自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                  // 定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 分频因子
    TIM_TimeBaseInit( TIM12, &TIM_TimeBaseInitStructure );          // 初始化定时器

    /* Enable update interrupt */
    TIM_ITConfig( TIM12, TIM_IT_Update, ENABLE);

    /* Enable Timer */
    TIM_Cmd( TIM12, ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIM12_PRE;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIM12_SUB;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM13_Init(u16 arr,u16 psc)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    /* Enable clock */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM13, ENABLE );

    /* Initialize */
    TIM_TimeBaseInitStructure.TIM_Period = arr;     //自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;  //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit( TIM13, &TIM_TimeBaseInitStructure ); //初始化定时器

    /* Enable update interrupt */
    TIM_ITConfig( TIM13, TIM_IT_Update, ENABLE);

    /* Enable Timer */
    TIM_Cmd( TIM13, ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIM13_PRE;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIM13_SUB;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM14_Init(u16 arr,u16 psc)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    /* Enable clock */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM14, ENABLE );

    /* Initialize */
    TIM_TimeBaseInitStructure.TIM_Period = arr;     //自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;  //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit( TIM14, &TIM_TimeBaseInitStructure ); //初始化定时器

    /* Enable update interrupt */
    TIM_ITConfig( TIM14, TIM_IT_Update, ENABLE);

    /* Enable Timer */
    TIM_Cmd( TIM14, ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIM14_PRE;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIM14_SUB;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM1_BRK_TIM9_IRQHandler( void )
{
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();
    OSIntEnter();

    if(((TIM9->SR & TIM_IT_Update) != 0 ) && ((TIM9->DIER & TIM_IT_Update) != 0 ))  // TIM_GetITStatus, TIM_IT_Update
    {
        if( g_uiRxTOCnt[TIM9_SERL_INDEX] > 0 )
        {
            g_uiRxTOCnt[TIM9_SERL_INDEX]--;

            if( 0 == g_uiRxTOCnt[TIM9_SERL_INDEX])
            {
                /* Receive finished: update receive length */
                UsartUpdateRxLen( USART1 );
            }
        }
    }

    TIM9->SR = (uint16_t)~TIM_IT_Update;  // TIM_ClearITPendingBit, TIM_IT_Update

    OSIntExit();
    OS_EXIT_CRITICAL();
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM1_UP_TIM10_IRQHandler( void )
{
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();
    OSIntEnter();

    if(((TIM10->SR & TIM_IT_Update) != 0 ) && ((TIM10->DIER & TIM_IT_Update) != 0 ))  // TIM_GetITStatus, TIM_IT_Update
    {
        if( g_uiRxTOCnt[TIM10_SERL_INDEX] > 0 )
        {
            g_uiRxTOCnt[TIM10_SERL_INDEX]--;

            if( 0 == g_uiRxTOCnt[TIM10_SERL_INDEX])
            {
                /* Receive finished: update receive length */
                UsartUpdateRxLen( USART2 );
            }
        }
    }

    TIM10->SR = (uint16_t)~TIM_IT_Update;  // TIM_ClearITPendingBit, TIM_IT_Update

    OSIntExit();
    OS_EXIT_CRITICAL();
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM1_TRG_COM_TIM11_IRQHandler( void )
{
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();
    OSIntEnter();

    if(((TIM11->SR & TIM_IT_Update) != 0 ) && ((TIM11->DIER & TIM_IT_Update) != 0 ))  // TIM_GetITStatus, TIM_IT_Update
    {
        if( g_uiRxTOCnt[TIM11_SERL_INDEX] > 0 )
        {
            g_uiRxTOCnt[TIM11_SERL_INDEX]--;

            if( 0 == g_uiRxTOCnt[TIM11_SERL_INDEX])
            {
                /* Receive finished: update receive length */
                UsartUpdateRxLen( USART3 );
            }
        }
    }

    TIM11->SR = (uint16_t)~TIM_IT_Update;  // TIM_ClearITPendingBit, TIM_IT_Update

    OSIntExit();
    OS_EXIT_CRITICAL();
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM8_BRK_TIM12_IRQHandler( void )
{
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();
    OSIntEnter();

    if(((TIM12->SR & TIM_IT_Update) != 0 ) && ((TIM12->DIER & TIM_IT_Update) != 0 ))  // TIM_GetITStatus, TIM_IT_Update
    {
        if( g_uiRxTOCnt[TIM12_SERL_INDEX] > 0 )
        {
            g_uiRxTOCnt[TIM12_SERL_INDEX]--;

            if( 0 == g_uiRxTOCnt[TIM12_SERL_INDEX])
            {
                /* Receive finished: update receive length */
                UsartUpdateRxLen( UART4 );
            }
        }
    }

    TIM12->SR = (uint16_t)~TIM_IT_Update;  // TIM_ClearITPendingBit, TIM_IT_Update

    OSIntExit();
    OS_EXIT_CRITICAL();
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM8_UP_TIM13_IRQHandler( void )
{
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();
    OSIntEnter();

    if(((TIM13->SR & TIM_IT_Update) != 0 ) && ((TIM13->DIER & TIM_IT_Update) != 0 ))  // TIM_GetITStatus, TIM_IT_Update
    {
        if( g_uiRxTOCnt[TIM13_SERL_INDEX] > 0 )
        {
            g_uiRxTOCnt[TIM13_SERL_INDEX]--;

            if( 0 == g_uiRxTOCnt[TIM13_SERL_INDEX])
            {
                /* Receive finished: update receive length */
                UsartUpdateRxLen( UART5 );
            }
        }
    }

    TIM13->SR = (uint16_t)~TIM_IT_Update;  // TIM_ClearITPendingBit, TIM_IT_Update

    OSIntExit();
    OS_EXIT_CRITICAL();
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM8_TRG_COM_TIM14_IRQHandler( void )
{
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();
    OSIntEnter();

    if(((TIM14->SR & TIM_IT_Update) != 0 ) && ((TIM14->DIER & TIM_IT_Update) != 0 ))  // TIM_GetITStatus, TIM_IT_Update
    {
        if( g_uiRxTOCnt[TIM14_SERL_INDEX] > 0 )
        {
            g_uiRxTOCnt[TIM14_SERL_INDEX]--;

            if( 0 == g_uiRxTOCnt[TIM14_SERL_INDEX])
            {
                /* Receive finished: update receive length */
                UsartUpdateRxLen( USART6 );
            }
        }
    }

    TIM14->SR = (uint16_t)~TIM_IT_Update;  // TIM_ClearITPendingBit, TIM_IT_Update

    OSIntExit();
    OS_EXIT_CRITICAL();
}

/************************ (C) COPYRIGHT LDY *****END OF FILE****/
