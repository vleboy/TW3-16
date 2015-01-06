#include "all.h"

void CONTROL_GPIO_CON(void)
{
	/*DB口为输出*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	/*op.sw口*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure); 

	/*ins.sw口*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*LE口*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	/*led口*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	LED(CLOSE);
	
}

void ATTControl(u8 att)
{
	att = ~att;
	GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)(0x01&att));//B0 PB11
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)(0x01&(att >> 1)));//B1 PB12
	GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)(0x01&(att >> 2)));//B2 PB13
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, (BitAction)(0x01&(att >> 3)));//B3 PB14
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, (BitAction)(0x01&(att >> 4)));//B4 PB15
	GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)(0x01&(att >> 5)));//B5 PA8

	Delayus(20);

	GPIO_SetBits(GPIOB,GPIO_Pin_2);//le PB2 OPEN

	Delayus(20);

	GPIO_ResetBits(GPIOB,GPIO_Pin_2);//le PB2 CLOSE
}
void CONTROL_CON(void)
{
	CONTROL_GPIO_CON();
	ATTControl(63);

	if(OPEN != Config.laserSWflag)
	{
		OP_SW(OP_OFF);	//off
	}
	else
	{
		OP_SW(OP_ON);	//on
	}

	if (Config.AGC == flg_AGC)
	{
		ATTControl(10);
	}

	
	
}
