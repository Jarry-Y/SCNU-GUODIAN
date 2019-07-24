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

//OLED任务
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
OS_STK HMI_TASK_STK[HMI_STK_SIZE];
//任务函数
void HMI_task(void *pdata);

//KEY任务
//设置任务优先级
#define KEY_TASK_PRIO       			6 
//设置任务堆栈大小
#define KEY_STK_SIZE  					128
//任务堆栈
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//任务函数
void KEY_task(void *pdata);

/////////////////////////函数声明///////////////////////////////////
void Oled_Show(char DB, u32 Hz, u16 input_hz, u16 add_count);

/////////////////////////变量定义///////////////////////////////////
u16 dacval=500;		//DAC电压 0~3300 分别对应0~3V
u16 pwmf=8000;			//PWM频率（KHz）
u8 PEdb=5;					//衰减器衰减增益（DB）

void All_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      //初始化延时函数
	
	//EXTIX_Init();
	LED_Init();					//初始化LED 
	KEY_Init();
	Dac1_Init();		 		//DAC通道1初始化			A4
	DAC_SetChannel1Data(DAC_Align_12b_R,0);//初始值为0	
	//Adc_Init();         //初始化ADC						A5
 	TIM3_PWM_Init(84000/pwmf-1,1-1);	//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.  		A6口
	uart_init(115200);	//初始化串口波特率为115200
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  //使能串口1的DMA发送 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断，串口接收到数据中断
	//PE_GPIO_Init();			//PE43702端口初始化
	AD9854_Init();		//初始化AD9854
	ADF4351Init();		//初始化ADF4351
	PE4302_Init();			//初始化PE4302
//	OLED_Init();
//	OLED_Clear();
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	
}

int main(void)
{	
	All_Init();
	
	Dac1_Set_Vol(dacval);								//设置DAC输出电压
	TIM_SetCompare1(TIM3,84000/pwmf/2);	//设置PWM输出脉宽
	AD9854_SetSine(40000000,4095);//设置频率和幅值
	ADF4351WriteFreq(400);				//设置频率为400M
	//PE43702Set(PEdb);		//设置PE43702衰减器衰减增益
	PE4302Set(PEdb,1);		//设置PE4302衰减器衰减增益

	OSStart();	
}


//开始任务
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	//OSTaskCreate(Oled_task,(void *)0,(OS_STK*)&OLED_TASK_STK[OLED_STK_SIZE-1],OLED_TASK_PRIO);	 	
 	//OSTaskCreate(HMI_task,(void *)0,(OS_STK*)&HMI_TASK_STK[HMI_STK_SIZE-1],HMI_TASK_PRIO);	 
	OSTaskCreate(KEY_task,(void *)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO);	 	
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
} 