/*
*********************************************************************************************************
*
*	模块名称 : 外部SRAM驱动
*	文件名称 : bsp_sram.c
*	版    本 : V1.0
*	说    明 : 核心板SRAM驱动,SI62WV51216，容量1MB
*
*	修改记录 :
*		版本号  日期        作者     说明
*		
*
*	Copyright (C), 2016-2020, 北京龙鼎源科技股份有限公司
*
*********************************************************************************************************
*/
 
#include "Flash_W25Q.h"

/*
*********************************************************************************************************
*	函 数 名: bsp_W25QXX_Init
*	功能说明: 对W25Q128进行初始化，初始化SPI FLASH的IO口，
            4Kbytes为一个Sector，16个扇区为1个Block，容量为16M字节,共有128个Block,4096个Sector 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/ 
u16 W25QXX_TYPE=W25Q128;	//默认是W25Q128

void bsp_W25QXX_Init(void)
{ 
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PB12
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

    W25QXX_CS=1;			//SPI FLASH不选中
    SPI2_Init();		   			//初始化SPI
    SPI2_SetSpeed(SPI_BaudRatePrescaler_2);		//设置为42M时钟,高速模式
    W25QXX_TYPE=W25QXX_ReadID();	//读取FLASH ID.
}  
/*
*********************************************************************************************************
*	函 数 名: SPI2_Init(void)
*	功能说明: SPI口初始化，配置成主机模式 		
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/ 
void SPI2_Init(void)
{	 
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//使能SPI2时钟


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PB13~15复用功能输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2); //PB3复用为 SPI2
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2); //PB4复用为 SPI2
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2); //PB5复用为 SPI2

    //这里只针对SPI口初始化
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);//复位SPI1
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//停止复位SPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
    SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI2, ENABLE); //使能SPI外设

    SPI2_ReadWriteByte(0xff);//启动传输
}   
/*
*********************************************************************************************************
*	函 数 名: W25QXX_ReadSR(void)
*	功能说明: 读取W25QXX的状态寄存器
            BIT7  6   5   4   3   2   1   0
            SPR   RV  TB BP2 BP1 BP0 WEL BUSY
            SPR:默认0,状态寄存器保护位,配合WP使用
            TB,BP2,BP1,BP0:FLASH区域写保护设置
            WEL:写使能锁定
            BUSY:忙标记位(1,忙;0,空闲)
            默认:0x00
*	形    参: 无
*	返 回 值: 一个字节，W25QXX寄存器状态
*********************************************************************************************************
*/ 
u8 W25QXX_ReadSR(void)   
{  
	u8 byte=0;   
	W25QXX_CS=0;                            //使能器件   
	SPI2_ReadWriteByte(W25X_ReadStatusReg);    //发送读取状态寄存器命令    
	byte=SPI2_ReadWriteByte(0Xff);             //读取一个字节  
	W25QXX_CS=1;                            //取消片选     
	return byte;   
} 
/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_SR(u8 sr)
*	功能说明: 写W25QXX状态寄存器
            只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
*	形    参: u8 sr，
*	返 回 值: 一个字节，W25QXX寄存器状态
*********************************************************************************************************
*/ 
//void W25QXX_Write_SR(u8 sr)   
//{   
//	W25QXX_CS=0;                            //使能器件   
//	SPI2_ReadWriteByte(W25X_WriteStatusReg);   //发送写取状态寄存器命令    
//	SPI2_ReadWriteByte(sr);               //写入一个字节  
//	W25QXX_CS=1;                            //取消片选     	      
//}   
/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_Enable(void)
*	功能说明: 将WEL置位 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/ 
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(W25X_WriteEnable); //发送写使能  
	W25QXX_CS=1;                            //取消片选     	      
} 
/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_Disable(void) 
*	功能说明: W25QXX写禁止
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(W25X_WriteDisable);     //发送写禁止指令    
	W25QXX_CS=1;                            //取消片选     	      
} 	
/*
*********************************************************************************************************
*	函 数 名: W25QXX_ReadID(void) 
*	功能说明: 读取芯片ID
*	形    参: 无
*	返 回 值: 0XEF13,表示芯片型号为W25Q80 
            0XEF14,表示芯片型号为W25Q16
            0XEF15,表示芯片型号为W25Q32
            0XEF16,表示芯片型号为W25Q64 
            0XEF17,表示芯片型号为W25Q128 
*********************************************************************************************************
*/
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;	  
	W25QXX_CS=0;				    
	SPI2_ReadWriteByte(0x90);//发送读取ID命令	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI2_ReadWriteByte(0xFF);	 
	W25QXX_CS=1;				    
	return Temp;
} 
/*
*********************************************************************************************************
*	函 数 名: bsp_W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
*	功能说明: 在指定地址开始读取指定长度的数据
*	形    参: pBuffer:数据存储区，ReadAddr:开始读取的地址(24bit)，
            NumByteToRead:要读取的字节数(最大65535)
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	W25QXX_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(W25X_ReadData);         //发送读取命令   
    SPI2_ReadWriteByte((u8)((ReadAddr)>>16));  //发送24bit地址    
    SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI2_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	  { 
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);   //循环读数  
    }
	W25QXX_CS=1;  				    	      
} 
/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
*	功能说明: 在指定地址开始写入最大256字节的数据
*	形    参: pBuffer:数据存储区，WriteAddr:开始写入的地址(24bit)，
            NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    W25QXX_Write_Enable();                  //SET WEL 
	W25QXX_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(W25X_PageProgram);      //发送写页命令   
    SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); //发送24bit地址    
    SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI2_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI2_ReadWriteByte(pBuffer[i]);//循环写数  
	W25QXX_CS=1;                            //取消片选 
	W25QXX_Wait_Busy();					   //等待写入结束
} 
/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
*	功能说明: 无检验写SPI FLASH，必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
            具有自动换页功能 ，在指定地址开始写入指定长度的数据,但是要确保地址不越界!，
*	形    参: pBuffer:数据存储区，WriteAddr:开始写入的地址(24bit)，
            NumByteToWrite:要写入的字节数(最大65535)	
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		bsp_W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
}
/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
*	功能说明: 写SPI FLASH，在指定地址开始写入指定长度的数据，该函数带擦除操作!
*	形    参: pBuffer:数据存储区，WriteAddr:开始写入的地址(24bit)，
            NumByteToWrite:要写入的字节数(最大65535)	
*	返 回 值: 无
*********************************************************************************************************
*/
u8 W25QXX_BUFFER[4096];		 
void bsp_W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * W25QXX_BUF;	  
  W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		bsp_W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			W25QXX_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;  //写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 
}
///*
//*********************************************************************************************************
//*	函 数 名: W25QXX_Erase_Chip(void) 
//*	功能说明: 擦除整个芯片，等待时间150ms*16
//*	形    参: 无
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//void W25QXX_Erase_Chip(void)   
//{                                   
//    W25QXX_Write_Enable();                  //SET WEL 
//    W25QXX_Wait_Busy();   
//  	W25QXX_CS=0;                            //使能器件   
//    SPI2_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令  
//	W25QXX_CS=1;                            //取消片选     	      
//	W25QXX_Wait_Busy();   				   //等待芯片擦除结束
//}
/*
*********************************************************************************************************
*	函 数 名: W25QXX_Erase_Sector(u32 Dst_Addr)
*	功能说明: 擦除一个扇区
*	形    参: Dst_Addr:扇区地址 根据实际容量设置，擦除一个山区的最少时间:150ms
*	返 回 值: 无
*********************************************************************************************************
*/

void W25QXX_Erase_Sector(u32 Dst_Addr)   
{  
	//监视falsh擦除情况,测试用   
// 	printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
    W25QXX_Write_Enable();                  //SET WEL 	 
    W25QXX_Wait_Busy();   
  	W25QXX_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(W25X_SectorErase);      //发送扇区擦除指令 
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //发送24bit地址    
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI2_ReadWriteByte((u8)Dst_Addr);  
	  W25QXX_CS=1;                            //取消片选     	      
    W25QXX_Wait_Busy();   				   //等待擦除完成
}  
/*
*********************************************************************************************************
*	函 数 名: W25QXX_Wait_Busy(void)
*	功能说明: 等待空闲
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}  
/*
*********************************************************************************************************
*	函 数 名: W25QXX_PowerDown(void)
*	功能说明: 进入掉电模式
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
//void W25QXX_PowerDown(void)   
//{ 
//  	W25QXX_CS=0;                            //使能器件   
//    SPI2_ReadWriteByte(W25X_PowerDown);        //发送掉电命令  
//	W25QXX_CS=1;                            //取消片选     	      
//    bsp_DelayUS(3);                               //等待TPD  
//}  
/*
*********************************************************************************************************
*	函 数 名: W25QXX_WAKEUP(void)
*	功能说明: 唤醒
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
//void W25QXX_WAKEUP(void)   
//{  
//  	W25QXX_CS=0;                            //使能器件   
//    SPI2_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
//	  W25QXX_CS=1;                            //取消片选     	      
//    bsp_DelayUS(3);                         //等待TRES1
//} 
/*
*********************************************************************************************************
*	函 数 名: SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
*	功能说明: SPI速度设置函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI2->CR1&=0XFFC7;                //位3-5清零，用来设置波特率
	SPI2->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI2,ENABLE);             //使能SPI1
} 
/*
*********************************************************************************************************
*	函 数 名: SPI2_ReadWriteByte(u8 TxData)
*	功能说明: SPI2读写函数
*	形    参: TxData:要写入的字节
*	返 回 值: 读取到的字节
*********************************************************************************************************
*/
u8 SPI2_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个byte  数据
		
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据	
 		    
}
/***************************** 北京龙鼎源科技股份有限公司 (END OF FILE) *********************************/

