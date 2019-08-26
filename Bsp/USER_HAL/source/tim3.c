/* Includes ------------------------------------------------------------------*/
#include "tim3.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u32 Tim3_Nus = 0;

/**
  * @brief  
  * @param  
  * @retval 
  */
void TIM3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period = arr;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_Pulse = 50000;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
	
	TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,DISABLE); //使能定时器3
	
	NVIC_PriorityGroupConfig(NVIC_PRIGROUP);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=NVIC_TIM3_PRE; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=NVIC_TIM3_SUB; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}		

/**
  * @brief  
  * @param  
  * @retval 
  */
void TIM3_IrqHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET) //溢出中断
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1); 
		Tim3_Nus += 50000;
		if(Tim3_Nus >= (1000000*60*30)) // 30 分钟
		{
			Tim3_Nus -= (1000000*60*30);
		}
	}
}

/**
  * @brief  
  * @param  
  * @retval 
  */
void TIM3_NusReset(void)
{
	TIM_SetCounter(TIM3, 0);
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1); 
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE); 
	TIM_Cmd(TIM3, ENABLE);
	Tim3_Nus = 0;
}



	
/************************ (C) COPYRIGHT LDY *****END OF FILE****/
