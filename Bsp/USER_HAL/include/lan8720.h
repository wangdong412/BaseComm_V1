/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LAN8720_H__
#define __LAN8720_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
extern u8 LAN8720_Init(void);
extern u8 LAN8720_Get_Speed(void);
extern FrameTypeDef ETH_Rx_Packet(void);
extern u8 ETH_Tx_Packet(u16 FrameLength);
extern u32 ETH_GetCurrentTxBuffer(void);
extern u32 ETH_Mem_Malloc(void);
extern void ETH_Mem_Free(void);
extern void ETH_IrqHandler(void);

#endif /* __LAN8720_H__ */

/************************ (C) COPYRIGHT LDY *****END OF FILE****/
