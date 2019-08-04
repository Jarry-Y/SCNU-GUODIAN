#include "key.h"
#include "includes.h"

//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com							  
////////////////////////////////////////////////////////////////////////////////// 	 

//按键初始化函数
void KEY_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_5|GPIO_Pin_4; //4位写键盘
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB7,6,5,4
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1|GPIO_Pin_0; //4位读键盘
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB3,2,1,0
} 

/*函数名称:KEY_Scan
*	函数介绍:键盘扫描函数
*	输入:无
*	输出:键盘标号
*/
int KEY_Scan(void)
{	 
	u8 KeyLData;	//按键低4位
	u8 j;					//扫描列数
	u8 i;					//扫描行数
	u8 KCC;				//循环扫描数
	int key;
	key = -1;
	GPIOB->ODR = (GPIOB->ODR & 0xff0f) | 0x00f0;	//设置B7-B4为高电平
	delay_us(10);				//给予充足反应时间
	//KeyLData = GPIOB->ODR & 0x000f;	//读B口低4位
	//USART_SendData(USART1,PBin(2));
	KeyLData = H3<<3|0|H1<<1|H0;	//读B口低4位，B2口有问题，无法下拉
	if(KeyLData != 0x00)						//如果有按键按下
	{
		delay_ms(10);									//开始消抖				
		if(KeyLData != 0x00)					//如果仍然有按键按下
		{
			KCC = 0x10;
			for(j=0;j<4;j++)
			{
				GPIOB->ODR = (GPIOB->ODR & 0xff0f) | KCC;	//按键输出引脚依次置高进行循环扫描
				delay_us(10);		//给予充足反应时间
				KeyLData = H3<<3|0|H1<<1|H0;	//读B口低4位，B2口有问题，无法下拉
				if(KeyLData != 0x00)					//找到键值
				{
					switch(KeyLData)						//键值转换
					{
						case 0x01:i = 0; break;
						case 0x02:i = 1; break;
						case 0x04:i = 2; break;
						case 0x08:i = 3; break;
					}
					key = i*10 + j;					//输出键值
					break;									//结束循环
				}
				KCC<<=1;									//KCC左移一位
			}
		}
	}
	return key;
}

//KEY任务
void KEY_task(void *pdata)
{	 
	int key = -1;
	u16 test;
	while(1)
	{
		//键盘扫描
		key = KEY_Scan();
		if(key>=0)
		{
			USART_SendData(USART1,key);
		}	
		delay_ms(60);                     //延时60ms
	}
}