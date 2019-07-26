#include "sys.h"
#include "usart.h"	
#include "math.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0;       //接收状态标记	
u16 USART_RX_STA1 = 0;      //接收状态标记1	
u16	USART_RX_CNT = 0;				//接收计数器
u16 USART_RX_CNT1 = 0;      //接收计数器1	
u8 HMI_READY = 0;						//HMI串口屏数据透传就绪标志
u8 HMI_REC_LEN = 0;					//HMI串口屏接收指令长度

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

#endif
	
}




void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res,i;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
/////////////////////////接收0XFE,0XFF,0XFF,0XFF后标志置1//////////////////////////
		if(USART_RX_STA == 0XFE)				//接收过0XFE
		{
			if(Res == 0xff)								//当前接收的是0XFF，则计数器自增
			{
				USART_RX_CNT++;
			}
			else													//否则计数器清零
			{
				USART_RX_CNT = 0;
				USART_RX_STA = 0;
			}
			if(USART_RX_CNT == 3)					//连续接收三个0XFF
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
		
/////////////////////////接收指令，开始符0XFC，结束符0XFD//////////////////////////	
		if(USART_RX_STA1 == 0xfc)			//接收过开始符0XFC
		{
			if(USART_RX_CNT1 >= USART_REC_LEN)		//接收指令长度超出范围
			{
				USART_RX_STA1	=	0;	//重新接收
				USART_RX_CNT1 = 0;
			}
			
			if(Res == 0xfd)				//接收到结束符0XFD
			{
				HMI_REC_LEN = USART_RX_CNT1;	//指令接收完毕
//				for(i=0;i<HMI_REC_LEN;i++)
//				{
//					USART_SendData(USART1,USART_RX_BUF[i]);
//					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//				}
				USART1_RXBUFF_HANDLE();		//串口1接收字符数组处理函数
				
				USART_RX_STA1	=	0;	//重新接收
				USART_RX_CNT1 = 0;
			}
			else									//接收指令字符
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
//		if((USART_RX_STA&0x8000)==0)//接收未完成
//		{
//			if(USART_RX_STA&0x4000)//接收到了0x0d
//			{
//				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
//				else USART_RX_STA|=0x8000;	//接收完成了 
//			}
//			else //还没收到0X0D
//			{	
//				if(Res==0x0d)USART_RX_STA|=0x4000;
//				else
//				{
//					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
//					USART_RX_STA++;
//					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
//				}		 
//			}
//		}   		 
  } 
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 

void USART1_RXBUFF_HANDLE(void)
{
	u8 i;
	u8 	del_loc;			//小数点位置					PLL
	u16 int_part;			//频率整数部分（MHz）	PLL
	u8 	del_part;			//频率小数部分（MHz）	PLL
	u8 	del_loc_D;		//小数点位置					DDS
	u16 int_part_D;		//频率整数部分（MHz）	DDS
	u8 	del_part_D;		//频率小数部分（MHz）	DDS
	u16 dacval;				//dac电压（mV）
	float freq = 0;							//ADF4351信号源所需调节频率
	float freq_D = 0;						//AD9851信号源频率
	ulong freq_DDS = 0;
	u8 	PEdb;					//PE4302衰减db
	
	del_loc = 0;
	int_part = 0;
	del_part = 0;
	dacval = 0;
	int_part_D = 0;
	del_part_D = 0;
	PEdb = 0;
	///////////////////////////////接收ADF4351频率控制字///////////////////////////////////////////////////////
	if(USART_RX_BUF[0]==0x0f)				//若首字符为0x0f，则调节信号源频率
	{
		for(i=1;i<HMI_REC_LEN;i++)		//遍历接收数组，找到小数点位置
		{
			if(USART_RX_BUF[i]==0x2E)		//找到小数点
			{
				del_loc = i;							//将当前数组下标赋给del_loc
				break;
			}
			if(i==HMI_REC_LEN-1)				//如果找不到小数点
			{
				del_loc = HMI_REC_LEN;		
			}
		}
		//找到小数点后将整数和小数部分从字符数组转为浮点型
		for(i=1;i<del_loc;i++)				//整数部分
		{
			int_part += (USART_RX_BUF[i] - 0x30)*pow(10.0,(float)(del_loc-i-1));
		}
		if(!del_loc)
		{
			del_part = USART_RX_BUF[del_loc+1] - 0x30;	//小数部分，只取一位小数
		}
		freq = (float)int_part + (float)del_part/10.0;
		//将调节频率送给ADF4351
		ADF4351_Init_some();
		ADF4351WriteFreq(freq);				//设置频率
		
//		USART_SendData(USART1,int_part);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//		USART_SendData(USART1,del_part);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//		USART_SendData(USART1,freq);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}
	///////////////////////////////接收DAC控制字///////////////////////////////////////////////////////
	if(USART_RX_BUF[0]==0x0a)				//若首字符为0x0d，则调节DAC电压
	{
		for(i=1;i<HMI_REC_LEN;i++)		//遍历接收数组，将字符数组转为u16类型
		{
			dacval += (USART_RX_BUF[i] - 0x30)*pow(10.0,(float)(HMI_REC_LEN-i-1));
		}
		Dac1_Set_Vol(dacval);																	//设置DAC输出电压
		
//		USART_SendData(USART1,dacval>>8);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//		USART_SendData(USART1,dacval);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}
	///////////////////////////////接收AD9854频率控制字///////////////////////////////////////////////////////
	if(USART_RX_BUF[0]==0x0d)				//若首字符为0x0d，则调节信号源频率
	{
		for(i=1;i<HMI_REC_LEN;i++)		//遍历接收数组，找到小数点位置
		{
			if(USART_RX_BUF[i]==0x2E)		//找到小数点
			{
				del_loc_D = i;							//将当前数组下标赋给del_loc
				break;
			}
			if(i==HMI_REC_LEN-1)				//如果找不到小数点
			{
				del_loc_D = HMI_REC_LEN;		
			}
		}
		//找到小数点后将整数和小数部分从字符数组转为浮点型
		for(i=1;i<del_loc_D;i++)				//整数部分
		{
			int_part_D += (USART_RX_BUF[i] - 0x30)*pow(10.0,(float)(del_loc_D-i-1));
		}
		if(!del_loc_D)
		{
			del_part_D = USART_RX_BUF[del_loc_D+1] - 0x30;	//小数部分，只取一位小数
		}
		freq_D = (float)int_part_D+ (float)del_part_D/10.0;
		freq_DDS = freq_D*1000.0;
		
//		USART_SendData(USART1,int_part_D);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		//将调节频率送给AD9854
		AD9854_SetSine(freq_DDS,4095);//设置频率和幅值
	}
		///////////////////////////////接收PE4302控制字///////////////////////////////////////////////////////
	if(USART_RX_BUF[0]==0x0E)				//若首字符为0x0E，则调节衰减器增益
	{
		for(i=1;i<HMI_REC_LEN;i++)		//遍历接收数组，将字符数组转为u16类型
		{
			PEdb += (USART_RX_BUF[i] - 0x30)*pow(10.0,(float)(HMI_REC_LEN-i-1));
		}
		PE4302Set(PEdb,1);		//设置PE4302衰减器衰减增益
		
		USART_SendData(USART1,PEdb);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}
}

#endif	

 



