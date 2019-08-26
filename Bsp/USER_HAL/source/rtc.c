/* Includes ------------------------------------------------------------------*/
#include "rtc.h"
#include "delay.h"
#include "tim3.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
  * @brief    RTC时间设置
  * @param    hour,min,sec:小时,分钟,秒钟 @RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
  * @retval   SUCEE(1),成功     ERROR(0),进入初始化模式失败 
  */
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
}

/**
  * @brief    RTC日期设置
  * @param    year,month,date:年(0~99),月(1~12),日(0~31) week:星期(1~7,0,非法!)
  * @retval   SUCEE(1),成功     ERROR(0),进入初始化模式失败 
  */
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}


/**
  * @brief  
  * @param  
  * @retval 
  */
u8 Rtc_Init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问 

	RCC_LSICmd(ENABLE);//LSI 开启    
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
	{
		retry++;
		Delay_ms(10);
	}
	if(retry==0)return 1;		//LSE 开启失败. 
		
//	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//是否第一次配置?
	{
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟 

		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC异步分频系数(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTC同步分频系数(0~7FFF)
		RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式
		RTC_Init(&RTC_InitStructure);

		RTC_Set_Time(0,0,0,RTC_H12_AM);	//设置时间
		TIM3_NusReset();
		
		RTC_Set_Date(17,5,12,1);		//设置日期

		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//标记已经初始化过了
	} 

	return 0;
}		

/**
  * @brief    设置闹钟时间(按星期闹铃,24小时制)
  * @param    week:星期几(1~7) @ref  RTC_Alarm_Definitions
  * @retval   hour,min,sec:小时,分钟,秒钟
  */
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//关闭闹钟A 

	RTC_TimeTypeInitStructure.RTC_Hours=hour;//小时
	RTC_TimeTypeInitStructure.RTC_Minutes=min;//分钟
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;//秒
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;

	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;//星期
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;//按星期闹
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;//精确匹配星期，时分秒
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);


	RTC_ClearITPendingBit(RTC_IT_ALRA);//清除RTC闹钟A的标志
	EXTI_ClearITPendingBit(EXTI_Line17);//清除LINE17上的中断标志位 

	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//开启闹钟A中断
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//开启闹钟A 

	EXTI_InitStructure.EXTI_Line = EXTI_Line17;//LINE17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE17
	EXTI_Init(&EXTI_InitStructure);//配置
	
	NVIC_PriorityGroupConfig(NVIC_PRIGROUP);

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_RTC_ALARM_PRE;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_RTC_ALARM_SUB;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
}

/**
  * @brief    周期性唤醒定时器设置  
  * @param  
		wksel:  @ref RTC_Wakeup_Timer_Definitions
		#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
		#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
		#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
		#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
		#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
		#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
  * @retval 
  */
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	RTC_WakeUpCmd(DISABLE);//关闭WAKE UP

	RTC_WakeUpClockConfig(wksel);//唤醒时钟选择

	RTC_SetWakeUpCounter(cnt);//设置WAKE UP自动重装载寄存器

	RTC_ClearITPendingBit(RTC_IT_WUT); //清除RTC WAKE UP的标志
	EXTI_ClearITPendingBit(EXTI_Line22);//清除LINE22上的中断标志位 

	RTC_ITConfig(RTC_IT_WUT,ENABLE);//开启WAKE UP 定时器中断
	RTC_WakeUpCmd( ENABLE);//开启WAKE UP 定时器　

	EXTI_InitStructure.EXTI_Line = EXTI_Line22;//LINE22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE22
	EXTI_Init(&EXTI_InitStructure);//配置
	
	NVIC_PriorityGroupConfig(NVIC_PRIGROUP);

	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_RTC_WKUP_PRE;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_RTC_WKUP_SUB;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
}

/**
  * @brief  
  * @param  
  * @retval 
  */
void Rtc_Test(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	
	printf("Time:%02d:%02d:%02d:%d ", RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,Delay_Getus());
	printf("Date:20%02d-%02d-%02d\r\n",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
}

	
/************************ (C) COPYRIGHT LDY *****END OF FILE****/
