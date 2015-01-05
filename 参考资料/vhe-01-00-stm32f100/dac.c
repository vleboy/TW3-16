#include "all.h"
void DAC_GPIO_CON(void)
{

}
void DAC_DMA_CON(void)
{

}
void DAC_INIT_CON(void)
{

}
void alignmentDAC()
{

}

void DAC_INIT(void)
{
	DAC_GPIO_CON();
	DAC_DMA_CON();
	DAC_INIT_CON();
	alignmentDAC();
}

