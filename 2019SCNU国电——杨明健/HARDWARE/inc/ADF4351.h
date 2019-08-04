#ifndef _ADF4351_H_
#define _ADF4351_H_

#include "sys.h"

#define ADF4351_CLK PCout(0)
#define ADF4351_OUTPUT_DATA PCout(1)
#define ADF4351_LE PCout(2)
#define ADF4351_CE PCout(3)

#define ADF4351_INPUT_DATA PCin(10)

#define ADF4351_RF_OFF	((u32)0XEC801C)

extern u16 interval;			//扫频间隔
extern u8 Adc_data[320];	//ADC采样间隔

void ADF4351Init(void);
void ReadToADF4351(u8 count, u8 *buf);
void WriteToADF4351(u8 count, u8 *buf);
void WriteOneRegToADF4351(u32 Regster);
void ADF4351_Init_some(void);
void ADF4351WriteFreq(float Fre);		//	(xx.x) M Hz
void sweep(float fl,float fh);	//ɨƵ

#endif

