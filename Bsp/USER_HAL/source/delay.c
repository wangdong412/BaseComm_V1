/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "includes.h"	
#include "tim3.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define delay_osrunning		OSRunning			//OS是否运行标记,0,不运行;1,在运行
#define delay_tickspersec	OS_TICKS_PER_SEC	//OS时钟节拍,即每秒调度次数
#define delay_intnesting 	OSIntNesting		//中断嵌套级别,即中断嵌套次数
/* Private variables ---------------------------------------------------------*/
static u8  fac_us=0;//us延时倍乘数
static u16 fac_ms=0;//ms延时倍乘数
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

    fac_us=Sysclk/8;						//为系统时钟的1/8
    reload=Sysclk/8;						//每秒钟的计数次数 单位为K
    reload*=1000000/delay_tickspersec;		//根据OS_TICKS_PER_SEC设定溢出时间
                                            //reload为24位寄存器,最大值:16777216,在168M下,约合0.7989s左右
    fac_ms=1000/delay_tickspersec;			//代表ucos可以延时的最少单位
    SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;//开启SYSTICK中断
    SysTick->LOAD=reload; 					//每1/OS_TICKS_PER_SEC秒中断一次
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; //开启SYSTICK

    //TIM3_Init(50000, 84-1);  // 最大计数50ms
}

/**
  * @brief  us级延时时,关闭任务调度(防止打断us级延迟)
  * @param  
  * @retval 
  */ 
void Delay_Schedlock(void)
{
	OSSchedLock();							//UCOSII的方式,禁止调度，防止打断us延时
}

/**
  * @brief  us级延时时,恢复任务调度
  * @param  
  * @retval 
  */ 
void Delay_Schedunlock(void)
{	
	OSSchedUnlock();						//UCOSII的方式,恢复调度
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
	u32 reload=SysTick->LOAD;	//LOAD的值	    	 
	ticks=nus*fac_us; 			//需要的节拍数	  		 
	tcnt=0;
	Delay_Schedlock();			//阻止OS调度，防止打断us延时
	told=SysTick->VAL;        	//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;	//时间超过/等于要延迟的时间,则退出.
		}  
	};
	Delay_Schedunlock();			//恢复OS调度											    
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
  * @brief   调用OS自带的延时函数延时
  * @param   ticks:延时的节拍数
  * @retval 
  */ 
void Delay_OsTimeDly(u32 ticks)
{
	OSTimeDly(ticks);						//UCOSII延时
}

/**
  * @brief  
  * @param  
  * @retval 
  */ 
void Delay_ms(u16 nms)
{	
	if(delay_osrunning && delay_intnesting==0)//如果os已经在跑了	   
	{		  
		if(nms>=fac_ms) //延时的时间大于ucos的最少时间周期 
		{
			Delay_OsTimeDly(nms/fac_ms);	//OS延时
		}
		nms%=fac_ms;						//ucos已经无法提供这么小的延时了,采用普通方式延时    
	}
	Delay_us((u32)(nms*1000));		//普通方式延时 
}


/************************ (C) COPYRIGHT LDY *****END OF FILE****/
