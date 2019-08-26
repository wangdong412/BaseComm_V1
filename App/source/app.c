/* Includes ------------------------------------------------------------------*/
#include "app.h"
#include "bsp.h"
#include "rtc.h"
#include "lwip_comm.h"
#include "includes.h"
#include "queue.h"
#include "TaskCheck.h"

/* Task */
#include "InterComm.h"
#include "BaseConfig.h"
#include "SerlComm.h"
#include "delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static OS_STK s_uiTaskStkInterComm[INTER_COMM_TASK_STK_SIZE];
static OS_STK s_uiTaskStkSerlComm[SERL_COMM_TASK_STK_SIZE];
static OS_STK s_uiTaskStkBaseCfg[BASE_CFG_TASK_STK_SIZE];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  
  * @param  
  * @retval 
  */
void Sys_Init(void)
{	
    uint8_t ucErr = Lwip_CommInit();
	switch( ucErr )
	{
		case ETH_MEM_MALLOC_ERROR:
			printf("ETH 内存分配错误\r\n");
			break;
		case LWIP_MEM_MALLOC_ERROR:
			printf("LWIP 内存分配错误\r\n");
			break;
		case LAN8720_MAC_ERROR:
			printf("LAN8720 MAC和PHY初始化错误，可能网线没插\r\n");
			break;
		case NETIF_ERROR:
			printf("网卡初始化错误\r\n");
			break;
		case LWIP_INIT_OK:
			printf("网卡初始化OK\r\n");
			break;
	}
}

/**
  * @brief  
  * @param  
  * @retval 
  */
void Task_Init(void)
{
	OS_CPU_SR cpu_sr;
	
	/* Initialize */
	InterCommInit();
	BaseCfgInit();
	SerlCommInit();
	
	OS_ENTER_CRITICAL();

 	/* Task Create */
 	OSTaskCreateExt( InterCommTask, (void*)0, (OS_STK*)&s_uiTaskStkInterComm[(INTER_COMM_TASK_STK_SIZE) - 1], \
 	                 (INTER_COMM_TASK_PRIO), (INTER_COMM_TASK_PRIO), (OS_STK*)&s_uiTaskStkInterComm[0], (INTER_COMM_TASK_STK_SIZE), (void *)0, \
 	                 OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR );

    OSTaskCreateExt( SerlCommTask, (void*)0, (OS_STK*)&s_uiTaskStkSerlComm[(SERL_COMM_TASK_STK_SIZE) - 1], \
                     (SERL_COMM_TASK_PRIO), (SERL_COMM_TASK_PRIO), (OS_STK*)&s_uiTaskStkSerlComm[0], (SERL_COMM_TASK_STK_SIZE), (void *)0, \
                     OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR );

    OSTaskCreateExt( ConfigTask, (void*)0, (OS_STK*)&s_uiTaskStkBaseCfg[(BASE_CFG_TASK_STK_SIZE) - 1], \
                     (BASE_CFG_TASK_PRIO), (BASE_CFG_TASK_PRIO), (OS_STK*)&s_uiTaskStkBaseCfg[0], (BASE_CFG_TASK_STK_SIZE), (void *)0, \
                     OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR );

	OS_EXIT_CRITICAL();

	/* System tick
	 * 168: SYSCLK is 168MHZ */
	Delay_Init( 168 );
}


/**
  * @brief  
  * @param  
  * @retval 
  */
void Start_Task(void *pdata)
{
	Sys_Init();
	
	Task_Init();

	OSTaskSuspend(OS_PRIO_SELF);
	
	while(1)
	{
		OSTaskSuspend(OS_PRIO_SELF);
	}
}		


/************************ (C) COPYRIGHT LDY *****END OF FILE****/
