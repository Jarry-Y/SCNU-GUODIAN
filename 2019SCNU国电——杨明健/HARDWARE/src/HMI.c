//=====================================================================
//                    HMI串口屏驱动程序设计
//HMI.c
//SCNU国电2019——杨明健
//=====================================================================

#include "HMI.h"   
#include "dma.h"	
#include "delay.h"	
#include "includes.h"	
#include "stdlib.h"
#include "stdio.h"

//通过DMA的串口发送一条指令
//order:指令内容
//size:指令长度
void HMI_sendorder(u8 order[], u32 size)
{ 
	u8 i;
	u32 SEND_BUF_SIZE =size+3;	//发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.
	u8 SendBuff[50];		//发送数据缓冲区
	for(i=0;i<SEND_BUF_SIZE;i++)
	{
		if(i<size)
		{
			SendBuff[i]=order[i];
		}
		else
		{
			SendBuff[i]=0xff;
		}
	}
	MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)SendBuff,SEND_BUF_SIZE);//DMA2,STEAM7,CH4,外设为串口1,存储器为SendBuff,长度为:SEND_BUF_SIZE.
	MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);     //开始一次DMA传输！	 
}


//数据透传画波形
void HMI_addt(u8 ch)
{
	u8 order0[]={"addt 1,0,320"};
	u8 order1[]={"addt 1,1,320"};
	u8 order2[]={"addt 1,2,320"};
	u8 order3[]={"addt 1,3,320"};
	u8 size=sizeof(order0);
	int i;
	switch(ch)
	{
		case 0:HMI_sendorder(order0,size);break;
		case 1:HMI_sendorder(order1,size);break;
		case 2:HMI_sendorder(order2,size);break;
		case 3:HMI_sendorder(order3,size);break;
	}
}

//HMI任务
void HMI_task(void *pdata)
{
  u8 order[]={"add 1,0,150"};	 
	int size=sizeof(order);	//指令长度
	u8 order1[]={"add 1,0,200"};	 
	int size1=sizeof(order);	//指令长度
	u8 flag=0;
	
	#define SEND_BUF_SIZE 320
	int i;
	u8 data[SEND_BUF_SIZE];
	
	char* cmd="add 1,0,";
	u8 buf[3] = {0}; 
	u8 cnt = 0;							//计数器
	char* temp="";

	
	for(i=0;i<SEND_BUF_SIZE;i++)
	{
		data[i]=30;
	}

	while(1)
	{
		//DMA串口发送一条指令
		if(HMI_ADDT_ORDER ==1)
		{
//			if(flag)
//			{
//				HMI_sendorder(order,size);
//				flag=0;
//			}
//			else
//			{
//				HMI_sendorder(order1,size1);
//				flag=1;
//			}
			//Int2String(Adc_data[cnt++],temp);
			//u8转字符数组
			buf[2]=Adc_data[cnt++]/100 + 0x30;
			buf[1]=Adc_data[cnt]/10 - Adc_data[cnt]/100*10 +0x30;
			buf[0]=Adc_data[cnt]%10	+ 0x30;
			if(cnt>=200)
			{
				cnt=0;
			}
			//将字符指针转为字符数组
//			for(i=2;i>=0;i--)
//			{
//				USART_SendData(USART1,buf[1]);
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//			}
				USART_SendData(USART1,Adc_data[cnt]);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			
//			while(1)
//			{
//				if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//等待DMA2_Steam7传输完成
//				{ 
//					DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);	//清除DMA2_Steam7传输完成标志
//					break;
//				}	
//				else																					//如果未传输完数据，则让出CPU
//				{
//					delay_us(100);
//				}
//			}
			delay_us(100);
		}
		else
		{
			delay_ms(100);
		}
		
//		//数据透传
//		if(HMI_ADDT_ORDER==1)
//		{
//			delay_ms(20);
//			printf("addt 1,0,320");
////			while(1)
////			{
////				if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//等待DMA2_Steam7传输完成
////				{ 
////					DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);	//清除DMA2_Steam7传输完成标志
////					break;
////				}	
////				else																					//如果未传输完数据，则让出CPU
////				{
////					delay_ms(20);
////				}
////			}
//			while(1)																				//等待串口屏数据透传就绪
//			{
//				if(HMI_READY != 1)
//				{
//					delay_ms(20);
//				}
//				else
//				{
//					HMI_READY = 0;
//					break;
//				}
//			}
//			MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)data,SEND_BUF_SIZE);//DMA2,STEAM7,CH4,外设为串口1,存储器为SendBuff,长度为:SEND_BUF_SIZE.
//			MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);     //开始一次DMA传输！	 
//			while(1)
//			{
//				if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//等待DMA2_Steam7传输完成
//				{ 
//					DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);	//清除DMA2_Steam7传输完成标志
//					break;
//				}	
//				else																					//如果未传输完数据，则让出CPU
//				{
//					delay_ms(20);
//				}
//			}
//		}
//		delay_ms(100);
	}
}

