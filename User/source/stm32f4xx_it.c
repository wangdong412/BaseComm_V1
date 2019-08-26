/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "tim3.h"
#include "ucos_ii.h"
#include "lan8720.h"
#include "includes.h"
#include "lwip_comm.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

__asm void ReturnToBack()
{
    BX lr  //BX ������ת��ָ��
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  printf("Hard Falut!!!\r\n");
  ReturnToBack();
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();
    OSIntEnter();

    OSTimeTick();  /* Call uC/OS-II's OSTimeTick()              */

    OSIntExit();   /* Tell uC/OS-II that we are leaving the ISR */
    OS_EXIT_CRITICAL();
}

/**
  * @brief  This function handles SPI1 Handler.
  * @param  None
  * @retval None
  */
void SPI1_IRQHandler(void)
{	
	OSIntEnter();
	/* SPI in Receiver mode */
	if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET)
	{

	}
	/* SPI in Transmitter mode */
	if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) == SET)
	{
		/* Disable the Tx buffer empty interrupt */
		SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
	}
	OSIntExit();
}

/**
  * @brief  This function handles SPI2 Handler.
  * @param  None
  * @retval None
  */
void SPI2_IRQHandler(void)
{
	OSIntEnter();
	/* SPI in Receiver mode */
	if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) == SET)
	{

	}
	/* SPI in Transmitter mode */
	if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET)
	{
		/* Disable the Tx buffer empty interrupt */
		SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
	}
	OSIntExit();
}

/**
  * @brief  This function handles SPI3 Handler.
  * @param  None
  * @retval None
  */
void SPI3_IRQHandler(void)
{
	OSIntEnter();	
	/* SPI in Receiver mode */
	if (SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE) == SET)
	{

	}
	/* SPI in Transmitter mode */
	if (SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_TXE) == SET)
	{
		/* Disable the Tx buffer empty interrupt */
		SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, DISABLE);
	}
	OSIntExit();
}

/**
  * @brief  This function handles TIM3 Handler.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	OSIntEnter();
	TIM3_IrqHandler();
	OSIntExit();
}

/**
  * @brief  This function handles ETH Handler.
  * @param  None
  * @retval None
  */
void ETH_IRQHandler(void)
{
	OSIntEnter();
	ETH_IrqHandler();
	OSIntExit();
}

//RTC�����жϷ�����
void RTC_Alarm_IRQHandler(void)
{    
	OSIntEnter();
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A�ж�?
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);//����жϱ�־
	}   
	EXTI_ClearITPendingBit(EXTI_Line17);	//����ж���17���жϱ�־ 											 
	OSIntExit();
}

//RTC WAKE UP�жϷ�����
void RTC_WKUP_IRQHandler(void)
{    
	OSIntEnter();
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)  //WK_UP�ж�?
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);	      //����жϱ�־
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);      //����ж���22���жϱ�־ 	
	OSIntExit();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
