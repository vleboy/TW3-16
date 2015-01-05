#include "all.h"
/*-----------------------------------------------------------------*/
/* ��ʼ��I2C                                                       */
/*-----------------------------------------------------------------*/

unsigned char I2Crecv[50] = {0};
unsigned char I2Cuse[50] = {0};
unsigned char I2Csend[50] = {0};
unsigned char I2C_recv_flg = FALSE;
uint32_t g_I2Cupdate = 0;
uint32_t i2clen = 3;
I2CRcvStruct I2CRcvFrame;
uint32_t I2CTimeDelay;
uint32_t I2Cchanged = 0;
uint32_t I2Cchoosed_M = 0xff;
uint32_t I2Cchkxor = 0;
uint32_t I2Cchksum = 0;
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


void I2C_Init ( void )
{
    /* ʹ��I2C0������ */
    I2C0->I2CON |= I2C_I2CON_ENS1_Msk;
    /* I2C0ʱ�ӳ�Ƶ, I2C����Ƶ�� = PCLK / (4*5) = 1.11MHz */
    //  I2C0->I2CLK = I2C_I2CLK_DIV4(5);
    /* ����I2C0��4���ӻ���ַ�͵�ַ���� */
    I2C0->I2CADDR0 = 0x01 << I2C_I2CADDR_I2CADDR_Pos;
    I2C0->I2CADM0 = 0x00 << I2C_I2CADM_I2CADM_Pos;
    /* ʹ��I2C0�жϣ����趨�жϿ�����(NVIC)֮I2C0λ */
    I2C0->I2CON |= I2C_I2CON_EI_Msk;
    _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_SI_AA );
    NVIC_SetPriority ( I2C0_IRQn, 3 );
    NVIC_EnableIRQ ( I2C0_IRQn );
}
/*-----------------------------------------------------------------*/
/*  I2C0 �ж���� (������startup_M051Series.s����)                 */
/*-----------------------------------------------------------------*/
void I2C0_IRQHandler ( void )
{
    static uint32_t I2CountIndex = 0, I2CountIndex2 = 0;
    uint32_t u32Status;
    u32Status = I2C0->I2CSTATUS;

    if ( I2C0->I2CTOC & I2C_I2CTOC_TIF_Msk )
        {
            // ���I2C0��ʱ���
            I2C0->I2CTOC |= I2C_I2CTOC_TIF_Msk;
        }

    else
        {
            if ( u32Status == I2C_ST1 || u32Status == I2C_ST2 || u32Status == I2C_ST3 || u32Status == I2C_ST4 || u32Status == I2C_ST5 )
                {
                    ;
                }

            switch ( u32Status )
                {
                        //�ӷ�����ģʽ/
                    case I2C_ST1://�����������ֽڲ�����ACK λ
                    case I2C_ST2:
                        {
                            I2CountIndex2 = 0;
                            I2C0->I2CDAT = I2Csend[I2CountIndex2++];
                            _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_SI_AA );
                        }
                        break;

                    case I2C_ST3:
                        {
                            I2C0->I2CDAT = I2Csend[I2CountIndex2++];

                            if ( I2CountIndex2 >= I2Csend[2] )
                                {
                                    I2CountIndex2 = 0;
                                    _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_SI_AA );
                                }

                            else
                                {
                                    _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_SI_AA );
                                }
                        }
                        break;

                    case I2C_ST4:
                    case I2C_ST5:
                        {
                            _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_SI_AA );
                        }
                        break;

                        //	 �ӽ�����ģʽ
                    case I2C_SR1:
                    case I2C_SR2:
                        {
                            I2CountIndex = 0;
                            _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_SI_AA );
                        }
                        break;

                    case I2C_SR5:
                        {
                            I2Crecv[I2CountIndex++] = I2C0->I2CDAT;

                            if ( I2CountIndex == 3 )
                                {
                                    i2clen = I2Crecv[2];
                                }

                            if ( I2CountIndex >= i2clen )
                                {
                                    _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_SI_AA ); //ACK
                                }

                            else
                                {
                                    _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_SI_AA ); //ACK
                                }
                        }
                        break;

                    case I2C_SR6://�л�������ѰַSLA ģʽ��ʶ������SLA
                        {
                            i2clen = 0;
                            _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_SI_AA );
                        }
                        break;

                    case I2C_SR9:
                        I2cTimeTask = 0;
                        //i2clen = 0;
                        _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_SI_AA );
                        CopyTo ( I2Crecv, I2Cuse, ( sizeof ( I2Crecv ) / sizeof ( char ) ) );
                        ClearDim ( I2Crecv, ( sizeof ( I2Crecv ) / sizeof ( char ) ) );
                        break;

                    case 0x00:
                        _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_STO_SI );
                        _I2C_SET_CONTROL_BITS ( I2C0, I2C_I2CON_SI_AA );
                        break;
                }
        }
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
