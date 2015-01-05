#include "all.h"

unsigned short OPr[10][2]={{0xE8C,36},{0x9B2,32},{0x4D9,28},{0x3D5,24},{0x34C,20},{0x30E,16},{0x26D,12},{0x1F0,8},{0x188,4},{0x136,0}};//H-L
signed short ABdoor[10][2]={{-6,0x136},{-5,0x0},{-4,0x0},{-3,0x0},{-2,0x0},{-1,0x0},{0,0x0},{1,0x0},{2,0x0},{3,0x0}};
void AdcTask ( void )
{
    NVIC_EnableIRQ ( ADC_IRQn );
}

signed char ATTval(unsigned short agc,signed char agctemp)
{
		signed char ATT_Temp=0;
	  unsigned char ATT = 0;
    unsigned char index = 0;
		if (  agc < 0x7c )//小于-10dbm
        {
           ATT =63;
        }

    else
        {
            for ( index = 0; index < 10; index++ )
                {
                    if (   agc >= OPr[index][0] )
                        {
                           ATT = OPr[index][1];
                            break;
                        }
                }
        }
		ATT_Temp = ATT - agctemp;
    if ( ATT_Temp < 0 )
        {
            ATT = 0;
        }

    else if ( ATT_Temp >= 63 )
        {
            ATT = 63;
        }

    else
        {
            ATT = ATT_Temp;
        }
    return ATT;
}
void OPR_AGC_Task(void)
{
   
    unsigned char ATT = 0,CC=0;
   
    static unsigned char ATT_OLDa = 1,ATT_OLDb = 1;
ATT =ATTval(MainStructR.ORAop,MainStructW.AGCa);
CC = CelsiusComp();
if(ATT >= CC)
(ATT = ATT - CC);
else
(ATT = 0);
    if ( ATT_OLDa !=  ATT)
        {
     
						PE4304Send(1,ATT);
            ATT_OLDa = ATT;
            
        }
			ATT=	ATTval(MainStructR.ORBop,MainStructW.AGCb);
			CC = CelsiusComp();
			if(ATT >= CC)
			(ATT = ATT - CC);
			else
			(ATT = 0);
		if ( ATT_OLDb != ATT )
        {
     
						PE4304Send(2,ATT);
            ATT_OLDb = ATT;
            
        }
}
void WarningTask()
{
	 static unsigned Count[4] = {0, 0, 0, 0};
   unsigned char  f = 0;
   unsigned char alarmtemp = 0;
	if(ADCcovVal[0] < 0x136 || ADCcovVal[0] > 0x9b2)//接收光功率A报警 -6db ~ +3db (0x136~0x9b2)
	{
		if ( Count[0] > 2 )
                {
                    alarmtemp |= ( 0x01 << 0 );
                    Count[0] = 3;
                    f = 1;
                }

            else
                {
                    Count[0]++;
										alarmtemp &= ~( 0x01 << 0 );
                }
	}
	if(ADCcovVal[3] < 0x136 || ADCcovVal[3] > 0x9b2)//接收光功率A报警 -6db ~ +3db (0x136~0x9b2)
	{
		if ( Count[0] > 2 )
                {
                    alarmtemp |= ( 0x01 << 1 );
                    Count[0] = 3;
                    f = 1;
                }

            else
                {
                    Count[0]++;
										alarmtemp &= ~( 0x01 << 1 );
                }
	}
	if(ADCcovVal[2] < 0x7c || ADCcovVal[2] > 0xc1f)//&& (P02==TxA))//发射光功率0x7c = -10db,0xc1f = 4db
	{
		
				if ( Count[0] > 2 )
                {
                    alarmtemp |= ( 0x01 << 2 );
                    Count[0] = 3;
                    f = 1;
                }

            else
                {
                    Count[0]++;
										alarmtemp &= ~( 0x01 << 2 );
                }
		}
	if(ADCcovVal[9] < 0x7c || ADCcovVal[9] > 0xc1f)//&& (P02==TxB))//发射光功率0x7c = -10db,0xc1f = 4db
	{
		
				if ( Count[0] > 2 )
                {
                    alarmtemp |= ( 0x01 << 3 );
                    Count[0] = 3;
                    f = 1;
                }

            else
                {
                    Count[0]++;
										alarmtemp &= ~( 0x01 << 3 );
                }
		}
	if(ADCcovVal[1] < 0x174 || ADCcovVal[1] > 0x6ca)//&& (P03==IbA))//激光器电流 15ma - 70ma (0x174)-(0x6ca)
	{
		
				if ( Count[0] > 2 )
                {
                    alarmtemp |= ( 0x01 << 4 );
                    Count[0] = 3;
                    f = 1;
                }

            else
                {
                    Count[0]++;
										alarmtemp &= ~( 0x01 << 4 );
                }
		}
	if(ADCcovVal[8] < 0x174 || ADCcovVal[8] > 0x6ca)//&& (P03==IbB))//激光器电流 15ma - 70ma
	{
				if ( Count[0] > 2 )
                {
                    alarmtemp |= ( 0x01 << 5 );
                    Count[0] = 3;
                    f = 1;
                }

            else
                {
                    Count[0]++;
										alarmtemp &= ~(0x01 << 5);
                }
		}
	if(ADCcovVal[7] < 0x136 || ADCcovVal[7] > 0x9b2)//输出电平范围为 80dBuV ~ 116dBuV
	{
		if ( Count[0] > 2 )
                {
                    alarmtemp |= ( 0x01 << 6 );
                    Count[0] = 3;
                    f = 1;
                }

            else
                {
                    Count[0]++;
										alarmtemp &= ~(0x01 << 6);
                }
		
	}
	
	if ( f != 1 )	//有告警
        {
            //			LED(CLOSE);
        }
    else		//无告警
        {
            //	LED(OPEN);			//open red led
        }

    MainStructR.Warning = alarmtemp;
	
	}

void CheckMode(void)
{

	MainStructR.online = P42+(P36<<1)+(P41<<2)+(P04<<3);//bit0 - 接收模块A, bit1 - 接收模块B , bit2 - 发射模块A , bit3 - 发射模块B
	
	MainStructR.Box = 	P05;
	
	TriSwitchTask();
	

	
}

unsigned char CelsiusComp(void)
{
	unsigned char x;
	ReadCelsius();
	MainStructR.Ce >= 125 ?(x=10):(
	MainStructR.Ce >= 115 ?(x=9):(
	MainStructR.Ce >= 105 ?(x=8):(
	MainStructR.Ce >= 95 ?(x=7):(
	MainStructR.Ce >= 85 ?(x=6):(
	MainStructR.Ce >= 75 ?(x=5):(
	MainStructR.Ce >= 65 ?(x=4):(
	MainStructR.Ce >= 55 ?(x=3):(
	MainStructR.Ce >= 45 ?(x=2):(
	MainStructR.Ce >= 35 ?(x=1):(
	x=0))))))))));
	return x;
}
/*三态门*/
void TriSwitchTask(void)
{
	switch (MainStructW.Port12)
	{
		case 0x01:
		{
			TriSwitchTo(A_LL);
		}
		break;
		case 0x02:
		{
	TriSwitchTo(A_HH);
		}
		break;
		case 0x03:
		{
	TriSwitchTo(A_HL);
		}
		break;
	}
	switch (MainStructW.Port34)
	{
		case 0x01:
		{
			TriSwitchTo(B_LL);
		}
		break;
		case 0x02:
		{
	TriSwitchTo(B_HH);
		}
		break;
		case 0x03:
		{
	TriSwitchTo(B_HL);
		}
		break;
	}
	
SetA();

}

/*AB二切一开关任务*/
void ABSwitchTask(void)
{
	switch (MainStructW.AB_Switch)
	{
			case 0x01: //强制A
			{
				SetA();
			}
			break;
			case 0x02: //强制B
			{
				SetB();
			}
			break;
			case 0x03: //A优先
			{
				if(MainStructR.ORAop > ABdoor[MainStructW.AB_Switch_door+6][0])
				{
					SetA();
				}
				else
				{
					SetB();
				}
			}
			break;
			case 0x04: //B优先
			{
				if(MainStructR.ORBop > ABdoor[MainStructW.AB_Switch_door+6][0])
				{
					SetB();
				}
				else
				{
					SetA();
				}
			}
			break;
	}
}
////MainStructR.ORAop = ADCcovVal[0];

////				 MainStructR.online = 0x0f;
////				 MainStructR.AC60 = 60;
////				 MainStructR.DC24 = 240;
////				 MainStructR.DC5 = 50;
////				 MainStructR.Temp = 33;
////				 MainStructR.ORAop = 1;
////				 MainStructR.ORBop = 1;
////				 MainStructR.OTAop = 30;
////				 MainStructR.OTBop = 30;
////				 MainStructR.OTAim = 20;
////				 MainStructR.OTBim = 20;
//////				 MainStructR.RF = 100;
////				 MainStructR.Box = 0x01;
////				 MainStructR.Warning = 0;
//}

//void DataTaskCover(unsigned int data)
//{
//	 float fdata;
//	 data = data	*33/4096;
//	 
//	 
//	
//}
