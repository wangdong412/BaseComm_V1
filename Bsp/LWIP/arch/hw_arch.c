#include "hw_arch.h"
#include "malloc.h"
#include "lwip_comm.h"


/* Private variables ---------------------------------------------------------*/
ETH_DMADESCTypeDef  *DMARxDscrTab; /* 以太网DMA接收描述符数据结构体指针 */
ETH_DMADESCTypeDef  *DMATxDscrTab; /* 以太网DMA发送描述符数据结构体指针 */
u8 *Rx_Buff;                       /* 以太网底层驱动接收buffers指针  */
u8 *Tx_Buff;                       /* 以太网底层驱动发送buffers指针 */



/**
  * @brief  
  * @param  
  * @retval 
  */
void ETH_IrqHandler(void)
{
	while(ETH_GetRxPktSize(DMARxDescToGet) != 0) /* Check Whether exist data */
	{
		lwip_pkt_handle();	
	}
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}

/**
  * @brief  
  * @param  
  * @retval 
  */
FrameTypeDef ETH_Rx_Packet(void)
{ 
	u32 framelength = 0;
	FrameTypeDef frame = {0,0};   
	
	/* check whether the current descriptor belongs to ETHERNET DMA */
	if((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (u32)RESET)
	{	
		frame.length = ETH_ERROR; 
		if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  
		{ 
			ETH->DMASR = ETH_DMASR_RBUS;  
			ETH->DMARPDR=0;
		}
		return frame;
	}  
	if( ( (DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (u32)RESET)&& 
	    ( (DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (u32)RESET)&&  
	    ( (DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (u32)RESET) )  
	{       
		framelength = ((DMARxDescToGet->Status&ETH_DMARxDesc_FL)>>ETH_DMARxDesc_FrameLengthShift)-4;
 		frame.buffer = DMARxDescToGet->Buffer1Addr;
	}
	else
	{
		framelength = ETH_ERROR;  	
	}

	frame.length = framelength; 
	frame.descriptor = DMARxDescToGet;  

	DMARxDescToGet = (ETH_DMADESCTypeDef*)(DMARxDescToGet->Buffer2NextDescAddr);   
	
	return frame;  
}

/**
  * @brief  
  * @param  
  * @retval 
  */
u8 ETH_Tx_Packet(u16 FrameLength)
{   
	/* check whether the current descriptor belongs to ETHERNET DMA */
	if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
	{
		return ETH_ERROR; 
	}
 	DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1); 
	DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS; 
  	DMATxDescToSet->Status |= ETH_DMATxDesc_OWN; 
	if((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET) 
	{ 
		ETH->DMASR = ETH_DMASR_TBUS; 
		ETH->DMATPDR = 0; 
	} 

	DMATxDescToSet = (ETH_DMADESCTypeDef*)(DMATxDescToSet->Buffer2NextDescAddr);
    
	return ETH_SUCCESS;   
}


/**
  * @brief  
  * @param  
  * @retval 
  */
u32 ETH_GetCurrentTxBuffer(void)
{
	return DMATxDescToSet->Buffer1Addr;
}

/**
  * @brief  
  * @param  
  * @retval 
  */
u32 ETH_Mem_Malloc(void)
{
	DMARxDscrTab = mymalloc(SRAMIN, ETH_RXBUFNB*sizeof(ETH_DMADESCTypeDef));//申请内存
	DMATxDscrTab = mymalloc(SRAMIN, ETH_TXBUFNB*sizeof(ETH_DMADESCTypeDef));//申请内存  
	Rx_Buff = mymalloc(SRAMIN, ETH_RX_BUF_SIZE*ETH_RXBUFNB);	//申请内存
	Tx_Buff = mymalloc(SRAMIN, ETH_TX_BUF_SIZE*ETH_TXBUFNB);	//申请内存
	if(!DMARxDscrTab || !DMATxDscrTab || !Rx_Buff ||!Tx_Buff)
	{
		ETH_Mem_Free();
		return 1;	//申请失败
	}	
	return 0;		//申请成功
}

/**
  * @brief  
  * @param  
  * @retval 
  */ 
void ETH_Mem_Free(void)
{
	myfree(SRAMIN, DMARxDscrTab);//释放内存
	myfree(SRAMIN, DMATxDscrTab);//释放内存
	myfree(SRAMIN, Rx_Buff);		//释放内存
	myfree(SRAMIN, Tx_Buff);		//释放内存  
}


/**
  * @brief  
  * @param  
  * @retval 
  */
u8 ETH_MACDMA_Config(uint16_t PHYAddress)
{
	u8 rval;
	ETH_InitTypeDef ETH_InitStructure; 
	
	/* Enable Ethernet clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
                        
	ETH_DeInit();  								/* AHB Bus reset to start ethernet  */
	ETH_SoftwareReset();  						/* software restart ethernet  */
	while (ETH_GetSoftwareResetStatus() == SET);/* wait for the software reset finish */
	ETH_StructInit(&ETH_InitStructure); 	 	 

#if 0  /* 自协商不能用,写死就行了 */
	/* Ethernet MAC configuration */
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;   			 
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;				 
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable; 		/* close retry */ 
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; 
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;						 
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable; /* Enable broadcast */
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;			 
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;  
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;	 
#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 		 
#endif
#endif


	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable;   			 
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;				 
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable; 		/* close retry */ 
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; 
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;						 
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable; /* Enable broadcast */
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;			 
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;  
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
	ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
	ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
 	
#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 		 
#endif


	/* receive store and transmit store configuration */
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;  
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;     
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;  

	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;      
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;	 
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;  		 
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;  	 
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;            			 
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;     	   
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;			 
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
	
	rval=ETH_Init(&ETH_InitStructure, PHYAddress);	 
	
	if(rval == ETH_SUCCESS) 
	{
		ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);   
	}
	return rval;
}

/**
  * @brief  
  * @param  
  * @retval 
  */
void Ethernet_NVICConfiguration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PRIGROUP);
	
	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_ETH_PRE;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_ETH_SUB;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}











