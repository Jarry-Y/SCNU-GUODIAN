#include "led.h" 
#include "usart.h"	
#include "delay.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4����ģ��-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com							  
////////////////////////////////////////////////////////////////////////////////// 	 

//��ʼ��PA6��PA7Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ��

  //GPIOF9,F10��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//LED0��LED1��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIO
	
	GPIO_SetBits(GPIOC,GPIO_Pin_13);//���øߣ�����

}

//LED0����
void led0_task(void *pdata)
{	 
	u8 flag=0;
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);  //LED0��Ӧ����GPIOC.13���ͣ���  ��ͬLED0=0;
	while(1)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);  //LED0��Ӧ����GPIOC.13���ͣ���  ��ͬLED0=0;
		delay_ms(500);  
		GPIO_SetBits(GPIOC,GPIO_Pin_13);	   //LED0��Ӧ����GPIOC.13���ߣ���  ��ͬLED0=1;
		delay_ms(500);                     //��ʱ500ms
		
//		//�����жϽ���ָ����֤����
//		if(HMI_REC_LEN)
//		{
//			HMI_REC_LEN = 0;
//			if(flag)
//			{
//				GPIO_ResetBits(GPIOC,GPIO_Pin_13);  //LED0��Ӧ����GPIOC.13���ͣ���  ��ͬLED0=0;
//				flag = 0;
//			}
//			else
//			{
//				GPIO_SetBits(GPIOC,GPIO_Pin_13);  //LED0��Ӧ����GPIOC.13���ͣ���  ��ͬLED0=0;
//				flag = 1;
//			}
//		}
//		delay_ms(50);
	}
}




