#include "PE43702.h"

void PE_GPIO_Init(void)
{
	/*
	LE: 	C5
	CLK: 	C6
	DATA:	C7
	*/
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	 //使能PE端口时钟

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		 //IO口速度为50MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC
  PE_LE_1;
}
void PE43702Set(unsigned char db)
{
  unsigned char i;
  unsigned char W_DB;
  db = db&0x7f;
  PE_CLK_0;
  PE_LE_0;
  for(i = 0; i < 8;i++)
  {
    W_DB = db&0x01;
    if(W_DB == 1)
    {
      PE_DAT_1;
    }
    else
    {
      PE_DAT_0;
    }
    PE_CLK_1;
		db=db>>1;
    PE_CLK_0;
  }
  PE_LE_1;
}
void PE43702Set2(unsigned char db)
{
  unsigned char i;
  unsigned char W_DB;
  db = db&0x7f;
  PE2_CLK_0;
  PE2_LE_0;
  for(i = 0; i < 8;i++)
  {
    W_DB = db&0x01;
    if(W_DB == 1)
    {
      PE2_DAT_1;
    }
    else
    {
      PE2_DAT_0;
    }
    PE2_CLK_1;
		db=db>>1;
    PE2_CLK_0;
  }
  PE2_LE_1;
}
