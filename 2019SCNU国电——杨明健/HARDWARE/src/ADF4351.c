	/**********************************************************
                       康威电子
										 
功能：ADF4351正弦波点频输出，范围35M-4400M，步进0.1M，stm32f103rct6控制
			显示：12864
接口：CLK-PC11 DATA-PC10 LE-PC9 CE-PC12  按键接口请参照key.h
时间：2015/11/3
版本：1.0
作者：康威电子
其他：程序借鉴正点原子，添加自己的驱动，未经作者许可，不得用于其它任何用途

更多电子需求，请到淘宝店，康威电子竭诚为您服务 ^_^
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

u16 interval=0;			//扫频间隔
u8 Adc_data[320];		//ADC采集回的数据

void ADF_Output_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}

void ADF_Input_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
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
	
	multiple = (Fre*N_Mul)/25;		//带宽出已固定为参考25M，若改参考频率，则应修改R4低字节
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

/***********扫频************/
//步进100KHz
//fl:扫频最低频率	(xx.x MHz)
//fh:扫谱最高频率	(xx.x MHz)
//time:扫频时间		(s) 一般为1~5
void sweep(float fl,float fh)				
{
	float bw	=	fh-fl;	//扫频带宽
	u16 times = bw*10;	//扫频次数
	float freq = fh;		//输出频率
	//AD采集
	u16 adcx;
	float temp;
	u16 del_part;	//小数部分
	u16 cnt=0;		//接收数据计数
	int i;
	//HMI发数据
	u8 cmd[]="add 1,0,";
	u8 buf[3] = {0}; 
	u8 order[11];
	
	while(1)
	{
		interval = PLL_SWEEP_TIME*1000/times;
		if(PLL_SWEEP_ENABLE)
		{
			ADF4351_Init_some();
			ADF4351WriteFreq(freq);				//设置频率
			if(freq<=fl)									//如果目前频率小于等于最低频率，则重新扫频
			{
				freq = fh;
			}
			else
			{
				freq -= 0.1;								//步进100KHz
			}
			
			//AD采样
			adcx=Get_Adc2_Average(ADC_Channel_14,1);	//获取通道5的转换值，20次取平均
			temp=(float)adcx*(3.3/4096);          	//获取计算后的带小数的实际电压值，比如3.1111
			Adc_data[cnt]=temp*255.0/3.3;
//			USART_SendData(USART1,Adc_data[cnt]);
//			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			
			//HMI发数据
			if(HMI_ADDT_ORDER ==1)
			{
				//u8转字符数组
				buf[2]=Adc_data[cnt]/100 + 0x30;
				buf[1]=Adc_data[cnt]/10 - Adc_data[cnt]/100*10 +0x30;
				buf[0]=Adc_data[cnt]%10	+ 0x30;
//				for(i=2;i>=0;i--)
//				{
//					USART_SendData(USART1,buf[i]);
//					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//				}
//				USART_SendData(USART1,buf[0]);
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
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
//					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
				}
				
				HMI_sendorder(order,sizeof(order));	
				while(1)
				{
					if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//等待DMA2_Steam7传输完成
					{ 
						DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);	//清除DMA2_Steam7传输完成标志
						break;
					}	
					else																					//如果未传输完数据，则让出CPU
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
			delay_ms(interval);						//每次延时扫描间隔时间
		}
		else
		{
			freq = fl;
			delay_ms(50);
		}
	}
}

