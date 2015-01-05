#include "all.h"
/*-----------------------------------------------------------------*/
/* 初始化Timer0                                                    */
/*-----------------------------------------------------------------*/
uint32_t UartTimeDelay = 0, AdcTimeTask = 0, Uart1TimeTask = 0;
void TMR0_Init ( void )
{
    /* 使能Timer0 */
    TIMER0->TCSR = TIMER_TCSR_IE_Msk
                   | TIMER_TCSR_MODE_PERIODIC
                   | TIMER_TCSR_TDR_EN_Msk
                   | TIMER_TCSR_PRESCALE ( 5 );
    /* 设定定时器比较值 */
    TIMER0->TCMPR = 18432;//5MS；
    /* 设定外部引脚功能 */
    // TIMER0->TEXCON = TIMER_TEXCON_TX_PHASE_RISING;
    /* 使能Timer0中断 */
    NVIC_EnableIRQ ( TMR0_IRQn );
    //	NVIC_SetPriority(TMR0_IRQn,0);
    TIMER0->TCSR |= TIMER_TCSR_CEN_Msk;//开始TIME0
}

/*-----------------------------------------------------------------*/
/* Timer0 中断入口 (函数由startup_M051Series.s引入)                */
/*-----------------------------------------------------------------*/
void TMR0_IRQHandler ( void )
{
    /*处理Timer0中断 */
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

    

    /* 清除Timer0计数达到比较值的中断标记 */
    _TIMER_CLEAR_CMP_INT_FLAG ( TIMER0 );
}


