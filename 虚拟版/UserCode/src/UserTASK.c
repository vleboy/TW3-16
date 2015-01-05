#include "all.h"

unsigned char InquireFrame[] =
{
    0xf5 , 0x00 , 0x06 , 0x00  , 0xfb , 0xaa ,// F5 ADD LENALL MODE CS AA
};

unsigned char ModuCheckFlg = 0;

void Inquire ( unsigned char mode )
{
    if ( mode == 0x01 )
        {
            InquireFrame[1] = 0x01;
            InquireFrame[4] = 0xfb + InquireFrame[1];
            RS485_Send ( InquireFrame, 6 );
        }

    else if ( mode == 0x02 )
        {
            InquireFrame[1] = 0x02;
            InquireFrame[4] = 0xfb + InquireFrame[1];
            RS485_Send ( InquireFrame, 6 );
        }

    else if ( mode == 0x03 )
        {
            InquireFrame[1] = 0x03;
            InquireFrame[4] = 0xfb + InquireFrame[1];
            RS485_Send ( InquireFrame, 6 );
        }

    else if ( mode == 0x04 )
        {
            InquireFrame[1] = 0x04;
            InquireFrame[4] = 0xfb + InquireFrame[1];
            RS485_Send ( InquireFrame, 6 );
        }

    else if ( mode == 0x05 )
        {
            InquireFrame[1] = 0x05;
            InquireFrame[4] = 0xfb + InquireFrame[1];
            RS485_Send ( InquireFrame, 6 );
        }

    else if ( mode == 0x06 )
        {
            InquireFrame[1] = 0x06;
            InquireFrame[4] = 0xfb + InquireFrame[1];
            RS485_Send ( InquireFrame, 6 );
        }

    else if ( mode == 0x07 )
        {
            InquireFrame[1] = 0x07;
            InquireFrame[4] = 0xfb + InquireFrame[1];
            RS485_Send ( InquireFrame, 6 );
        }

    else if ( mode == 0x08 )
        {
            InquireFrame[1] = 0x08;
            InquireFrame[4] = 0xfb + InquireFrame[1];
            RS485_Send ( InquireFrame, 6 );
        }

    else if ( mode == 0x09 )
        {
            InquireFrame[1] = 0x09;
            InquireFrame[4] = 0xfb + InquireFrame[1];
            RS485_Send ( InquireFrame, 6 );
        }

    else if ( mode == 0x0a )
        {
            InquireFrame[1] = 0x0a;
            InquireFrame[4] = 0xfb + InquireFrame[1];
            RS485_Send ( InquireFrame, 6 );
        }

    DelayMs ( 20 );
}


void AdcTask ( void )
{
    NVIC_EnableIRQ ( ADC_IRQn );
}


