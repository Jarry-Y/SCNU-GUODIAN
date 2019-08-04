#include "key.h"
#include "includes.h"

//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4����ģ��-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com							  
////////////////////////////////////////////////////////////////////////////////// 	 

//������ʼ������
void KEY_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_5|GPIO_Pin_4; //4λд����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB7,6,5,4
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1|GPIO_Pin_0; //4λ������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB3,2,1,0
} 

/*��������:KEY_Scan
*	��������:����ɨ�躯��
*	����:��
*	���:���̱��
*/
int KEY_Scan(void)
{	 
	u8 KeyLData;	//������4λ
	u8 j;					//ɨ������
	u8 i;					//ɨ������
	u8 KCC;				//ѭ��ɨ����
	int key;
	key = -1;
	GPIOB->ODR = (GPIOB->ODR & 0xff0f) | 0x00f0;	//����B7-B4Ϊ�ߵ�ƽ
	delay_us(10);				//������㷴Ӧʱ��
	//KeyLData = GPIOB->ODR & 0x000f;	//��B�ڵ�4λ
	//USART_SendData(USART1,PBin(2));
	KeyLData = H3<<3|0|H1<<1|H0;	//��B�ڵ�4λ��B2�������⣬�޷�����
	if(KeyLData != 0x00)						//����а�������
	{
		delay_ms(10);									//��ʼ����				
		if(KeyLData != 0x00)					//�����Ȼ�а�������
		{
			KCC = 0x10;
			for(j=0;j<4;j++)
			{
				GPIOB->ODR = (GPIOB->ODR & 0xff0f) | KCC;	//����������������ø߽���ѭ��ɨ��
				delay_us(10);		//������㷴Ӧʱ��
				KeyLData = H3<<3|0|H1<<1|H0;	//��B�ڵ�4λ��B2�������⣬�޷�����
				if(KeyLData != 0x00)					//�ҵ���ֵ
				{
					switch(KeyLData)						//��ֵת��
					{
						case 0x01:i = 0; break;
						case 0x02:i = 1; break;
						case 0x04:i = 2; break;
						case 0x08:i = 3; break;
					}
					key = i*10 + j;					//�����ֵ
					break;									//����ѭ��
				}
				KCC<<=1;									//KCC����һλ
			}
		}
	}
	return key;
}

//KEY����
void KEY_task(void *pdata)
{	 
	int key = -1;
	u16 test;
	while(1)
	{
		//����ɨ��
		key = KEY_Scan();
		if(key>=0)
		{
			USART_SendData(USART1,key);
		}	
		delay_ms(60);                     //��ʱ60ms
	}
}