/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "includes.h"	
#include "tim3.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define delay_osrunning		OSRunning			//OS�Ƿ����б��,0,������;1,������
#define delay_tickspersec	OS_TICKS_PER_SEC	//OSʱ�ӽ���,��ÿ����ȴ���
#define delay_intnesting 	OSIntNesting		//�ж�Ƕ�׼���,���ж�Ƕ�״���
/* Private variables ---------------------------------------------------------*/
static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  * @param  
  * @retval 
  */
void DelayUs(u32 n)
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
void Delay_Init(u8 Sysclk)
{
    u32 reload = 0;

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

    fac_us=Sysclk/8;						//Ϊϵͳʱ�ӵ�1/8
    reload=Sysclk/8;						//ÿ���ӵļ������� ��λΪK
    reload*=1000000/delay_tickspersec;		//����OS_TICKS_PER_SEC�趨���ʱ��
                                            //reloadΪ24λ�Ĵ���,���ֵ:16777216,��168M��,Լ��0.7989s����
    fac_ms=1000/delay_tickspersec;			//����ucos������ʱ�����ٵ�λ
    SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;//����SYSTICK�ж�
    SysTick->LOAD=reload; 					//ÿ1/OS_TICKS_PER_SEC���ж�һ��
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; //����SYSTICK

    //TIM3_Init(50000, 84-1);  // ������50ms
}

/**
  * @brief  us����ʱʱ,�ر��������(��ֹ���us���ӳ�)
  * @param  
  * @retval 
  */ 
void Delay_Schedlock(void)
{
	OSSchedLock();							//UCOSII�ķ�ʽ,��ֹ���ȣ���ֹ���us��ʱ
}

/**
  * @brief  us����ʱʱ,�ָ��������
  * @param  
  * @retval 
  */ 
void Delay_Schedunlock(void)
{	
	OSSchedUnlock();						//UCOSII�ķ�ʽ,�ָ�����
}

/**
  * @brief  
  * @param  
  * @retval 
  */    								   
void Delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;	//LOAD��ֵ	    	 
	ticks=nus*fac_us; 			//��Ҫ�Ľ�����	  		 
	tcnt=0;
	Delay_Schedlock();			//��ֹOS���ȣ���ֹ���us��ʱ
	told=SysTick->VAL;        	//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;	//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
	Delay_Schedunlock();			//�ָ�OS����											    
}

/**
  * @brief  
  * @param  
  * @retval 
  */  
u32 Delay_Getus(void)
{
	u32 nus = 0;
	nus = Tim3_Nus + (TIM3->CNT);
	return nus;
}


/**
  * @brief   ����OS�Դ�����ʱ������ʱ
  * @param   ticks:��ʱ�Ľ�����
  * @retval 
  */ 
void Delay_OsTimeDly(u32 ticks)
{
	OSTimeDly(ticks);						//UCOSII��ʱ
}

/**
  * @brief  
  * @param  
  * @retval 
  */ 
void Delay_ms(u16 nms)
{	
	if(delay_osrunning && delay_intnesting==0)//���os�Ѿ�������	   
	{		  
		if(nms>=fac_ms) //��ʱ��ʱ�����ucos������ʱ������ 
		{
			Delay_OsTimeDly(nms/fac_ms);	//OS��ʱ
		}
		nms%=fac_ms;						//ucos�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	Delay_us((u32)(nms*1000));		//��ͨ��ʽ��ʱ 
}


/************************ (C) COPYRIGHT LDY *****END OF FILE****/
