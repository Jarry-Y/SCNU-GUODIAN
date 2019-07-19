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

/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				128
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
 			   
//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO       			7 
//���������ջ��С
#define LED0_STK_SIZE  		    		128
//�����ջ	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);

//LED1����
//�����������ȼ�
#define OLED_TASK_PRIO       			9 
//���������ջ��С
#define OLED_STK_SIZE  					128
//�����ջ
OS_STK OLED_TASK_STK[OLED_STK_SIZE];
//������
void Oled_task(void *pdata);

//HMI����
//�����������ȼ�
#define HMI_TASK_PRIO       			8 
//���������ջ��С
#define HMI_STK_SIZE  					128
//�����ջ
OS_STK OHMI_TASK_STK[HMI_STK_SIZE];
//������
void HMI_task(void *pdata);

/////////////////////////��������///////////////////////////////////
void Oled_Show(char DB, u32 Hz, u16 input_hz, u16 add_count);

/////////////////////////��������///////////////////////////////////
u16 dacval=1500;		//DAC��ѹ 0~3300 �ֱ��Ӧ0~3V
u16 pwmf=8000;			//PWMƵ�ʣ�KHz��
u8 PEdb=5;					//˥����˥�����棨DB��
char DB=0;
u32 Hz=0;
u16 input_hz = 2;
u16 add_count=10;

const u8 TEXT_TO_SEND[]={"add 1,0,150"};	 
#define SEND_BUF_SIZE (sizeof(TEXT_TO_SEND)+2)*8	//�������ݳ���,��õ���sizeof(TEXT_TO_SEND)+2��������.
u8 SendBuff[SEND_BUF_SIZE];												//�������ݻ�����


void All_Init(void)
{
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);      //��ʼ����ʱ����
	
	//EXTIX_Init();
	LED_Init();					//��ʼ��LED 
	Dac1_Init();		 		//DACͨ��1��ʼ��			A4
	DAC_SetChannel1Data(DAC_Align_12b_R,0);//��ʼֵΪ0	
	//Adc_Init();         //��ʼ��ADC						A5
 	TIM3_PWM_Init(84000/pwmf-1,1-1);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.  		A6��
	uart_init(115200);	//��ʼ�����ڲ�����Ϊ115200
	MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)SendBuff,SEND_BUF_SIZE);//DMA2,STEAM7,CH4,����Ϊ����1,�洢��ΪSendBuff,����Ϊ:SEND_BUF_SIZE.
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  //ʹ�ܴ���1��DMA���� 
	//PE_GPIO_Init();			//PE43702�˿ڳ�ʼ��
	//AD9854_Init();		//��ʼ��AD9854
	//ADF4351Init();		//��ʼ��ADF4351
	//PE4302_Init();			//��ʼ��PE4302
	OLED_Init();
	OLED_Clear();
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	
}

int main(void)
{	
	u16 i;
	u8 t=0;
	u8 j,mask=0;
	
	All_Init();
	
	Dac1_Set_Vol(dacval);								//����DAC�����ѹ
	TIM_SetCompare1(TIM3,84000/pwmf/2);	//����PWM�������
	//AD9854_SetSine(40000000,4095);//����Ƶ�ʺͷ�ֵ
	//ADF4351WriteFreq(400);				//����Ƶ��Ϊ400M
	//PE43702Set(PEdb);		//����PE43702˥����˥������
	//PE4302Set(PEdb,1);		//����PE4302˥����˥������
	
	j=sizeof(TEXT_TO_SEND);	   
	for(i=0;i<SEND_BUF_SIZE;i++)//���ASCII�ַ�������
  {
		if(t==j-1)//���뻻�з�
		{
			SendBuff[i]=0xff;
			i++;
			SendBuff[i]=0xff;
			i++;
			SendBuff[i]=0xff;
			t=0;
		}
		else//����TEXT_TO_SEND���
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}   	   
  }		

	OSStart();	
}


//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	OSTaskCreate(Oled_task,(void *)0,(OS_STK*)&OLED_TASK_STK[OLED_STK_SIZE-1],OLED_TASK_PRIO);	 	
 	OSTaskCreate(HMI_task,(void *)0,(OS_STK*)&OHMI_TASK_STK[OLED_STK_SIZE-1],HMI_TASK_PRIO);	 			
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
} 

//LED0����
void led0_task(void *pdata)
{	 	
	while(1)
	{
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);  //LED0��Ӧ����GPIOC.13���ͣ���  ��ͬLED0=0;
	delay_ms(500);  
	GPIO_SetBits(GPIOC,GPIO_Pin_13);	   //LED0��Ӧ����GPIOC.13���ߣ���  ��ͬLED0=1;
	delay_ms(500);                     //��ʱ500ms
	}
}

//OLED����
void Oled_task(void *pdata)
{	  
	while(1)
	{
		//oled��ʾ����
		Oled_Show(DB,Hz,input_hz,add_count);
		delay_ms(20);                     //��ʱ500ms
	}
}

//HMI����
void HMI_task(void *pdata)
{	  
	while(1)
	{
		//DMA���ڷ������� 
		MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);     //��ʼһ��DMA���䣡	  
		if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//�ȴ�DMA2_Steam7�������
		{ 
			DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);	//���DMA2_Steam7������ɱ�־
		}	
		else																					//���δ���������ݣ����ó�CPU
		{
			delay_ms(5);
		}
		delay_ms(5);
	}
}

void Oled_Show(char DB, u32 Hz, u16 input_hz, u16 add_count)
{
	OLED_ShowCHinese(0,0,0);//��
	OLED_ShowCHinese(13,0,1);//��
	OLED_ShowCHinese(26,0,2);//��
	OLED_ShowCHinese(39,0,3);//��
	OLED_ShowCHinese(52,0,14);//:
	OLED_ShowCHinese(78,0,15);//DB
	
	OLED_ShowChar(0,2,'I',12);
	OLED_ShowChar(8,2,'N',12);
	OLED_ShowChar(58,2,'H',12);
	OLED_ShowChar(64,2,'z',12);
	
	/*OLED_ShowCHinese(0,3,4);//��
	OLED_ShowCHinese(13,3,5);//ֹ
	OLED_ShowCHinese(26,3,6);//Ƶ
	OLED_ShowCHinese(39,3,7);//��
	OLED_ShowCHinese(52,3,14);//:
	OLED_ShowChar(75,3,'k',12);
	OLED_ShowCHinese(83,3,16);//Hz*/
	
	OLED_ShowChar(0,4,'A',12);
	OLED_ShowChar(8,4,'D',12);
	OLED_ShowChar(16,4,'D',12);
	
	
	OLED_ShowCHinese(0,6,8);//��
	OLED_ShowCHinese(13,6,9);//��
	OLED_ShowCHinese(26,6,10);//�
	OLED_ShowCHinese(52,6,11);//��
	OLED_ShowCHinese(65,6,12);//��
	OLED_ShowCHinese(78,6,13);//��
	
	OLED_ShowNum(60,0,DB,2,12);
	//OLED_ShowNum(60,3,Hz/1000,2,12);	
	OLED_ShowNum(20,2,input_hz*10,6,12);
	OLED_ShowNum(26,4,add_count,5,12);
}


