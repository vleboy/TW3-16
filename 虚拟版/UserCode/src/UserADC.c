#include "all.h"
uint32_t ADCtempA[4] = {0};
uint32_t ADCtempB[4] = {0};

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
    ADC->ADCHER = 0x03;
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
    static unsigned char indexA = 0, indexB = 0;
    NVIC_DisableIRQ ( ADC_IRQn );

    if ( ADC->ADSR & ADC_ADSR_ADF_Msk )
        {
            if ( ( ADC->ADDR[0] & ADC_ADDR_VALID_Msk ) )
                {
                    ADCtempA[indexA] = ( ADC->ADDR[0] & ADC_ADDR_RSLT_Msk );
                    indexA++;
                }

            if ( ADC->ADDR[1] & ADC_ADDR_VALID_Msk )
                {
                    ADCtempB[indexB] = ( ADC->ADDR[1] & ADC_ADDR_RSLT_Msk );
                    indexB++;
                }

            ADC->ADSR = ADC_ADSR_ADF_Msk;      /* ���A/Dת��������� */
        }

    if ( indexA > 3 && indexB > 3 )
        {
            indexA = 0;
            indexB = 0;
            //MainModu.POWERA = FourDataAdc ( ADCtempA );
            //MainModu.POWERB = FourDataAdc ( ADCtempB );
        }

    if ( indexA > 200 )
        {
            indexA = 0;
        }

    if ( indexB > 200 )
        {
            indexB = 0;
        }
}

/*-----------------------------------------------------------------*/
/* ADC �˲�                           ��ֵ�˲�                     */
/*-----------------------------------------------------------------*/
unsigned int FourDataAdc ( unsigned int *data )
{
    unsigned int max = 0, min = 0;
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

