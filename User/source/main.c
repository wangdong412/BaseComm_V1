/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "includes.h"
#include "bsp.h"
#include "app.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static OS_STK START_TASK_STK[START_STK_SIZE];  /* Task Stack */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	BSP_Init();
	OSInit();
	OSTaskCreate(Start_Task, (void*)0, (OS_STK*)&START_TASK_STK[START_STK_SIZE-1], START_TASK_PRIO);
	OSStart();
}

/************************ (C) COPYRIGHT LDY *****END OF FILE****/
