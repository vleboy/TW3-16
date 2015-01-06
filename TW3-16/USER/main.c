/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/

#include "all.h"
GPIO_InitTypeDef  GPIO_InitStructure; 
unsigned char indexmk = 0;
MainWStruct MainStructW;
MainRStruct MainStructR;
AsciiStruct MainAscii;
unsigned char PartNumber[16] = {'T','W','3','-','1','6',0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
unsigned char LogicID[16] = {'T','W','3','-','1','6'};
unsigned char VendorName[16] = {'D','O','N','Y','A','N',0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
unsigned char SerialNumber[16]={0};
/*

*/
void maintask(void)
{

//			if (OtherTimeTask >= OtherTimeTaskFlg)
//		{
//				OtherTimeTask = 0;
//			CheckMode();
//		}
//    if ( AdcTimeTask >= AdcTimeTaskFlg )
//        {

//            AdcTimeTask = 0;
//            AdcTask();
//				}
//		if (OPrTimeTask >= OPrAgcTaskFlg)
//			{
//				OPrTimeTask = 0;
//				OPR_AGC_Task();
//			}




	
}
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  System_Init();
  /* Infinite loop */
TxopSwitchTo(TxB);
IbSwitchTo(IbB);
    while ( 1 )
        {
					// P01=1,P00=1;
            maintask();
         //   UartFramePro ( &UartRcvFrame );

        }
}


void System_Init(void)
{
	RCC_CON();//配置时钟
	SYSTICK_CON();
  ReadInitPara();
	CONTROL_CON();
//	UART_CON();
//	ADC_INIT();//配置并开启ADC
	
	
}

void RCC_CON(void)
{
	RCC_DeInit();					//默认值
  RCC_HSICmd(ENABLE); // internal HSI RC (8 MHz)使能内部时钟HSI
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);//设置系统时钟为HSI

  RCC_HSEConfig(RCC_HSE_OFF);						//关闭HSE
  RCC_LSEConfig(RCC_LSE_OFF);						//关闭LSE
  RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_6); // 24HMzMIX 设置锁相环
  RCC_PLLCmd(ENABLE);										//使能PLL
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);			//等待PLL标志置位
  
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);		//打开FLASH缓冲
  FLASH_SetLatency(FLASH_Latency_0); 										//设置FLASH缓冲等级为0 0-24M
	
	RCC_HCLKConfig(RCC_SYSCLK_Div1); 											//配置HCLK时钟为系统时钟1分频
  RCC_PCLK2Config(RCC_HCLK_Div1);												//配置高速外设HCLK时钟1分频
  RCC_PCLK1Config(RCC_HCLK_Div2);												//配置低速外设HCLK时钟2分频
   
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);						//配置系统时钟为PLL 24MHz

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
         | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
         | RCC_APB2Periph_AFIO, ENABLE);								//使能外设IO口ABCD,AFIO
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能外设串口1

}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
