#include "all.h"
unsigned short AGC_Task_Time =0; //100MS = 5*20
unsigned short ADCtask_Time = 0; //300ms = 5 * 60
unsigned char WDT_Time = 0;
unsigned char AGColdflg=0;
const unsigned short AGC_RFVol[87] = {
0x0160,
0x0164,
0x0168,
0x016B,
0x016E,
0x0172,
0x0175,
0x017A,
0x017F,
0x0184,
0x0189,
0x018F,
0x0196,
0x019C,
0x01A3,
0x01AA,
0x01B1,
0x01B8,
0x01BF,
0x01C7,
0x01CF,
0x01D7,
0x01E0,
0x01E9,
0x01F1,
0x01FA,
0x0202,
0x020C,
0x0215,
0x0221,
0x022C,
0x0235,
0x023E,
0x0249,
0x0254,
0x025E,
0x0269,
0x0273,
0x027E,
0x0288,
0x0293,
0x029E,
0x02A9,
0x02B4,
0x02BE,
0x02C9,
0x02D5,
0x02E0,
0x02EC,
0x02F3,
0x02FA,
0x0304,
0x030F,
0x031B,
0x0327,
0x0332,
0x033D,
0x0348,
0x0353,
0x035E,
0x036A,
0x0375,
0x0381,
0x038C,
0x0397,
0x03A3,
0x03AE,
0x03B7,
0x03BF,
0x03CD,
0x03DC,
0x03E6,
0x03F1,
0x03FD,
0x0408,
0x0414,
0x041F,
0x0429,
0x0434,
0x043E,
0x0449,
0x0454,
0x045F,
0x0469,
0x0473,

};
//	const unsigned short AGC_RFVol1[43] = {
//0x56 , 0x57 , 0x59 , 0x5a , 0x5c , 	0x5f , 0x61 , 0x64 , 0x66 , 0x6a ,
//0x6d , 0x71 , 0x75 , 0x7a , 0x7f , 	0x84 , 0x88 , 0x8d , 0x92 , 0x97 ,
//0x9d , 0xa2 , 0xa8 , 0xad , 0xb1 , 	0xb6 , 0xbb , 0xc1 , 0xc7 , 0xcc ,
//0xd2 , 0xd8 , 0xde , 0xe3 , 0xe9 , 	0xef , 0xf4 , 0xfa , 0x100, 0x105,
//0x10a, 0x110, 0x115,};


u16 ATT_AGC_old;
u16 ATT_old;
static u16 Att;
void AGC_Task(void)
{


	u16 RFVol;//基准的RF电压值
	u16 RF=0;	
	
	
	if (Config.AGC == flg_AGC)					 //agc
	{
	 	RF = ADC_RUN(ADC_OP);//ADC_Data[0];
	
		if (RF < 0X05)
		{
			Att= ATT_AGC_old;//没有信号,开机没信号则=30,
						//中途没信号则=ATTAGCold
		}
		else 
		{
					
			if (Dataflash.AGC > 86 )
				{
				Dataflash.AGC = 86;
			}
			else if (Dataflash.AGC <= 0 )
				{
				Dataflash.AGC = 0;
			}
		
			RFVol  = AGC_RFVol[Dataflash.AGC];
			
		
			if ( AGColdflg == flg_MGC)	//上次是MGC状态，不比较直接进入AGC状态
			{
						Att = ATT_AGC_old;
			}
			else 	//last time AGC
			{
				if ( RF < (RFVol-4) )		//实际值RF值 小于基准值 
				//if (RFVol - adcRF > RF_OFFSET)
				{
			
					if ( ATT_AGC_old > 0)				//那么减小衰减值
					{
						Att = ATT_AGC_old - 1;
					}
					else
					{
						Att = 0;
					}
				}
				else if ( RF  > (RFVol+4) ) 	//实际RF 大于 基准值 加大衰减量
				{
	
					if ( ATT_AGC_old < 63 )
					{
						Att = ATT_AGC_old + 1;
					}
					else 
					{
						Att = 63;
					}
				}
				else	//等于基准
				{
		
					Att=ATT_AGC_old  ;
				}
			}
		}
		ATT_AGC_old = Att ;
		
		AGColdflg = flg_AGC;
	}
	else		//MGC
	{
		Att = Dataflash.MGC;
		AGColdflg = flg_MGC;
	}
	if (Att != ATT_old)
	{
		ATTControl(Att);
//		LED(CLOSE);
		Delayms(20);
//	LED(OPEN);
//	Delayms(100);
	}
	ATT_old = Att;
	//RFF[5]= Att;
//SendFrame(RFF,6);
}

//

void ADCtask(void)
{
	unsigned char alarmtemp = 0;
	u8 f = 0;
	static unsigned count[4]={0,0,0,0};


	if ( Config.laserSWflag == OPEN)
	{
		if (ADC_RUN(ADC_OP) < LASER_OP_LOW)	//OP
		{
			if (count[0] > 2)
			{
				alarmtemp |= (0x01 << 0);
				count[0] = 3;
				f = 1;
			}
			else count[0]++;
		}
		else 
			{
				count[0]=0;
				alarmtemp &= 0xFE;
			}
	

		if ((ADC_RUN(ADC_IB) < LASER_IA_LOW) || (ADC_RUN(ADC_IB) > LASER_IA_HIGH))//IB
		{	
			if (count[1] > 2)
			{
				alarmtemp |= (0x01 << 1);
				count[1] = 3;
				f = 1;
			}
			else count[1]++;
		}
		else 
			{
				count[1]=0;
				alarmtemp &= 0xFD;
			}
		if ((ADC_RUN(ADC_TEM) < LASER_TEMP_LOW)||(ADC_RUN(ADC_TEM) > LASER_TEMP_HIGH))//TEMP
		{
			if (count[2] > 2)
			{
				alarmtemp |= (0x01 <<2);
				count[2] = 3;
				f=1;
			}
			else count[2]++;
		}
		else
			{
				count[2]=0;
				alarmtemp &= 0xFB;
			}
	}
	else
	{
		count[0]=0;
		count[1]=0;
		count[2]=0;
	}

	if((ADC_RUN(ADC_RF)< LASER_RF_LOW) || (ADC_RUN(ADC_RF) > LASER_RF_HIGH))
	{
		if(count[3]>2)
		{
			alarmtemp |= (0x01 << 3);
			count[3] = 3;
			f = 1;
		}
		else count[3]++;
	}
 	else 
		{
			count[3] = 0;
			alarmtemp &= 0xF7;
		}

	if(f==1)	//有告警
	{
		//alarmtemp |= 0x01;
		
		LED(OPEN);			//open red led

	}
	else		//无告警
	{
	//	alarmtemp &= 0xFE;
	
		LED(CLOSE);
	}
	Dataflash.Err = alarmtemp;
}

	
