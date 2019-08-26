#include "mv88e6061.h"
#include "includes.h"
#include "mv88e6061_interface.h"
#include "mv88e6061_phy.h"
#include "hw_arch.h"

#define MV88E6061_RST_PIN			GPIO_Pin_6
#define MV88E6061_RST_PORT			GPIOA
#define MV88E6061_RST_START()		GPIO_ResetBits(MV88E6061_RST_PORT, MV88E6061_RST_PIN)
#define MV88E6061_RST_END()         GPIO_SetBits(MV88E6061_RST_PORT, MV88E6061_RST_PIN)

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

static void MV88E6061_Reset_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIO时钟 
	
	/* RESET PIN */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void MV88E6061_Reset(void)
{
	MV88E6061_RST_START();
	Delay_ns(50000);
	MV88E6061_RST_END();
}
/*
        ETH_MDIO -------------------------> PA2*
        ETH_MDC --------------------------> PC1*
*/
static void MV88E6061_SMI_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC\
	                      |RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIO时钟 MII接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);   //使能SYSCFG时钟
  

	/* Configure PA2 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	
	/* Configure PC1 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);	
}


/*网络引脚设置 MII接口 
		ETH_PPS_OUT ---------------------->
        ETH_MII_CRS ----------------------> PH2  -- PA0*
        ETH_MII_COL ----------------------> PH3	 -- PA3*
		ETH_MII_RX_ER --------------------> 
        ETH_MII_RXD2 ---------------------> PH6  -- PB0*
        ETH_MII_RXD3 ---------------------> PH7  -- PB1*
        ETH_MII_TX_CLK -------------------> PC3*
        ETH_MII_TXD2 ---------------------> PC2*
        ETH_MII_TXD3 ---------------------> PB8*	
        ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1*
        ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7*
        ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4*
        ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5*
		ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PG11  -- PB11*
        ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13*
        ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14*		
*/
static void MV88E6061_MII_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC\
	                      |RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIO时钟 MII接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);   //使能SYSCFG时钟
  
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII); //MAC和PHY之间使用MII接口

	/* Configure PA1, PA2 and PA7 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH); 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);	
	
	/* Configure PB8 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_ETH);	
	
	/* Configure PC1, PC2, PC3, PC4 and PC5 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);	
	
	/* Configure PB11 */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
  
	/* Configure PG14 and PG13 */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);	

	/* Configure PA0, PA3 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_ETH);

	/* Configure PB0, PB1 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_ETH);	
}


u16 MV88E6061_GetState(void)
{
	u16 state = 0;
	
	state = MV88E6061_ReadReg(MV88E6061_PHY_ADDRESS, PHY_SR);
	return state;
}

u16 speed = 0;

/**
  * @brief  
  * @param  
  * @retval 
  */
u8 MV88E6061_Init(void)
{	
	u8 rval=0;	
	
	MV88E6061_SMI_Init();
	MV88E6061_MII_Init();	
	MV88E6061_Reset_Init();
	MV88E6061_Reset();
	
	Ethernet_NVICConfiguration();
	rval = ETH_MACDMA_Config(MV88E6061_PHY_ADDRESS);
	
	MV88E6061_ModeInit();
	
	MV88E6061_Test();

	return !rval;					/* ETH的规则为:0-失败; 1-成功;所以要取反一下 */
}

void MV88E6061_GetSpeed(void)
{
	speed = ETH_ReadPHYRegister(0, 0);
	speed = ETH_ReadPHYRegister(1, 0);
	speed = ETH_ReadPHYRegister(2, 0);
	speed = ETH_ReadPHYRegister(3, 0);
	speed = ETH_ReadPHYRegister(4, 0);
	MV88E6061IDGet(0);
	MV88E6061IDGet(1);
	MV88E6061IDGet(2);
	MV88E6061IDGet(3);
	MV88E6061IDGet(4);
}

/**
  * @brief  
  * @param  
  * @retval 
  */
void MV88E6061_Test(void)
{
	u16 bcr=0, bsr=0, auton;
		
	bcr = MV88E6061_ReadReg(MV88E6061_PHY_ADDRESS, MV88E6061_PHY_BCR);
	bsr = MV88E6061_ReadReg(MV88E6061_PHY_ADDRESS, MV88E6061_PHY_BSR);
	auton = MV88E6061_ReadReg(MV88E6061_PHY_ADDRESS, MV88E6061_PHY_AUTONEG_ADV);	
	
	
	MV88E6061_GetSpeed();
}

