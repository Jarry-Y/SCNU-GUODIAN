#include "led.h" 
#include "usart.h"	
#include "delay.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com							  
////////////////////////////////////////////////////////////////////////////////// 	 

//初始化PA6和PA7为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟

  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//LED0和LED1对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_SetBits(GPIOC,GPIO_Pin_13);//设置高，灯灭

}

//LED0任务
void led0_task(void *pdata)
{	 
	u8 flag=0;
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);  //LED0对应引脚GPIOC.13拉低，亮  等同LED0=0;
	while(1)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);  //LED0对应引脚GPIOC.13拉低，亮  等同LED0=0;
		delay_ms(500);  
		GPIO_SetBits(GPIOC,GPIO_Pin_13);	   //LED0对应引脚GPIOC.13拉高，灭  等同LED0=1;
		delay_ms(500);                     //延时500ms
	}
}




