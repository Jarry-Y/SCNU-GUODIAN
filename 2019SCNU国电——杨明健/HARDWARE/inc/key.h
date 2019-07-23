#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com									  
////////////////////////////////////////////////////////////////////////////////// 	 

/*下面的方式是通过直接操作库函数方式读取IO*/
#define KEY0 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) //PE4
#define KEY1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//PE3 
//#define KEY2 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) //PE2
#define WK_UP 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//PA0


/*下面方式是通过位带操作方式读取IO*/
/*
#define KEY0 		PEin(4)   	//PE4
#define KEY1 		PEin(3)		//PE3 
#define KEY2 		PEin(2)		//P32
#define WK_UP 	PAin(0)		//PA0
*/

#define L3 		PBout(7)  //PB7
#define L2 		PBout(6)	//PB6 
#define L1 		PBout(5)	//PB5
#define L0 		PBout(4)	//PB4

#define H3 		PBin(3)   //PB3
#define H2 		PBin(2)		//PB2 
#define H1 		PBin(1)		//PB1
#define H0 		PBin(0)		//PB0


#define KEY0_PRES 	1
#define KEY1_PRES	2
//#define KEY2_PRES	3
#define WKUP_PRES   4

void KEY_Init(void);	//IO初始化
int KEY_Scan(void);  		//按键扫描函数	
extern void KEY_task(void *pdata);

#endif
