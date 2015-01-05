 #include "all.h"

unsigned char indexmk = 0;
MainWStruct MainStructW;
MainRStruct MainStructR;
AsciiStruct MainAscii;
unsigned char PartNumber[16] = {'T','W','3','-','1','6',0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
unsigned char LogicID[16] = {'T','W','3','-','1','6'};
unsigned char VendorName[16] = {'D','O','N','Y','A','N',0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
unsigned char SerialNumber[16]={0};
	

void System_Init ( void )
{
	
    __set_PRIMASK ( 1 );	//�ر����ж�
    SYS_Init();
    IO_Init();
    FMC_Init();
    // WDT_Init();
		ReadInitPara();
    UART1_Init();
    ADC_Init();
    TMR0_Init();
    __set_PRIMASK ( 0 );
}
///*-----------------------------------------------------------------*/
///*WDT                                                   */
///*-----------------------------------------------------------------*/
//void WDT_Init(void)
//{

//    SYS_UnlockReg();


//    WDT->WTCR = WDT_WTCR_COMMON(7)
//                | WDT_WTCR_WTRE_Msk;

//    SYS_LockReg();
//}


//void maintask ( void )
//{
//    static unsigned char checknum = 0xff, checked = 0;
//    ModuLenNumStruct *p_modu;
//    p_modu = &ModuTemp;

//    if ( AdcTimeTask >= AdcTimeTaskFlg )
//        {
//            ModuCheckFlg += 1;
//            AdcTimeTask = 0;
//            AdcTask();
//#ifdef vhe01

//            if ( P40 )
//                {
//                    P40 = 0;
//                }

//            else
//                {
//                    P40 = 1;
//                }

//#else

//            if ( P15 )
//                {
//                    P15 = 0;
//                }

//            else
//                {
//                    P15 = 1;
//                }

//#endif
//        }

//    if ( Uart1TimeTask >= Uart1TimeTaskFlg )
//        {
//            Uart1TimeTask = 0;

//            if ( indexmk > 0x0a )
//                {
//                    indexmk = 0x01;
//                }

//            Inquire ( indexmk );
//            *p_modu = I2cSwitchModu ( indexmk );

//            if ( UartRcvFrame.len == 0 && p_modu->Online == 1 ) //ʧ��
//                {
//                    checknum = indexmk;
//                    checked++;

//                    if ( checked > 3 )
//                        {
//                            p_modu->Online = 0;
//                            MainModu.Inserted &= ~ ( 0x01 << ( indexmk - 1 ) );
//                            checked = 0;
//                            checknum = 0xff;
//                            indexmk++;
//                        }
//                }

//            else
//                {
//                    if ( checknum == indexmk )
//                        {
//                            checked = 0;
//                        }

//                    indexmk++;
//                }
//        }

//    if ( I2cTimeTask >= I2cTimeTaskFlg )
//        {
//            _SYS_RESET_CPU();
//            I2cTimeTask = 0;
//        }
//}

int32_t main ( void )
{
    System_Init();
 //   _ADC_START_CONVERT();                 // Start ADC convert

    while ( 1 )
        {
 //           maintask();
            UartFramePro ( &UartRcvFrame );

        }
}
