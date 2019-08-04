	/**********************************************************
                       ��������
										 
���ܣ�ADF4351���Ҳ���Ƶ�������Χ35M-4400M������0.1M��stm32f103rct6����
			��ʾ��12864
�ӿڣ�CLK-PC11 DATA-PC10 LE-PC9 CE-PC12  �����ӿ������key.h
ʱ�䣺2015/11/3
�汾��1.0
���ߣ���������
����������������ԭ�ӣ�����Լ���������δ��������ɣ��������������κ���;

������������뵽�Ա��꣬�������ӽ߳�Ϊ������ ^_^
https://shop110336474.taobao.com/?spm=a230r.7195193.1997079397.2.Ic3MRJ

**********************************************************/
#include "ADF4351.h"
#include "delay.h"
#include "includes.h"

//#define 
#define ADF4351_R0			((u32)0X2C8018)
#define ADF4351_R1			((u32)0X8029)
#define ADF4351_R2			((u32)0X10E42)
#define ADF4351_R3			((u32)0X4B3)
#define ADF4351_R4			((u32)0XEC803C)
#define ADF4351_R5			((u32)0X580005)

#define ADF4351_R1_Base	((u32)0X8001)
#define ADF4351_R4_Base	((u32)0X8C803C)
#define ADF4351_R4_ON	  ((u32)0X8C803C)
#define ADF4351_R4_OFF	((u32)0X8C883C)

//#define ADF4351_RF_OFF	((u32)0XEC801C)

#define ADF4351_PD_ON		((u32)0X10E42)
#define ADF4351_PD_OFF	((u32)0X10E02)

u16 interval=0;			//ɨƵ���
u8 Adc_data[320];		//ADC�ɼ��ص�����

void ADF_Output_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}

void ADF_Input_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

void delay (int length)
{
	while (length >0)
    	length--;
}

void WriteToADF4351(u8 count, u8 *buf)
{
	u8 ValueToWrite = 0;
	u8 i = 0;
	u8 j = 0;
	
//	ADF_Output_GPIOInit();
	
	ADF4351_CE = 1;
	delay_us(1);
	ADF4351_CLK = 0;
	ADF4351_LE = 0;
	delay_us(1);
	
	for(i = count; i>0; i--)
	{
		ValueToWrite = *(buf+i-1);
		for(j=0; j<8; j++)
		{
			if(0x80 == (ValueToWrite & 0x80))
			{
				ADF4351_OUTPUT_DATA = 1;
			}
			else
			{
				ADF4351_OUTPUT_DATA = 0;
			}
			delay_us(1);
			ADF4351_CLK = 1;
			delay_us(1);
			ValueToWrite <<= 1;
			ADF4351_CLK = 0;	
		}
	}
	ADF4351_OUTPUT_DATA = 0;
	delay_us(1);
	ADF4351_LE = 1;
	delay_us(1);
	ADF4351_LE = 0;
}


void ReadToADF4351(u8 count, u8 *buf)
{
	u8 i = 0;
	u8 j = 0;
	u8 iTemp = 0;
	u8 RotateData = 0;
	
	ADF_Input_GPIOInit();
	ADF4351_CE = 1;
	delay_us(1);
	ADF4351_CLK = 0;
	ADF4351_LE = 0;
	delay_us(1);
	
	for(i = count; i>0; i--)
	{
		for(j = 0; j<8; j++)
		{
			RotateData <<=1;
			delay_us(1);
			iTemp = ADF4351_INPUT_DATA;
			ADF4351_CLK = 1;
			if(0x01 == (iTemp&0x01))
			{
				RotateData |= 1;
			}
			delay_us(1);
			ADF4351_CLK = 0;
		}
		*(buf+i-1) = RotateData;
	}
	delay_us(1);
	ADF4351_LE = 1;
	delay_us(1);
	ADF4351_LE = 0;
}


void ADF4351Init(void)
{
	u8 buf[4] = {0,0,0,0};
	
	ADF_Output_GPIOInit();
	
	buf[3] = 0x00;
	buf[2] = 0x58;
	buf[1] = 0x00;				//write communication register 0x00580005 to control the progress 
 	buf[0] = 0x05;				//to write Register 5 to set digital lock detector
	WriteToADF4351(4,buf);		

	buf[3] = 0x00;
	buf[2] = 0xec;				//(DB23=1)The signal is taken from the VCO directly;(DB22-20:4H)the RF divider is 16;(DB19-12:50H)R is 80
	buf[1] = 0x80;				//(DB11=0)VCO powerd up;
 	buf[0] = 0x3C;				//(DB5=1)RF output is enabled;(DB4-3=3H)Output power level is 5
	WriteToADF4351(4,buf);		

	buf[3] = 0x00;
	buf[2] = 0x00;
	buf[1] = 0x04;				//(DB14-3:96H)clock divider value is 150.
 	buf[0] = 0xB3;
	WriteToADF4351(4,buf);	

	buf[3] = 0x00;
	buf[2] = 0x01;				//(DB6=1)set PD polarity is positive;(DB7=1)LDP is 6nS;
	buf[1] = 0x0E;				//(DB8=0)enable fractional-N digital lock detect;
 	buf[0] = 0x42;				//(DB12-9:7H)set Icp 2.50 mA;
	WriteToADF4351(4,buf);		//(DB23-14:1H)R counter is 1

	buf[3] = 0x00;
	buf[2] = 0x00;
	buf[1] = 0x80;			   //(DB14-3:6H)MOD counter is 6;
 	buf[0] = 0x29;			   //(DB26-15:6H)PHASE word is 1,neither the phase resync 
	WriteToADF4351(4,buf);	   //nor the spurious optimization functions are being used
							   //(DB27=1)prescaler value is 8/9

	buf[3] = 0x00;
	buf[2] = 0x2c;
	buf[1] = 0x80;
 	buf[0] = 0x18;				//(DB14-3:0H)FRAC value is 0;
	WriteToADF4351(4,buf);		//(DB30-15:140H)INT value is 320;
	
	
//	WriteOneRegToADF4351(ADF4351_R0);
//	WriteOneRegToADF4351(ADF4351_R1);
//	WriteOneRegToADF4351(ADF4351_R2);
//	WriteOneRegToADF4351(ADF4351_R3);
//	WriteOneRegToADF4351(ADF4351_R4);
//	WriteOneRegToADF4351(ADF4351_R5);
	
}
void WriteOneRegToADF4351(u32 Regster)
{
	u8 buf[4] = {0,0,0,0};
	buf[3] = (u8)((Regster>>24)&(0X000000FF));
	buf[2] = (u8)((Regster>>16)&(0X000000FF));
	buf[1] = (u8)((Regster>>8) &(0X000000FF));
	buf[0] = (u8)((Regster)&(0X000000FF));
	WriteToADF4351(4,buf);	
}
void ADF4351_Init_some(void)
{
	WriteOneRegToADF4351(ADF4351_R2);
	WriteOneRegToADF4351(ADF4351_R3);
	WriteOneRegToADF4351(ADF4351_R5);
}

void ADF4351WriteFreq(float Fre)		//	(xx.x) M Hz
{
	u16 Fre_temp, N_Mul = 1, Mul_Core = 0;
	u16 INT_Fre, Frac_temp, Mod_temp, i;
	u32 W_ADF4351_R0 = 0, W_ADF4351_R1 = 0, W_ADF4351_R4 = 0;
	float multiple;
	
	if(Fre < 35.0)
		Fre = 35.0;
	if(Fre > 4400.0)
		Fre = 4400.0;
	Mod_temp = 1000;
	Fre = ((float)((u32)(Fre*10)))/10;
	
	Fre_temp = Fre;
	for(i = 0; i < 10; i++)
	{
		if(((Fre_temp*N_Mul) >= 2199.9) && ((Fre_temp*N_Mul) <= 4400.1))
			break;
		Mul_Core++;
		N_Mul = N_Mul*2;
	}
	
	multiple = (Fre*N_Mul)/25;		//������ѹ̶�Ϊ�ο�25M�����Ĳο�Ƶ�ʣ���Ӧ�޸�R4���ֽ�
	INT_Fre = (u16)multiple;
//	if(((u32)(multiple*10000)%10)> 5)
//		multiple += 0.001;
	Frac_temp = ((u32)(multiple*1000))%1000;
	while(((Frac_temp%5) == 0) && ((Mod_temp%5) == 0))
	{
		Frac_temp = Frac_temp/5;
		Mod_temp = Mod_temp/5;
	}
	while(((Frac_temp%2) == 0)&&((Mod_temp%2) == 0))
	{
		Frac_temp = Frac_temp/2;
		Mod_temp = Mod_temp/2;
	}
	
	Mul_Core %= 7;
	W_ADF4351_R0 = (INT_Fre<<15)+(Frac_temp<<3);
	W_ADF4351_R1 = ADF4351_R1_Base + (Mod_temp<<3);
	W_ADF4351_R4 = ADF4351_R4_ON + (Mul_Core<<20);
	
	
//	WriteOneRegToADF4351(ADF4351_PD_OFF); //ADF4351_RF_OFF
//	WriteOneRegToADF4351((u32)(ADF4351_R4_OFF + (6<<20)));
	WriteOneRegToADF4351(ADF4351_RF_OFF);
	WriteOneRegToADF4351(W_ADF4351_R0);
	WriteOneRegToADF4351(W_ADF4351_R1);
	WriteOneRegToADF4351(W_ADF4351_R4);
//	WriteOneRegToADF4351(ADF4351_PD_ON);
	
//	WriteOneRegToADF4351((u32)(ADF4351_R4_ON + (Mul_Core<<20)));
}

/***********ɨƵ************/
//����100KHz
//fl:ɨƵ���Ƶ��	(xx.x MHz)
//fh:ɨ�����Ƶ��	(xx.x MHz)
//time:ɨƵʱ��		(s) һ��Ϊ1~5
void sweep(float fl,float fh)				
{
	float bw	=	fh-fl;	//ɨƵ����
	u16 times = bw*10;	//ɨƵ����
	float freq = fh;		//���Ƶ��
	//AD�ɼ�
	u16 adcx;
	float temp;
	u16 del_part;	//С������
	u16 cnt=0;		//�������ݼ���
	int i;
	//HMI������
	u8 cmd[]="add 1,0,";
	u8 buf[3] = {0}; 
	u8 order[11];
	
	while(1)
	{
		interval = PLL_SWEEP_TIME*1000/times;
		if(PLL_SWEEP_ENABLE)
		{
			ADF4351_Init_some();
			ADF4351WriteFreq(freq);				//����Ƶ��
			if(freq<=fl)									//���ĿǰƵ��С�ڵ������Ƶ�ʣ�������ɨƵ
			{
				freq = fh;
			}
			else
			{
				freq -= 0.1;								//����100KHz
			}
			
			//AD����
			adcx=Get_Adc2_Average(ADC_Channel_14,1);	//��ȡͨ��5��ת��ֵ��20��ȡƽ��
			temp=(float)adcx*(3.3/4096);          	//��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111
			Adc_data[cnt]=temp*255.0/3.3;
//			USART_SendData(USART1,Adc_data[cnt]);
//			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			
			//HMI������
			if(HMI_ADDT_ORDER ==1)
			{
				//u8ת�ַ�����
				buf[2]=Adc_data[cnt]/100 + 0x30;
				buf[1]=Adc_data[cnt]/10 - Adc_data[cnt]/100*10 +0x30;
				buf[0]=Adc_data[cnt]%10	+ 0x30;
//				for(i=2;i>=0;i--)
//				{
//					USART_SendData(USART1,buf[i]);
//					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//				}
//				USART_SendData(USART1,buf[0]);
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
				for(i=0;i<11;i++)
				{
					if(i<8)
					{
						order[i]=cmd[i];
					}
					else
					{
						order[i]=buf[10-i];
					}
//					USART_SendData(USART1,order[i]);
//					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
				}
				
				HMI_sendorder(order,sizeof(order));	
				while(1)
				{
					if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//�ȴ�DMA2_Steam7�������
					{ 
						DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);	//���DMA2_Steam7������ɱ�־
						break;
					}	
					else																					//���δ���������ݣ����ó�CPU
					{
						delay_us(1);
					}
				}
			}
			
			if(cnt>=times)
			{
				cnt = 0;
			}
			else
			{
				cnt++;
			}
			delay_ms(interval);						//ÿ����ʱɨ����ʱ��
		}
		else
		{
			freq = fl;
			delay_ms(50);
		}
	}
}

