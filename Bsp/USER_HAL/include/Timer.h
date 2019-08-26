/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_H__
#define __TIMER_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "Usart.h"

#define TIM9_SERL_INDEX   (0)
#define TIM10_SERL_INDEX  (1)
#define TIM11_SERL_INDEX  (2)
#define TIM12_SERL_INDEX  (3)
#define TIM13_SERL_INDEX  (4)
#define TIM14_SERL_INDEX  (5)


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void TIM9_Init(u16 arr,u16 psc);
extern void TIM10_Init(u16 arr,u16 psc);
extern void TIM11_Init(u16 arr,u16 psc);
extern void TIM12_Init(u16 arr,u16 psc);
extern void TIM13_Init(u16 arr,u16 psc);
extern void TIM14_Init(u16 arr,u16 psc);


extern void TIM1_BRK_TIM9_IRQHandler( void );
extern void TIM1_UP_TIM10_IRQHandler( void );
extern void TIM1_TRG_COM_TIM11_IRQHandler( void );
extern void TIM8_BRK_TIM12_IRQHandler( void );
extern void TIM8_UP_TIM13_IRQHandler( void );
extern void TIM8_TRG_COM_TIM14_IRQHandler( void );

#endif /* __TIMER_H__ */

/************************ (C) COPYRIGHT LDY *****END OF FILE****/
