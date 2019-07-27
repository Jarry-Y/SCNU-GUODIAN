#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com								  
////////////////////////////////////////////////////////////////////////////////// 	 
 							   
void Adc_Init(void); 				//ADC通道初始化
u16  Get_Adc3(u8 ch); 				//获得某个通道值 
void ADC_task(void *pdata);	//ADC任务
u16	 Get_Adc3_Average(u8 ch,u8 times);//得到某个通道给定次数采样的平均值  
u16  Get_Adc1(u8 ch); 				//获得某个通道值 
u16  Get_Adc1_Average(u8 ch,u8 times);//得到某个通道给定次数采样的平均值  
u16  Get_Adc2(u8 ch); 				//获得某个通道值 
u16  Get_Adc2_Average(u8 ch,u8 times);//得到某个通道给定次数采样的平均值  
#endif 















