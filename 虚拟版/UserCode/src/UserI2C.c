#include "all.h"
/*-----------------------------------------------------------------*/
/* ��ʼ��I2C                                                       */
/*-----------------------------------------------------------------*/


void CopyTo ( unsigned char *sour, unsigned char *tar, unsigned char len )
{
    unsigned char i = 0;

    for ( i = 0; i < len; i++ )
        {*tar++ = *sour++;}
}
void ClearDim ( unsigned char *cl, unsigned char len )
{
    unsigned char i = 0;

    for ( i = 0; i < len; i++ )
        {
            *cl++ = 0;
        }
}
unsigned short ADC_CH ( unsigned short adc )
{
    unsigned long x;
    x = adc * 10;
    x = x * 33;
    x = x / 4096;
    return ( unsigned short ) x;
}



/*-----------------------------------------------------------------*/
/*  I2C0 �ж���� (������startup_M051Series.s����)                 */
/*-----------------------------------------------------------------*/
void I2C0_IRQHandler ( void )
{
   
}
/*************************************************************
*�������ƣ�void I2cFramePro(I2CRcvFrame *frame)
*�������ܣ�����֡����
*��    ��������֡
*��    �أ�null
*************************************************************/
void I2cFramePro ( void )
{
    unsigned char MODUDATACS = 0;
    ModuLenNumStruct *p_modu;
    unsigned char i = 0;
    p_modu = &ModuTemp;

    /*����Ƿ�����������*/

    if ( I2Cuse[0] == 0x55 && I2Cuse[2] == 0x04 && I2Cuse[5] == 0xaa )
        {
            I2Csend[0] = 0x55;
            I2Csend[1] = 0x00;
            I2Csend[2] = 0x06;
            I2Csend[3] = 0x40;
            I2Csend[4] = 0x13;
            I2Csend[5] = 0xaa;
            ClearDim ( I2Cuse, sizeof ( I2Cuse ) / sizeof ( char ) );
            return;
        }

    if ( I2Cuse[0] == 0x55 && I2Cuse[5] == 0xAA && I2Cuse[2] == 0x00 )
        {
            SYS_UnlockReg();
            FMC->ISPCON = FMC_ISPCON_BS_LDROM;
            _SYS_RESET_CPU();//��LD ����
        }

    if ( I2Cuse[0] != FRAME_HEAD1 || I2Cuse[I2Cuse[2] - 1] != FRAME_REAR ) //��ͷ����β
        {
            g_I2Cupdate = 0;
            return;
        }

    I2Cchkxor = 0;

    for ( i = 0; i < I2Cuse[2] - 2; i++ )			//cs Ϊ��ͷ��CSǰ�����У��
        {
            I2Cchkxor ^= I2Cuse[i];
        }

    if ( I2Cchkxor != I2Cuse[I2Cuse[2] - 2] )
        {
            I2Cchkxor = 0;
            return;
        }

    I2Cchksum = 0;

    for ( i = 0; i < I2Cuse[2] - 2; i++ )			//cs Ϊ��ͷ��CSǰ�����У��
        {
            I2Cchksum += I2Cuse[i];
        }

    if ( I2Cuse[2] != 0x05 )			//����ģ������
        {
            I2Cuse[I2Cuse[2] - 2] = I2Cchksum;
            RS485_Send ( I2Cuse, I2Cuse[2] );
            DelayMs ( 10 );
            Inquire ( I2Cuse[1] );
            DelayMs ( 10 );
            ClearDim ( I2Cuse, ( sizeof ( I2Cuse ) / sizeof ( char ) ) );
        }

    else if ( I2Cuse[2]  == 5 ) //ѡ��ģ��(0-10)
        {
            I2Cchoosed_M = I2Cuse[1];
            *p_modu = I2cSwitchModu ( I2Cchoosed_M );

            if ( I2Cchoosed_M != 0x00 )
                {
                    I2Csend[0] = 0xf5;
                    I2Csend[1] = I2Cchoosed_M;
                    I2Csend[2] = 6 + p_modu ->DataLen;
                    I2Csend[3] = p_modu->ModuType;

                    for ( i = 0; i < p_modu->DataLen; i++ )
                        {
                            I2Csend[4 + i] = p_modu->ModuData[i];
                        }

                    I2Csend[4 + p_modu->DataLen] = 0;

                    for ( i = 0; i < 4 + p_modu->DataLen; i++ )
                        {
                            MODUDATACS ^= I2Csend[i];
                        }

                    I2Csend[4 + p_modu->DataLen] = MODUDATACS;
                    I2Csend[5 + p_modu->DataLen] = 0XAA;
                }

            else
                {
                    I2Csend[0] = 0xf5; //��ģ��
                    I2Csend[1] = 0x00;
                    I2Csend[2] = 11;
                    MainModu.POWERA = ADC_CH ( FourDataAdc ( ADCtempA ) );
                    MainModu.POWERB = ADC_CH ( FourDataAdc ( ADCtempB ) );
                    I2Csend[5] = MainModu.POWERA >> 8;
                    I2Csend[6] = MainModu.POWERA;
                    I2Csend[3] = MainModu.POWERB >> 8;
                    I2Csend[4] = MainModu.POWERB;
                    I2Csend[7] = MainModu.Inserted >> 8;
                    I2Csend[8] = MainModu.Inserted;
                    I2Csend[9] = 0;

                    for ( i = 0; i < 9; i++ )
                        {
                            I2Csend[9] ^= I2Csend[i];
                        }

                    I2Csend[10] = 0xAA;
                }

            ClearDim ( I2Cuse, ( sizeof ( I2Cuse ) / sizeof ( char ) ) );
        }

    else
        {
            ClearDim ( I2Cuse, ( sizeof ( I2Cuse ) / sizeof ( char ) ) );
        }
}
