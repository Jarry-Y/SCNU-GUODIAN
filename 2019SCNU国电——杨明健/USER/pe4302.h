#include "sys.h"

#define pe4302_Data        PAout(4)
#define pe4302_CLK         PAout(5)
#define pe4302_LE1         PAout(6)
#define pe4302_LE2         PAout(7)

#define pe4302_LE1_0          pe4302_LE1=0
#define pe4302_LE1_1          pe4302_LE1=1
#define pe4302_LE2_0          pe4302_LE2=0
#define pe4302_LE2_1          pe4302_LE2=1

#define pe4302_CLK_0         pe4302_CLK=0
#define pe4302_CLK_1         pe4302_CLK=1

#define pe4302_Data_0        pe4302_Data=0
#define pe4302_Data_1        pe4302_Data=1


void PE4302_GPIO_Init(void);
void PE4302Set(unsigned char db,u8 pe4302x);
void PE4302_Init(void);
