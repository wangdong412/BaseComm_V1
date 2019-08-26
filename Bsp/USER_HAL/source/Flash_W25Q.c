/*
*********************************************************************************************************
*
*	ģ������ : �ⲿSRAM����
*	�ļ����� : bsp_sram.c
*	��    �� : V1.0
*	˵    �� : ���İ�SRAM����,SI62WV51216������1MB
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		
*
*	Copyright (C), 2016-2020, ��������Դ�Ƽ��ɷ����޹�˾
*
*********************************************************************************************************
*/
 
#include "Flash_W25Q.h"

/*
*********************************************************************************************************
*	�� �� ��: bsp_W25QXX_Init
*	����˵��: ��W25Q128���г�ʼ������ʼ��SPI FLASH��IO�ڣ�
            4KbytesΪһ��Sector��16������Ϊ1��Block������Ϊ16M�ֽ�,����128��Block,4096��Sector 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
u16 W25QXX_TYPE=W25Q128;	//Ĭ����W25Q128

void bsp_W25QXX_Init(void)
{ 
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PB12
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��

    W25QXX_CS=1;			//SPI FLASH��ѡ��
    SPI2_Init();		   			//��ʼ��SPI
    SPI2_SetSpeed(SPI_BaudRatePrescaler_2);		//����Ϊ42Mʱ��,����ģʽ
    W25QXX_TYPE=W25QXX_ReadID();	//��ȡFLASH ID.
}  
/*
*********************************************************************************************************
*	�� �� ��: SPI2_Init(void)
*	����˵��: SPI�ڳ�ʼ�������ó�����ģʽ 		
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
void SPI2_Init(void)
{	 
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//ʹ��SPI2ʱ��


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PB13~15���ù������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2); //PB3����Ϊ SPI2
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2); //PB4����Ϊ SPI2
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2); //PB5����Ϊ SPI2

    //����ֻ���SPI�ڳ�ʼ��
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);//��λSPI1
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//ֹͣ��λSPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
    SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����

    SPI2_ReadWriteByte(0xff);//��������
}   
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_ReadSR(void)
*	����˵��: ��ȡW25QXX��״̬�Ĵ���
            BIT7  6   5   4   3   2   1   0
            SPR   RV  TB BP2 BP1 BP0 WEL BUSY
            SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
            TB,BP2,BP1,BP0:FLASH����д��������
            WEL:дʹ������
            BUSY:æ���λ(1,æ;0,����)
            Ĭ��:0x00
*	��    ��: ��
*	�� �� ֵ: һ���ֽڣ�W25QXX�Ĵ���״̬
*********************************************************************************************************
*/ 
u8 W25QXX_ReadSR(void)   
{  
	u8 byte=0;   
	W25QXX_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI2_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     
	return byte;   
} 
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_Write_SR(u8 sr)
*	����˵��: дW25QXX״̬�Ĵ���
            ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
*	��    ��: u8 sr��
*	�� �� ֵ: һ���ֽڣ�W25QXX�Ĵ���״̬
*********************************************************************************************************
*/ 
//void W25QXX_Write_SR(u8 sr)   
//{   
//	W25QXX_CS=0;                            //ʹ������   
//	SPI2_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
//	SPI2_ReadWriteByte(sr);               //д��һ���ֽ�  
//	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
//}   
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_Write_Enable(void)
*	����˵��: ��WEL��λ 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS=0;                            //ʹ������   
    SPI2_ReadWriteByte(W25X_WriteEnable); //����дʹ��  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
} 
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_Write_Disable(void) 
*	����˵��: W25QXXд��ֹ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS=0;                            //ʹ������   
    SPI2_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
} 	
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_ReadID(void) 
*	����˵��: ��ȡоƬID
*	��    ��: ��
*	�� �� ֵ: 0XEF13,��ʾоƬ�ͺ�ΪW25Q80 
            0XEF14,��ʾоƬ�ͺ�ΪW25Q16
            0XEF15,��ʾоƬ�ͺ�ΪW25Q32
            0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
            0XEF17,��ʾоƬ�ͺ�ΪW25Q128 
*********************************************************************************************************
*/
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;	  
	W25QXX_CS=0;				    
	SPI2_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
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
*	�� �� ��: bsp_W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
*	����˵��: ��ָ����ַ��ʼ��ȡָ�����ȵ�����
*	��    ��: pBuffer:���ݴ洢����ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)��
            NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	W25QXX_CS=0;                            //ʹ������   
    SPI2_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
    SPI2_ReadWriteByte((u8)((ReadAddr)>>16));  //����24bit��ַ    
    SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI2_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	  { 
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);   //ѭ������  
    }
	W25QXX_CS=1;  				    	      
} 
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
*	����˵��: ��ָ����ַ��ʼд�����256�ֽڵ�����
*	��    ��: pBuffer:���ݴ洢����WriteAddr:��ʼд��ĵ�ַ(24bit)��
            NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    W25QXX_Write_Enable();                  //SET WEL 
	W25QXX_CS=0;                            //ʹ������   
    SPI2_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
    SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); //����24bit��ַ    
    SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI2_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI2_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	W25QXX_CS=1;                            //ȡ��Ƭѡ 
	W25QXX_Wait_Busy();					   //�ȴ�д�����
} 
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
*	����˵��: �޼���дSPI FLASH������ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
            �����Զ���ҳ���� ����ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!��
*	��    ��: pBuffer:���ݴ洢����WriteAddr:��ʼд��ĵ�ַ(24bit)��
            NumByteToWrite:Ҫд����ֽ���(���65535)	
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		bsp_W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
}
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
*	����˵��: дSPI FLASH����ָ����ַ��ʼд��ָ�����ȵ����ݣ��ú�������������!
*	��    ��: pBuffer:���ݴ洢����WriteAddr:��ʼд��ĵ�ַ(24bit)��
            NumByteToWrite:Ҫд����ֽ���(���65535)	
*	�� �� ֵ: ��
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
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		bsp_W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;  //д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 
}
///*
//*********************************************************************************************************
//*	�� �� ��: W25QXX_Erase_Chip(void) 
//*	����˵��: ��������оƬ���ȴ�ʱ��150ms*16
//*	��    ��: ��
//*	�� �� ֵ: ��
//*********************************************************************************************************
//*/
//void W25QXX_Erase_Chip(void)   
//{                                   
//    W25QXX_Write_Enable();                  //SET WEL 
//    W25QXX_Wait_Busy();   
//  	W25QXX_CS=0;                            //ʹ������   
//    SPI2_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
//	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
//	W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
//}
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_Erase_Sector(u32 Dst_Addr)
*	����˵��: ����һ������
*	��    ��: Dst_Addr:������ַ ����ʵ���������ã�����һ��ɽ��������ʱ��:150ms
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void W25QXX_Erase_Sector(u32 Dst_Addr)   
{  
	//����falsh�������,������   
// 	printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
    W25QXX_Write_Enable();                  //SET WEL 	 
    W25QXX_Wait_Busy();   
  	W25QXX_CS=0;                            //ʹ������   
    SPI2_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI2_ReadWriteByte((u8)Dst_Addr);  
	  W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
    W25QXX_Wait_Busy();   				   //�ȴ��������
}  
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_Wait_Busy(void)
*	����˵��: �ȴ�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
}  
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_PowerDown(void)
*	����˵��: �������ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//void W25QXX_PowerDown(void)   
//{ 
//  	W25QXX_CS=0;                            //ʹ������   
//    SPI2_ReadWriteByte(W25X_PowerDown);        //���͵�������  
//	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
//    bsp_DelayUS(3);                               //�ȴ�TPD  
//}  
/*
*********************************************************************************************************
*	�� �� ��: W25QXX_WAKEUP(void)
*	����˵��: ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//void W25QXX_WAKEUP(void)   
//{  
//  	W25QXX_CS=0;                            //ʹ������   
//    SPI2_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
//	  W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
//    bsp_DelayUS(3);                         //�ȴ�TRES1
//} 
/*
*********************************************************************************************************
*	�� �� ��: SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
*	����˵��: SPI�ٶ����ú���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
	SPI2->CR1&=0XFFC7;                //λ3-5���㣬�������ò�����
	SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
	SPI_Cmd(SPI2,ENABLE);             //ʹ��SPI1
} 
/*
*********************************************************************************************************
*	�� �� ��: SPI2_ReadWriteByte(u8 TxData)
*	����˵��: SPI2��д����
*	��    ��: TxData:Ҫд����ֽ�
*	�� �� ֵ: ��ȡ�����ֽ�
*********************************************************************************************************
*/
u8 SPI2_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ��byte  ����
		
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
 
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����	
 		    
}
/***************************** ��������Դ�Ƽ��ɷ����޹�˾ (END OF FILE) *********************************/

