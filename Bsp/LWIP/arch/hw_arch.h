#ifndef __HW_ARCH_H__
#define __HW_ARCH_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"



extern ETH_DMADESCTypeDef  *DMATxDescToSet;			// DMA Tx descriptor point
extern ETH_DMADESCTypeDef  *DMARxDescToGet; 		// DMA Rx descriptor point 
extern ETH_DMADESCTypeDef  *DMARxDscrTab;        /* ��̫��DMA�������������ݽṹ��ָ�� */
extern ETH_DMADESCTypeDef  *DMATxDscrTab;        /* ��̫��DMA�������������ݽṹ��ָ�� */
extern u8 *Rx_Buff;                       /* ��̫���ײ���������buffersָ��  */
extern u8 *Tx_Buff;                       /* ��̫���ײ���������buffersָ�� */



extern u8 ETH_MACDMA_Config(uint16_t PHYAddress);
extern void Ethernet_NVICConfiguration(void);



#endif



