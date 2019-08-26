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
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period = arr;   //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_Pulse = 50000;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	
	TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,DISABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_PriorityGroupConfig(NVIC_PRIGROUP);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=NVIC_TIM3_PRE; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=NVIC_TIM3_SUB; //�����ȼ�3
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
	if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET) //����ж�
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1); 
		Tim3_Nus += 50000;
		if(Tim3_Nus >= (1000000*60*30)) // 30 ����
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
