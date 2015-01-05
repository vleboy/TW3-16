#include "all.h"
u16 SYSTICK_TIME=0;
void SYSTICK_CON(void)
{
	//SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);			//

	SysTick_Config(SystemCoreClock /1000);
	

}

void Delayms(u32 ms)
{
	SYSTICK_TIME = ms;
	while(SYSTICK_TIME != 0);
}

void Delayus(u32 us)
{
	while(us--);
}
