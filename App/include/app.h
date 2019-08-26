/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_H__
#define __APP_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "ucos_ii.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern OS_EVENT *Sem_Data1;
extern OS_EVENT *Q_Data1;  
/* Exported functions ------------------------------------------------------- */
extern void Start_Task(void *pdata);


#endif /* __APP_H__ */

/************************ (C) COPYRIGHT LDY *****END OF FILE****/
