#include "all.h"


void ControlInit ( void )
{
    PE4304io_LE1 = LOW;
    PE4304io_LE2 = LOW;
   
}
void PE4304Send ( unsigned char num , unsigned char Dat )
{
    unsigned char index;
    unsigned char att_data;
    att_data = ( Dat << 2 );

    for ( index = 0; index < 6; index++ )
        {
            PE4304_CLOCK ( LOW );
            DelayUs ( 10 );

            if ( att_data & 0x80 )
                {
                    PE4304_DATA ( 1 );
                }

            else
                {
                    PE4304_DATA ( 0 );
                }

            att_data <<= 1;
            PE4304_CLOCK ( HIGH );
            DelayUs ( 10 );
        }

    PE4304_CLOCK ( LOW );
    DelayUs ( 10 );

    if ( num == 1 )
        {
            PE4304io_LE1 = HIGH;
        }
    else if ( num == 2 )
        {
            PE4304io_LE2 = HIGH;
        }

    DelayUs ( 10 );
    PE4304io_LE1 = LOW;
    PE4304io_LE2 = LOW;
   
}
