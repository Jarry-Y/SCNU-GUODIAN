#ifndef __HMI_H
#define __HMI_H	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//HMI.H
//SCNU国电2019——杨明健						  
////////////////////////////////////////////////////////////////////////////////// 	 
 							   
void HMI_sendorder(u8 order[],u32 size);	//通过DMA的串口发送一条指令
void HMI_addt(u8 ch);				//数据透传画波形
void HMI_task(void *pdata); //HMI任务
char* Int2String(int num,char *str);//10?? 
#endif 















