#ifndef __AD9854_H
#define __AD9854_H	 
#include "sys.h"


#define AD9854_RST    PDout(4)   //AD9854��λ�˿�
#define AD9854_UDCLK  PDout(1)   //AD9854����ʱ��
#define AD9854_WR     PDout(3)  //AD9854дʹ�ܣ�����Ч
#define AD9854_RD     PDout(5)   //AD9854��ʹ�ܣ�����Ч
#define AD9854_OSK    PDout(0)   //AD9854 OSK���ƶ�
#define AD9854_FDATA  PDout(2)   //AD9854 FSK,PSK����

#define AD9854_DataBus GPIOE->BSRRL		//AD9854����λ��E�ڵͰ�λ
#define AD9854_AdrBus  GPIOE->BSRRL		//AD9854��ַλ��E�ڸ߰�λ�ĺ���λ

#define uint  unsigned int
#define uchar unsigned char
#define ulong unsigned long

extern void AD9854_Init(void);						  
static void Freq_convert(long Freq);	         	  
extern void AD9854_SetSine(ulong Freq,uint Shape);	  
static void Freq_double_convert(double Freq);		  
extern void AD9854_SetSine_double(double Freq,uint Shape);
extern void AD9854_InitFSK(void);				
extern void AD9854_SetFSK(ulong Freq1,ulong Freq2);					  
extern void AD9854_InitBPSK(void);					  
extern void AD9854_SetBPSK(uint Phase1,uint Phase2);					
extern void AD9854_InitOSK(void);					 
extern void AD9854_SetOSK(uchar RateShape);					  
extern void AD9854_InitAM(void);					 
extern void AD9854_SetAM(uint Shape);					
extern void AD9854_InitRFSK(void);					 
extern void AD9854_SetRFSK(ulong Freq_Low,ulong Freq_High,ulong Freq_Up_Down,ulong FreRate);		

#endif

