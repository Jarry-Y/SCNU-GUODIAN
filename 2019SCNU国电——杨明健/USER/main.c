#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "dac.h"
#include "adc.h"
#include "pwm.h"
#include "dma.h"	
#include "PE43702.h"
#include "AD9854.h"
#include "ADF4351.h"
#include "PE4302.h"
#include "oled.h"
#include "includes.h"

/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				128
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 			   
//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO       			7 
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		128
//任务堆栈	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);

//LED1任务
//设置任务优先级
#define OLED_TASK_PRIO       			9 
//设置任务堆栈大小
#define OLED_STK_SIZE  					128
//任务堆栈
OS_STK OLED_TASK_STK[OLED_STK_SIZE];
//任务函数
void Oled_task(void *pdata);

//HMI任务
//设置任务优先级
#define HMI_TASK_PRIO       			8 
//设置任务堆栈大小
#define HMI_STK_SIZE  					128
//任务堆栈
OS_STK OHMI_TASK_STK[HMI_STK_SIZE];
//任务函数
void HMI_task(void *pdata);

/////////////////////////函数声明///////////////////////////////////
void Oled_Show(char DB, u32 Hz, u16 input_hz, u16 add_count);

/////////////////////////变量定义///////////////////////////////////
u16 dacval=1500;		//DAC电压 0~3300 分别对应0~3V
u16 pwmf=8000;			//PWM频率（KHz）
u8 PEdb=5;					//衰减器衰减增益（DB）
char DB=0;
u32 Hz=0;
u16 input_hz = 2;
u16 add_count=10;

const u8 TEXT_TO_SEND[]={"add 1,0,150"};	 
#define SEND_BUF_SIZE (sizeof(TEXT_TO_SEND)+2)*8	//发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.
u8 SendBuff[SEND_BUF_SIZE];												//发送数据缓冲区


void All_Init(void)
{
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      //初始化延时函数
	
	//EXTIX_Init();
	LED_Init();					//初始化LED 
	Dac1_Init();		 		//DAC通道1初始化			A4
	DAC_SetChannel1Data(DAC_Align_12b_R,0);//初始值为0	
	//Adc_Init();         //初始化ADC						A5
 	TIM3_PWM_Init(84000/pwmf-1,1-1);	//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.  		A6口
	uart_init(115200);	//初始化串口波特率为115200
	MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)SendBuff,SEND_BUF_SIZE);//DMA2,STEAM7,CH4,外设为串口1,存储器为SendBuff,长度为:SEND_BUF_SIZE.
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  //使能串口1的DMA发送 
	//PE_GPIO_Init();			//PE43702端口初始化
	//AD9854_Init();		//初始化AD9854
	//ADF4351Init();		//初始化ADF4351
	//PE4302_Init();			//初始化PE4302
	OLED_Init();
	OLED_Clear();
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	
}

int main(void)
{	
	u16 i;
	u8 t=0;
	u8 j,mask=0;
	
	All_Init();
	
	Dac1_Set_Vol(dacval);								//设置DAC输出电压
	TIM_SetCompare1(TIM3,84000/pwmf/2);	//设置PWM输出脉宽
	//AD9854_SetSine(40000000,4095);//设置频率和幅值
	//ADF4351WriteFreq(400);				//设置频率为400M
	//PE43702Set(PEdb);		//设置PE43702衰减器衰减增益
	//PE4302Set(PEdb,1);		//设置PE4302衰减器衰减增益
	
	j=sizeof(TEXT_TO_SEND);	   
	for(i=0;i<SEND_BUF_SIZE;i++)//填充ASCII字符集数据
  {
		if(t==j-1)//加入换行符
		{
			SendBuff[i]=0xff;
			i++;
			SendBuff[i]=0xff;
			i++;
			SendBuff[i]=0xff;
			t=0;
		}
		else//复制TEXT_TO_SEND语句
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}   	   
  }		

	OSStart();	
}


//开始任务
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	OSTaskCreate(Oled_task,(void *)0,(OS_STK*)&OLED_TASK_STK[OLED_STK_SIZE-1],OLED_TASK_PRIO);	 	
 	OSTaskCreate(HMI_task,(void *)0,(OS_STK*)&OHMI_TASK_STK[OLED_STK_SIZE-1],HMI_TASK_PRIO);	 			
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
} 

//LED0任务
void led0_task(void *pdata)
{	 	
	while(1)
	{
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);  //LED0对应引脚GPIOC.13拉低，亮  等同LED0=0;
	delay_ms(500);  
	GPIO_SetBits(GPIOC,GPIO_Pin_13);	   //LED0对应引脚GPIOC.13拉高，灭  等同LED0=1;
	delay_ms(500);                     //延时500ms
	}
}

//OLED任务
void Oled_task(void *pdata)
{	  
	while(1)
	{
		//oled显示数据
		Oled_Show(DB,Hz,input_hz,add_count);
		delay_ms(20);                     //延时500ms
	}
}

//HMI任务
void HMI_task(void *pdata)
{	  
	while(1)
	{
		//DMA串口发送数据 
		MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);     //开始一次DMA传输！	  
		if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//等待DMA2_Steam7传输完成
		{ 
			DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);	//清除DMA2_Steam7传输完成标志
		}	
		else																					//如果未传输完数据，则让出CPU
		{
			delay_ms(5);
		}
		delay_ms(5);
	}
}

void Oled_Show(char DB, u32 Hz, u16 input_hz, u16 add_count)
{
	OLED_ShowCHinese(0,0,0);//程
	OLED_ShowCHinese(13,0,1);//控
	OLED_ShowCHinese(26,0,2);//放
	OLED_ShowCHinese(39,0,3);//大
	OLED_ShowCHinese(52,0,14);//:
	OLED_ShowCHinese(78,0,15);//DB
	
	OLED_ShowChar(0,2,'I',12);
	OLED_ShowChar(8,2,'N',12);
	OLED_ShowChar(58,2,'H',12);
	OLED_ShowChar(64,2,'z',12);
	
	/*OLED_ShowCHinese(0,3,4);//截
	OLED_ShowCHinese(13,3,5);//止
	OLED_ShowCHinese(26,3,6);//频
	OLED_ShowCHinese(39,3,7);//率
	OLED_ShowCHinese(52,3,14);//:
	OLED_ShowChar(75,3,'k',12);
	OLED_ShowCHinese(83,3,16);//Hz*/
	
	OLED_ShowChar(0,4,'A',12);
	OLED_ShowChar(8,4,'D',12);
	OLED_ShowChar(16,4,'D',12);
	
	
	OLED_ShowCHinese(0,6,8);//燕
	OLED_ShowCHinese(13,6,9);//泽
	OLED_ShowCHinese(26,6,10);//昊
	OLED_ShowCHinese(52,6,11);//周
	OLED_ShowCHinese(65,6,12);//欣
	OLED_ShowCHinese(78,6,13);//欣
	
	OLED_ShowNum(60,0,DB,2,12);
	//OLED_ShowNum(60,3,Hz/1000,2,12);	
	OLED_ShowNum(20,2,input_hz*10,6,12);
	OLED_ShowNum(26,4,add_count,5,12);
}


