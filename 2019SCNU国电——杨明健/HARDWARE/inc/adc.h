#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4����ģ��-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com								  
////////////////////////////////////////////////////////////////////////////////// 	 
 							   
void Adc_Init(void); 				//ADCͨ����ʼ��
u16  Get_Adc3(u8 ch); 				//���ĳ��ͨ��ֵ 
void ADC_task(void *pdata);	//ADC����
u16	 Get_Adc3_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
u16  Get_Adc1(u8 ch); 				//���ĳ��ͨ��ֵ 
u16  Get_Adc1_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
u16  Get_Adc2(u8 ch); 				//���ĳ��ͨ��ֵ 
u16  Get_Adc2_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
#endif 















