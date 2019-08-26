/* Includes ------------------------------------------------------------------*/
#include "queue.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static Queue_t Queue_Data1;  /* 定义一个输入队列 */
Data_t Queue_Data1Buf[QUEUE_SIZE];   /* 队列的缓冲区 */
u8 Queue_Index = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
  * @brief  
  * @param  
  * @retval 
  */
static BOOL QUEUE_Init(Queue_t *pQueue, Data_t *pBuf, unsigned int size)
{
    Queue_t *ptQ = pQueue;
    
    if((NULL == ptQ) || (NULL == pBuf) || (0 == size))
    {
        return FALSE;
    }
    
    ptQ->ptBuffer = pBuf;                                               
    ptQ->tSize = size;                                                     
    ptQ->tHead = 0;                                                         
    ptQ->tTail = 0;                                                         
    ptQ->tCounter = 0;                                                      

    return TRUE;
}

/**
  * @brief  
  * @param  
  * @retval 
  */
static BOOL QUEUE_In(Queue_t *ptQueue, Data_t *pdata)
{
    Queue_t *ptQ = ptQueue;
    
    __disable_irq();
    
    if(NULL == ptQ)
    {
        return FALSE;
    }
    
    if((ptQ->tHead == ptQ->tTail) && (0 != ptQ->tCounter))
    {
        return FALSE;
    }	
	__enable_irq();
	
	memcpy(&(ptQ->ptBuffer[ptQ->tTail]), pdata, DATA_HEAD_LEN + pdata->len);
	
	__disable_irq();
    ptQ->tTail++;
    if(ptQ->tTail >= ptQ->tSize)
    {
        ptQ->tTail = 0;
    }
    ptQ->tCounter++; 
	
    __enable_irq();
	
    return TRUE;
}
  
/**
  * @brief  
  * @param  
  * @retval 
  */
static BOOL QUEUE_Out(Queue_t *ptQueue, Data_t *pdata)
{
    Queue_t *ptQ = ptQueue;
    
    __disable_irq();
    
    if(NULL == ptQ)
    {
        return FALSE;
    }
    if((ptQ->tHead == ptQ->tTail) && (!ptQ->tCounter))
    {
        return FALSE;
    }
	__enable_irq();
	
    if(NULL != pdata)
    {
		memcpy(pdata, &(ptQ->ptBuffer[ptQ->tHead]), DATA_HEAD_LEN + ptQ->ptBuffer[ptQ->tHead].len);  
    }
	__disable_irq();
    ptQ->tHead++;
    if (ptQ->tHead >= ptQ->tSize) 
    {                                 
        ptQ->tHead = 0;                                             
    }                                                               
    ptQ->tCounter--;                                                
    __enable_irq();
	
    return TRUE;
}

BOOL Queue_Init(void)
{
	return QUEUE_Init(&Queue_Data1, Queue_Data1Buf, QUEUE_SIZE);
}

BOOL Queue_In(Data_t *data)
{
	return QUEUE_In(&Queue_Data1, data);
}

BOOL Queue_Out(Data_t *data)
{
	return QUEUE_Out(&Queue_Data1, data);
}



/************************ (C) COPYRIGHT LDY *****END OF FILE****/
