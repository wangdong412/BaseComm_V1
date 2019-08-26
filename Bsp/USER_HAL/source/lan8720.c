/* Includes ------------------------------------------------------------------*/
#include "lan8720.h"
#include "malloc.h"
#include "lwip_comm.h"
#include "hw_arch.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define LAN8720_RST_PIN			GPIO_Pin_0
#define LAN8720_RST_PORT		GPIOA
#define LAN8720_RST_START()		GPIO_ResetBits(LAN8720_RST_PORT, LAN8720_RST_PIN)
#define LAN8720_RST_END()       GPIO_SetBits(LAN8720_RST_PORT, LAN8720_RST_PIN)

#define CHECKSUM_BY_HARDWARE    /* stm32F4 enable hardware check and compute IP UDP ICMP check sum */
#define LAN8720_PHY_ADDRESS  	0x00  /* LAN8720 PHY Address */



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Delay_ns(u32 n);


/**
  * @brief  
  * @param  
  * @retval 
  */
static void Delay_ns(u32 n)
{
	u32 i = 0, j = 0;
	
	for(i=0; i<35; i++)
		for(j=0; j<n; j++);
}

/**
  * @brief  
  * @param  
  * @retval 
  */
u8 LAN8720_Init(void)
{
	u8 rval=0;
	
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOG , ENABLE); /* RMII */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);   

	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); /* MAC and PHY with RMII */

	/*  RMII interface :
	ETH_MDIO -------------------------> PA2
	ETH_MDC --------------------------> PC1
	ETH_RMII_REF_CLK------------------> PA1
	ETH_RMII_CRS_DV ------------------> PA7
	ETH_RMII_RXD0 --------------------> PC4
	ETH_RMII_RXD1 --------------------> PC5
	ETH_RMII_TX_EN -------------------> PG11
	ETH_RMII_TXD0 --------------------> PG13
	ETH_RMII_TXD1 --------------------> PG14
	ETH_RESET-------------------------> PA0*/

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);  
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH); 
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	LAN8720_RST_START();
	Delay_ns(50000);
	LAN8720_RST_END();
	Ethernet_NVICConfiguration();
	rval = ETH_MACDMA_Config(LAN8720_PHY_ADDRESS);

	return !rval;					/* ETH的规则为:0-失败; 1-成功;所以要取反一下 */
}



/**
  * @brief  
  * @param  
  * @retval 
  */
u8 LAN8720_Get_Speed(void)
{
	u8 speed = 0;
	speed = ( (ETH_ReadPHYRegister(0x00,31) & 0x1C) >> 2 ); 
	return speed;
}





	
/************************ (C) COPYRIGHT LDY *****END OF FILE****/
