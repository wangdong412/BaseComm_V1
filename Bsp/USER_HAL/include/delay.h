/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DELAY_H__
#define __DELAY_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void Delay_Init(u8 Sysclk);
extern void Delay_us(u32 n);
extern void Delay_ms(u16 nms);
extern u32 Delay_Getus(void);

#endif /* __DELAY_H__ */

/************************ (C) COPYRIGHT LDY *****END OF FILE****/
