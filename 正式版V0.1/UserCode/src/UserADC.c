#include "all.h"

uint32_t ADCvalue[10][4];/*AIN0 ���ջ��⹦��1
													AIN1 ����������A
													AIN2 ������⹦��A
													AIN3 ���ջ��⹦��2
													AIN4 +5V
													AIN5 +24V
													AIN6 AC60V
													AIN7 RF_POW 
													8:	 ����������B
													9:   ������⹦��B*/
uint32_t ADCcovVal[10];
/*-----------------------------------------------------------------*/
/* ��ʼ��ADC                                                       */
/*-----------------------------------------------------------------*/
void ADC_Init()
{
    /* �趨ADC���ƼĴ��� */
    ADC->ADCR = ADC_ADCR_DMOF_UNSIGNED_OUTPUT
                | ADC_ADCR_DIFFEN_SINGLE_END
                | ADC_ADCR_TRGCOND_LOW_LEVEL
                | ADC_ADCR_ADMD_CONTINUOUS
                | ADC_ADCR_ADIE_Msk
                | ADC_ADCR_ADEN_CONVERTER_ENABLE;
    /* ʹ��ģ������ͨ�� */
    ADC->ADCHER = 0xFF;
    /* У׼ADC */
    ADC->ADCALR |= ADC_ADCALR_CALEN_Msk;

    while ( ! ( ADC->ADCALR & ADC_ADCALR_CALDONE_Msk ) )
        {
            ;
        }

    /* ʹ��ADC�ж� */
    NVIC_EnableIRQ ( ADC_IRQn );
}


/*-----------------------------------------------------------------*/
/* ADC �ж���� (������startup_M051Series.s����)                   */
/*-----------------------------------------------------------------*/
void ADC_IRQHandler ( void )
{
    static unsigned char index[10];
		unsigned char i=0;
    NVIC_DisableIRQ ( ADC_IRQn );

    if ( ADC->ADSR & ADC_ADSR_ADF_Msk )
        {
					for(i=0;i<8;i++)
					{
						if(i==1 && P03== IbB)
						{
							if ( ( ADC->ADDR[i] & ADC_ADDR_VALID_Msk ) )
                {
                    ADCvalue[8][index[8]] = ( ADC->ADDR[i] & ADC_ADDR_RSLT_Msk );
                    index[8]++;
                }
						continue;
						}
						
						if(i==2 && P02==TxB )
						{
							if ( ( ADC->ADDR[i] & ADC_ADDR_VALID_Msk ) )
                {
                    ADCvalue[9][index[9]] = ( ADC->ADDR[i] & ADC_ADDR_RSLT_Msk );
                    index[9]++;
                }
					  continue;
						}
						
						if ( ( ADC->ADDR[i] & ADC_ADDR_VALID_Msk ) )
                {
                    ADCvalue[i][index[i]] = ( ADC->ADDR[i] & ADC_ADDR_RSLT_Msk );
                    index[i]++;
                }
					}

            ADC->ADSR = ADC_ADSR_ADF_Msk;      /* ���A/Dת��������� */
        }

				for(i=0;i<10;i++)
				{
					if(index[i] > 3)
					{
						ADCcovVal[i] = FourDataAdc(&ADCvalue[i][0]);
						index[i] = 0;
					}
				}
				MainStructR.ORAop = ADCcovVal[0];//AIN0 ���ջ��⹦��1
				MainStructR.OTAim = ADCcovVal[1];//									AIN1 ����������A
				MainStructR.OTAop = ADCcovVal[2];//									AIN2 ������⹦��A
				MainStructR.ORBop = ADCcovVal[3];//									AIN3 ���ջ��⹦��2
				MainStructR.DC5   = ADCcovVal[4];//									AIN4 +5V
				MainStructR.DC24  = ADCcovVal[5];//									AIN5 +24V
				MainStructR.AC60  = ADCcovVal[6];//									AIN6 AC60V
				MainStructR.RF    = ADCcovVal[7];//									AIN7 RF_POW 
				MainStructR.OTBim = ADCcovVal[8];//									8:	 ����������B
				MainStructR.OTBop = ADCcovVal[9];//									9:   ������⹦��B*/
}

/*-----------------------------------------------------------------*/
/* ADC �˲�                           ��ֵ�˲�                     */
/*-----------------------------------------------------------------*/
unsigned int FourDataAdc ( unsigned int *data )
{
    unsigned int max=data[0],min = data[0];
    unsigned char index;
    unsigned int filterdata;

    for ( index = 0; index < 4; index++ )
        {
            if ( max <= data[index] ) { max = data[index]; }

            if ( min >= data[index] ) { min = data[index]; }
        }

    filterdata = ( ( 0ul + data[0] + data[1] + data[2] + data[3] - max - min ) / 2 );
    return filterdata;
}

