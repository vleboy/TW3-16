#include "all.h"
/*-----------------------------------------------------------------*/
/* ��ʼ��Timer0                                                    */
/*-----------------------------------------------------------------*/
uint32_t UartTimeDelay = 0, AdcTimeTask = 0, Uart1TimeTask = 0;
void TMR0_Init ( void )
{
    /* ʹ��Timer0 */
    TIMER0->TCSR = TIMER_TCSR_IE_Msk
                   | TIMER_TCSR_MODE_PERIODIC
                   | TIMER_TCSR_TDR_EN_Msk
                   | TIMER_TCSR_PRESCALE ( 5 );
    /* �趨��ʱ���Ƚ�ֵ */
    TIMER0->TCMPR = 18432;//5MS��
    /* �趨�ⲿ���Ź��� */
    // TIMER0->TEXCON = TIMER_TEXCON_TX_PHASE_RISING;
    /* ʹ��Timer0�ж� */
    NVIC_EnableIRQ ( TMR0_IRQn );
    //	NVIC_SetPriority(TMR0_IRQn,0);
    TIMER0->TCSR |= TIMER_TCSR_CEN_Msk;//��ʼTIME0
}

/*-----------------------------------------------------------------*/
/* Timer0 �ж���� (������startup_M051Series.s����)                */
/*-----------------------------------------------------------------*/
void TMR0_IRQHandler ( void )
{
    /*����Timer0�ж� */
    if ( UartTimeDelay > 0 )
        {
            UartTimeDelay--;
        }

    if ( AdcTimeTask < AdcTimeTaskFlg )
        {
            AdcTimeTask++;
        }

    if ( Uart1TimeTask < Uart1TimeTaskFlg )
        {
            Uart1TimeTask++;
        }

    

    /* ���Timer0�����ﵽ�Ƚ�ֵ���жϱ�� */
    _TIMER_CLEAR_CMP_INT_FLAG ( TIMER0 );
}


