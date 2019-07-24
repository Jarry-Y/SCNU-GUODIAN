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

//OLED����
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
OS_STK HMI_TASK_STK[HMI_STK_SIZE];
//������
void HMI_task(void *pdata);

//KEY����
//�����������ȼ�
#define KEY_TASK_PRIO       			6 
//���������ջ��С
#define KEY_STK_SIZE  					128
//�����ջ
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//������
void KEY_task(void *pdata);

/////////////////////////��������///////////////////////////////////
void Oled_Show(char DB, u32 Hz, u16 input_hz, u16 add_count);

/////////////////////////��������///////////////////////////////////
u16 dacval=500;		//DAC��ѹ 0~3300 �ֱ��Ӧ0~3V
u16 pwmf=8000;			//PWMƵ�ʣ�KHz��
u8 PEdb=5;					//˥����˥�����棨DB��

void All_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);      //��ʼ����ʱ����
	
	//EXTIX_Init();
	LED_Init();					//��ʼ��LED 
	KEY_Init();
	Dac1_Init();		 		//DACͨ��1��ʼ��			A4
	DAC_SetChannel1Data(DAC_Align_12b_R,0);//��ʼֵΪ0	
	//Adc_Init();         //��ʼ��ADC						A5
 	TIM3_PWM_Init(84000/pwmf-1,1-1);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.  		A6��
	uart_init(115200);	//��ʼ�����ڲ�����Ϊ115200
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  //ʹ�ܴ���1��DMA���� 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����жϣ����ڽ��յ������ж�
	//PE_GPIO_Init();			//PE43702�˿ڳ�ʼ��
	AD9854_Init();		//��ʼ��AD9854
	ADF4351Init();		//��ʼ��ADF4351
	PE4302_Init();			//��ʼ��PE4302
//	OLED_Init();
//	OLED_Clear();
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	
}

int main(void)
{	
	All_Init();
	
	Dac1_Set_Vol(dacval);								//����DAC�����ѹ
	TIM_SetCompare1(TIM3,84000/pwmf/2);	//����PWM�������
	AD9854_SetSine(40000000,4095);//����Ƶ�ʺͷ�ֵ
	ADF4351WriteFreq(400);				//����Ƶ��Ϊ400M
	//PE43702Set(PEdb);		//����PE43702˥����˥������
	PE4302Set(PEdb,1);		//����PE4302˥����˥������

	OSStart();	
}


//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	//OSTaskCreate(Oled_task,(void *)0,(OS_STK*)&OLED_TASK_STK[OLED_STK_SIZE-1],OLED_TASK_PRIO);	 	
 	//OSTaskCreate(HMI_task,(void *)0,(OS_STK*)&HMI_TASK_STK[HMI_STK_SIZE-1],HMI_TASK_PRIO);	 
	OSTaskCreate(KEY_task,(void *)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO);	 	
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
} 