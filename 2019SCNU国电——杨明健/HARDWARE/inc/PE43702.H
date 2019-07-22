#ifndef _PE43702_H_
#define _PE43702_H_

#include "sys.h"

#define DB_0    ((u8)0X00)
#define DB_0_5  ((u8)0X01)
#define DB_1    ((u8)0X02)
#define DB_2    ((u8)0X04)
#define DB_4    ((u8)0X08)
#define DB_8    ((u8)0X10)
#define DB_16   ((u8)0X20)
#define DB_31_5 ((u8)0X3F)

#define PE2_LE_BIT         	 PAout(5)
#define PE2_CLK_BIT           PAout(6)
#define PE2_DAT_BIT           PAout(7)
#define PE_LE_BIT        PCout(5)
#define PE_CLK_BIT       PCout(6)
#define PE_DAT_BIT       PCout(7)

#define PE_LE_0          PE_LE_BIT=0
#define PE_LE_1        	 PE_LE_BIT=1

#define PE_CLK_0         PE_CLK_BIT=0
#define PE_CLK_1         PE_CLK_BIT=1

#define PE_DAT_0         PE_DAT_BIT=0
#define PE_DAT_1         PE_DAT_BIT=1

#define PE2_LE_0          PE2_LE_BIT=0
#define PE2_LE_1        	PE2_LE_BIT=1

#define PE2_CLK_0         PE2_CLK_BIT=0
#define PE2_CLK_1         PE2_CLK_BIT=1

#define PE2_DAT_0         PE2_DAT_BIT=0
#define PE2_DAT_1         PE2_DAT_BIT=1

void PE_GPIO_Init(void);
void PE43702Set(unsigned char db);

void PE2_GPIO_Init(void);
void PE43702Set2(unsigned char db);
#endif













