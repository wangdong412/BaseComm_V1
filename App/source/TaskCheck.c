/* Includes ------------------------------------------------------------------*/
#include "TaskCheck.h"
#include "app.h"
#include "includes.h"

/**
  * @brief  
  * @param  
  * @retval 
  */
void TaskCheck_Task(void *pdata)
{
#if 0
	OS_STK_DATA  StackBytes;
	
	while(1)
	{
		OSTaskStkChk(SOCKET_UDP_PRIO, &StackBytes);
		printf("Socket STK free:%d usd:%d\r\n", StackBytes.OSFree, StackBytes.OSUsed);
		OSTimeDlyHMSM(0,0,0,500);
	}
#endif
}


///************************ (C) COPYRIGHT LDY *****END OF FILE****/
