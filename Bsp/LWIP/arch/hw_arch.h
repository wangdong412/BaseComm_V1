#ifndef __HW_ARCH_H__
#define __HW_ARCH_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"



extern ETH_DMADESCTypeDef  *DMATxDescToSet;			// DMA Tx descriptor point
extern ETH_DMADESCTypeDef  *DMARxDescToGet; 		// DMA Rx descriptor point 
extern ETH_DMADESCTypeDef  *DMARxDscrTab;        /* 以太网DMA接收描述符数据结构体指针 */
extern ETH_DMADESCTypeDef  *DMATxDscrTab;        /* 以太网DMA发送描述符数据结构体指针 */
extern u8 *Rx_Buff;                       /* 以太网底层驱动接收buffers指针  */
extern u8 *Tx_Buff;                       /* 以太网底层驱动发送buffers指针 */



extern u8 ETH_MACDMA_Config(uint16_t PHYAddress);
extern void Ethernet_NVICConfiguration(void);



#endif



