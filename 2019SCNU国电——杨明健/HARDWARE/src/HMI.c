//=====================================================================
//                    HMI�����������������
//HMI.c
//SCNU����2019����������
//=====================================================================

#include "HMI.h"   
#include "dma.h"	
#include "delay.h"	
#include "usart.h"	

//ͨ��DMA�Ĵ��ڷ���һ��ָ��
//order:ָ������
//size:ָ���
void HMI_sendorder(u8 order[], u32 size)
{ 
	u8 i;
	u32 SEND_BUF_SIZE =size+2;	//�������ݳ���,��õ���sizeof(TEXT_TO_SEND)+2��������.
	u8 SendBuff[50];		//�������ݻ�����
	for(i=0;i<SEND_BUF_SIZE;i++)
	{
		if(i<size-1)
		{
			SendBuff[i]=order[i];
		}
		else
		{
			SendBuff[i]=0xff;
		}
	}
	MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)SendBuff,SEND_BUF_SIZE);//DMA2,STEAM7,CH4,����Ϊ����1,�洢��ΪSendBuff,����Ϊ:SEND_BUF_SIZE.
	MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);     //��ʼһ��DMA���䣡	 
}


//����͸��������
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

//HMI����
void HMI_task(void *pdata)
{
  u8 order[]={"add 1,0,150"};	 
	int size=sizeof(order);	//ָ���
	u8 order1[]={"add 1,0,200"};	 
	int size1=sizeof(order);	//ָ���
	u8 flag=0;
	
	#define SEND_BUF_SIZE 320
	int i;
	u8 data[SEND_BUF_SIZE];
	for(i=0;i<SEND_BUF_SIZE;i++)
	{
		data[i]=30;
	}
	
	while(1)
	{
//		//DMA���ڷ���һ��ָ��
//		if(flag)
//		{
//			HMI_sendorder(order,size);
//			flag=0;
//		}
//		else
//		{
//			HMI_sendorder(order1,size1);
//			flag=1;
//		}
//		while(1)
//		{
//			if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//�ȴ�DMA2_Steam7�������
//			{ 
//				DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);	//���DMA2_Steam7������ɱ�־
//				break;
//			}	
//			else																					//���δ���������ݣ����ó�CPU
//			{
//				delay_ms(1);
//			}
//		}
//		delay_ms(1);
		
		//����͸��
		HMI_addt(0);
		while(1)
		{
			if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//�ȴ�DMA2_Steam7�������
			{ 
				DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);	//���DMA2_Steam7������ɱ�־
				break;
			}	
			else																					//���δ���������ݣ����ó�CPU
			{
				delay_ms(1);
			}
		}
		while(1)																				//�ȴ�����������͸������
		{
			if(HMI_READY != 1)
			{
				delay_ms(5);
			}
			else
			{
				HMI_READY = 0;
				break;
			}
		}
		MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)data,SEND_BUF_SIZE);//DMA2,STEAM7,CH4,����Ϊ����1,�洢��ΪSendBuff,����Ϊ:SEND_BUF_SIZE.
		MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);     //��ʼһ��DMA���䣡	 
		while(1)
		{
			if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//�ȴ�DMA2_Steam7�������
			{ 
				DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);	//���DMA2_Steam7������ɱ�־
				break;
			}	
			else																					//���δ���������ݣ����ó�CPU
			{
				delay_ms(1);
			}
		}
		delay_ms(100);
	}
}
