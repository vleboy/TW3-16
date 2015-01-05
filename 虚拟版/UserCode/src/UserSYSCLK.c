#include "all.h"
/*-----------------------------------------------------------------*/
/* ��ʼ��ϵͳʱ��                                                  */
/*-----------------------------------------------------------------*/
void DelayUs ( uint32_t us )
{
    SysTick->LOAD = 1000 * us * CyclesPerUs / 2260;
    SysTick->VAL  = ( 0x00 );
    SysTick->CTRL =  SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    //
    // Waiting for down-count to zero
    //
    while ( ( SysTick->CTRL & ( 1 << 16 ) ) == 0 );
}
void DelayMs ( uint16_t ms )
{
    unsigned int  index = 0;

    for ( index = 0; index < ms; index++ )
        {
            DelayUs ( 1000 ); //1ms
        }
}

void DelayS ( uint16_t s )
{
    unsigned int  index = 0;

    for ( index = 0; index < s; index++ )
        {
            DelayMs ( 1000 ); //1s
        }
}

void SYS_Init ( void )
{
    /* ���������ļĴ��� */
    SYS_UnlockReg();
    /* ʹ�� OSC22M,OSC10K */
    SYSCLK->PWRCON |= SYSCLK_PWRCON_OSC22M_EN_Msk
                      | SYSCLK_PWRCON_OSC10K_EN_Msk;
    /* �ȴ�ʱ���ȶ� */
    SYS_WaitingForClockReady ( SYSCLK_CLKSTATUS_OSC22M_STB_Msk
                               | SYSCLK_CLKSTATUS_OSC10K_STB_Msk );
    /* �л�HCLK��SysTick��ʱ��Դ */
    SYSCLK->CLKSEL0 = SYSCLK_CLKSEL0_HCLK_IRC22M | SYSCLK_CLKSEL0_STCLK_HCLK_DIV2;
    /* �л�IPģ���ʱ��Դ */
    SYSCLK->CLKSEL1 = SYSCLK_CLKSEL1_WDT_IRC10K
                      | SYSCLK_CLKSEL1_UART_IRC22M
                      | SYSCLK_CLKSEL1_ADC_IRC22M;
    SYSCLK->CLKSEL1 |= SYSCLK_CLKSEL1_TMR0_IRC22M;
    SYSCLK->CLKDIV = ( 0 << SYSCLK_CLKDIV_HCLK_N_Pos )
                     | ( 0 << SYSCLK_CLKDIV_UART_N_Pos )
                     | ( 0 << SYSCLK_CLKDIV_ADC_N_Pos );
    /* !!! SysTick���趨Ϊ����"OSC22M/2", ����ʼ��ʱ��ʱ��
       �벻Ҫ��SysTick->CTRL��ʹ��SysTick_CTRL_CLKSOURCE_Mskλ��*/
    /* PLL����ģʽ */
    SYSCLK->PLLCON = 0x0005C22E;
    /* ���ڿ��԰�ȫ�Ĺر�ûʹ�õ�ʱ���ˣ� */
    SYSCLK->PWRCON &= ~ ( SYSCLK_PWRCON_XTL12M_EN_Msk );
    /* ʱ��AHB�豸ʱ�� */
    SYSCLK->AHBCLK |= SYSCLK_AHBCLK_ISP_EN_Msk;
    /* ʹ����Χ�豸ʱ�� */
    SYSCLK->APBCLK = SYSCLK_APBCLK_I2C_EN_Msk
                     | SYSCLK_APBCLK_WDT_EN_Msk
                     | SYSCLK_APBCLK_UART1_EN_Msk
                     | SYSCLK_APBCLK_ADC_EN_Msk
                     | SYSCLK_APBCLK_TMR0_EN_Msk;
    /* ������Χ�豸 */
    SYS->IPRSTC2 = SYS_IPRSTC2_I2C_RST_Msk
                   | SYS_IPRSTC2_UART1_RST_Msk
                   | SYS_IPRSTC2_ADC_RST_Msk
                   | SYS_IPRSTC2_GPIO_RST_Msk
                   | SYS_IPRSTC2_TMR0_RST_Msk;
    SYS->IPRSTC2 = 0;
    /* ���������ļĴ��� */
    SYS_LockReg();
}
/*-----------------------------------------------------------------*/
/* ��ʼ��IO����                                                    */
/*-----------------------------------------------------------------*/
void IO_Init ( void )
{
    /* �趨���Ÿ��ù��� */
    SYS->P1_MFP = SYS_MFP_P10_AIN0
                  | SYS_MFP_P11_AIN1
                  | SYS_MFP_P12_RXD1
                  | SYS_MFP_P13_TXD1;
    SYS->P3_MFP = SYS_MFP_P34_SDA0
                  | SYS_MFP_P35_SCL0;
    /* ���������������ģʽ */
    P0->PMD  = 0x0000FFFD;
    P0->OFFD = 0x00000001;
    P01 = 0;
    P00 = 0;
#ifdef vhe01
    P40 = 1;
#else
    P15 = 1;
#endif
}


