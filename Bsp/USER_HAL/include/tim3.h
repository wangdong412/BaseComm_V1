/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM3_H__
#define __TIM3_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern u32 Tim3_Nus;

extern void TIM3_Init(u16 arr,u16 psc);
extern void TIM3_IrqHandler(void);
extern void TIM3_NusReset(void);

#endif /* __TIM3_H__ */

/************************ (C) COPYRIGHT LDY *****END OF FILE****/
