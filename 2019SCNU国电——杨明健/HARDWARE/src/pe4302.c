#include "pe4302.h"

/***********PE4302���ų�ʼ������*********/
//��������
void PE4302_GPIO_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_12|GPIO_Pin_14;				 //LED0-->PA.8 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
	 pe4302_LE1_1;
	 pe4302_LE2_1;
}

/***********dB����************/
//pe4302x:��xƬpe4302
//dB:˥��dB
void PE4302Set(unsigned char dB,u8 pe4302x)
{
  unsigned char i;
  unsigned char Set_dB;
  dB = dB&0X3F;
  pe4302_CLK_0;
  if(pe4302x==1)
	pe4302_LE1_0;
  else
	pe4302_LE2_0;
  for(i = 0; i < 6;i++)
  {
    Set_dB = (dB>>5);
    if(Set_dB == 1)
    {
      pe4302_Data_1;
    }
    else
    {
      pe4302_Data_0;
    }
    pe4302_CLK_1;
    dB = (dB << 1)&0X3F;
    pe4302_CLK_0;
  }
  if(pe4302x==1)
	pe4302_LE1_1;
  else
	pe4302_LE2_1;
}

/***********PE4302��ʼ��*************/

void PE4302_Init(void)
{
	PE4302_GPIO_Init();
	PE4302Set(0,1);
	PE4302Set(0,2);
	
}




