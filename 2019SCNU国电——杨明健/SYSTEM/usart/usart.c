#include "sys.h"
#include "usart.h"	
#include "math.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4����ģ��-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0;       //����״̬���	
u16 USART_RX_STA1 = 0;      //����״̬���1	
u16	USART_RX_CNT = 0;				//���ռ�����
u16 USART_RX_CNT1 = 0;      //���ռ�����1	
u8 HMI_READY = 0;						//HMI����������͸��������־
u8 HMI_REC_LEN = 0;					//HMI����������ָ���

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

#endif
	
}




void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res,i;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
/////////////////////////����0XFE,0XFF,0XFF,0XFF���־��1//////////////////////////
		if(USART_RX_STA == 0XFE)				//���չ�0XFE
		{
			if(Res == 0xff)								//��ǰ���յ���0XFF�������������
			{
				USART_RX_CNT++;
			}
			else													//�������������
			{
				USART_RX_CNT = 0;
				USART_RX_STA = 0;
			}
			if(USART_RX_CNT == 3)					//������������0XFF
			{
				USART_RX_CNT = 0;
				USART_RX_STA = 0;
				HMI_READY = 1;
			}
		}
		
		if(Res == 0xfe)
		{
			USART_RX_STA = 0XFE;
		}
		
/////////////////////////����ָ���ʼ��0XFC��������0XFD//////////////////////////	
		if(USART_RX_STA1 == 0xfc)			//���չ���ʼ��0XFC
		{
			if(USART_RX_CNT1 >= USART_REC_LEN)		//����ָ��ȳ�����Χ
			{
				USART_RX_STA1	=	0;	//���½���
				USART_RX_CNT1 = 0;
			}
			
			if(Res == 0xfd)				//���յ�������0XFD
			{
				HMI_REC_LEN = USART_RX_CNT1;	//ָ��������
//				for(i=0;i<HMI_REC_LEN;i++)
//				{
//					USART_SendData(USART1,USART_RX_BUF[i]);
//					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//				}
				USART1_RXBUFF_HANDLE();		//����1�����ַ����鴦����
				
				USART_RX_STA1	=	0;	//���½���
				USART_RX_CNT1 = 0;
			}
			else									//����ָ���ַ�
			{
				USART_RX_BUF[USART_RX_CNT1]	=	Res;
				USART_RX_CNT1++;
			}
		}
		
		if(Res == 0xfc)
		{
			USART_RX_STA1 = 0xfc;
		}
		
///////////////////////////////////////////////////////////////////////////////////
//		if((USART_RX_STA&0x8000)==0)//����δ���
//		{
//			if(USART_RX_STA&0x4000)//���յ���0x0d
//			{
//				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
//				else USART_RX_STA|=0x8000;	//��������� 
//			}
//			else //��û�յ�0X0D
//			{	
//				if(Res==0x0d)USART_RX_STA|=0x4000;
//				else
//				{
//					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
//					USART_RX_STA++;
//					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
//				}		 
//			}
//		}   		 
  } 
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 

void USART1_RXBUFF_HANDLE(void)
{
	u8 i;
	u8 	del_loc;			//С����λ��					PLL
	u16 int_part;			//Ƶ���������֣�MHz��	PLL
	u8 	del_part;			//Ƶ��С�����֣�MHz��	PLL
	u8 	del_loc_D;		//С����λ��					DDS
	u16 int_part_D;		//Ƶ���������֣�MHz��	DDS
	u8 	del_part_D;		//Ƶ��С�����֣�MHz��	DDS
	u16 dacval;				//dac��ѹ��mV��
	float freq = 0;							//ADF4351�ź�Դ�������Ƶ��
	float freq_D = 0;						//AD9851�ź�ԴƵ��
	ulong freq_DDS = 0;
	u8 	PEdb;					//PE4302˥��db
	
	del_loc = 0;
	int_part = 0;
	del_part = 0;
	dacval = 0;
	int_part_D = 0;
	del_part_D = 0;
	PEdb = 0;
	///////////////////////////////����ADF4351Ƶ�ʿ�����///////////////////////////////////////////////////////
	if(USART_RX_BUF[0]==0x0f)				//�����ַ�Ϊ0x0f��������ź�ԴƵ��
	{
		for(i=1;i<HMI_REC_LEN;i++)		//�����������飬�ҵ�С����λ��
		{
			if(USART_RX_BUF[i]==0x2E)		//�ҵ�С����
			{
				del_loc = i;							//����ǰ�����±긳��del_loc
				break;
			}
			if(i==HMI_REC_LEN-1)				//����Ҳ���С����
			{
				del_loc = HMI_REC_LEN;		
			}
		}
		//�ҵ�С�����������С�����ִ��ַ�����תΪ������
		for(i=1;i<del_loc;i++)				//��������
		{
			int_part += (USART_RX_BUF[i] - 0x30)*pow(10.0,(float)(del_loc-i-1));
		}
		if(!del_loc)
		{
			del_part = USART_RX_BUF[del_loc+1] - 0x30;	//С�����֣�ֻȡһλС��
		}
		freq = (float)int_part + (float)del_part/10.0;
		//������Ƶ���͸�ADF4351
		ADF4351_Init_some();
		ADF4351WriteFreq(freq);				//����Ƶ��
		
//		USART_SendData(USART1,int_part);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//		USART_SendData(USART1,del_part);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//		USART_SendData(USART1,freq);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
	///////////////////////////////����DAC������///////////////////////////////////////////////////////
	if(USART_RX_BUF[0]==0x0a)				//�����ַ�Ϊ0x0d�������DAC��ѹ
	{
		for(i=1;i<HMI_REC_LEN;i++)		//�����������飬���ַ�����תΪu16����
		{
			dacval += (USART_RX_BUF[i] - 0x30)*pow(10.0,(float)(HMI_REC_LEN-i-1));
		}
		Dac1_Set_Vol(dacval);																	//����DAC�����ѹ
		
//		USART_SendData(USART1,dacval>>8);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//		USART_SendData(USART1,dacval);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
	///////////////////////////////����AD9854Ƶ�ʿ�����///////////////////////////////////////////////////////
	if(USART_RX_BUF[0]==0x0d)				//�����ַ�Ϊ0x0d��������ź�ԴƵ��
	{
		for(i=1;i<HMI_REC_LEN;i++)		//�����������飬�ҵ�С����λ��
		{
			if(USART_RX_BUF[i]==0x2E)		//�ҵ�С����
			{
				del_loc_D = i;							//����ǰ�����±긳��del_loc
				break;
			}
			if(i==HMI_REC_LEN-1)				//����Ҳ���С����
			{
				del_loc_D = HMI_REC_LEN;		
			}
		}
		//�ҵ�С�����������С�����ִ��ַ�����תΪ������
		for(i=1;i<del_loc_D;i++)				//��������
		{
			int_part_D += (USART_RX_BUF[i] - 0x30)*pow(10.0,(float)(del_loc_D-i-1));
		}
		if(!del_loc_D)
		{
			del_part_D = USART_RX_BUF[del_loc_D+1] - 0x30;	//С�����֣�ֻȡһλС��
		}
		freq_D = (float)int_part_D+ (float)del_part_D/10.0;
		freq_DDS = freq_D*1000.0;
		
//		USART_SendData(USART1,int_part_D);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		//������Ƶ���͸�AD9854
		AD9854_SetSine(freq_DDS,4095);//����Ƶ�ʺͷ�ֵ
	}
		///////////////////////////////����PE4302������///////////////////////////////////////////////////////
	if(USART_RX_BUF[0]==0x0E)				//�����ַ�Ϊ0x0E�������˥��������
	{
		for(i=1;i<HMI_REC_LEN;i++)		//�����������飬���ַ�����תΪu16����
		{
			PEdb += (USART_RX_BUF[i] - 0x30)*pow(10.0,(float)(HMI_REC_LEN-i-1));
		}
		PE4302Set(PEdb,1);		//����PE4302˥����˥������
		
		USART_SendData(USART1,PEdb);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}

#endif	

 



