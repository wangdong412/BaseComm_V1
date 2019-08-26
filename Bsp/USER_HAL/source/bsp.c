/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "includes.h"
#include "lan8720.h"
#include "lwip_comm.h"
#include "tim3.h"
#include "delay.h"
#include "malloc.h"
#include "rtc.h"
#include "queue.h"
#include <stdio.h>
#include "mv88e6061.h"
#include "Usart.h"
#include "Timer.h"
#include "Flash_W25Q.h"


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
void Systick_Init(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    u32 cpu_clk_freq;
    u32 cnts;

    RCC_GetClocksFreq(&rcc_clocks);
    cpu_clk_freq = (u32)rcc_clocks.HCLK_Frequency;
    cnts = cpu_clk_freq / OS_TICKS_PER_SEC;
    SysTick_Config(cnts);
}


/**
  * @brief  
  * @param  
  * @retval 
  */
void BSP_Init(void)
{
    /* Configures the NVIC priority grouping: 2 */
    NVIC_PriorityGroupConfig( NVIC_PRIGROUP );

    /* USART/UART */
    UsartInit( USART1 );
    UsartInit( USART2 );
    UsartInit( USART3 );
    UsartInit( UART4 );
    UsartInit( UART5 );
    UsartInit( USART6 );

    /* Timer: 10ms - interrupt cycle */
    /* Tout = ((arr+1)*(psc+1)) / Tclk(MHZ)    us */
    /* TIM9 - TIM11 Tclk: APB2*2=168MHZ */
    TIM9_Init( 100-1, 16800-1 );
    TIM10_Init( 100-1, 16800-1 );
    TIM11_Init( 100-1, 16800-1 );

    /* TIM12 - TIM14 Tclk: APB1*2=84MHZ */
    TIM12_Init( 100-1, 8400-1 );
    TIM13_Init( 100-1, 8400-1 );
    TIM14_Init( 100-1, 8400-1 );

    /* Flash - kanwei */
    bsp_W25QXX_Init();

    my_mem_init(SRAMIN);
    //my_mem_init(SRAMEX);  //初始化外部内存池
    my_mem_init(SRAMCCM);   //初始化CCM内存池

    printf("---BaseComm Debug Start---\r\n");
}		


/************************ (C) COPYRIGHT LDY *****END OF FILE****/
