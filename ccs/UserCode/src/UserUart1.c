#include "all.h"
/*-----------------------------------------------------------------*/
/* 初始化UART1                                                     */
/*-----------------------------------------------------------------*/
uint8_t g_u8SendData[12] = {0};
UartRcvStruct UartRcvFrame;

ModuLenNumStruct Modu1, Modu2, Modu3, Modu4, Modu5, Modu6, Modu7, Modu8, Modu9, ModuA, ModuTemp;
MainStruct MainModu;


void UART1_Init ( void )
{
    /* UART1设定 */
    UART1->BAUD = UART_BAUD_MODE0 | UART_BAUD_DIV_MODE0 ( 22118400, 115200 );
    _UART_SET_DATA_FORMAT ( UART1, UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1 );
    UART1->FCR = UART_FCR_RTS_TRI_LEV_1BYTE
                 | UART_FCR_RFITL_1BYTE
                 | UART_FCR_RX_DIS_Msk;
    /* 使能UART1中断 */
    _UART_ENABLE_INT ( UART1, UART_IER_RDA_IEN_Msk );
    NVIC_EnableIRQ ( UART1_IRQn );
}

/*-----------------------------------------------------------------*/
/* UART1 中断入口 (函数由startup_M051Series.s引入)                 */
/*-----------------------------------------------------------------*/
void UART1_IRQHandler ( void )
{
    uint8_t u8InChar = 0xFF;
    uint32_t u32IntSts = UART1->ISR;
    __set_PRIMASK ( 1 );	//关闭总中断

    if ( u32IntSts & UART_ISR_RDA_INT_Msk )
        {
            //  Get all the input characters
            while ( _UART_IS_RX_READY ( UART1 ) )
                {
                    // Get the character from UART Buffer
                    _UART_RECEIVEBYTE ( UART1, u8InChar );       /* Rx trigger level is 1 byte*/
                    UartFrameRcv ( &UartRcvFrame, u8InChar );
                }
        }

    __set_PRIMASK ( 0 );
}

void RS485_W ( uint8_t data )
{
    _UART_SENDBYTE ( ( UART1 ), data );
    DelayUs ( 100 );
}

int32_t RS485_Send ( uint8_t *databuf, uint32_t len )
{
    uint32_t  u32Count;
#ifdef vhe02
    //P00 = 1;
#else
   // P01 = 1;
#endif

    for ( u32Count = 0; u32Count != len; u32Count++ )
        {
            RS485_W ( databuf[u32Count] );     /* Send UART Data from buffer */
        }

#ifdef vhe02
    //P00 = 0;
#else
    //P01 = 0;
#endif
    return TRUE;
}

/*************************************************************
*函数名称：void UartFrameRcv(UartRcvStruct *frame,unsigned char dat)
*函数功能：接收帧验证，串口中断中调用
*参    数：数据帧，数据
*返    回：null
*************************************************************/
void UartFrameRcv ( UartRcvStruct *frame, unsigned char dat )
{
    if ( frame->ProFlag != 0 ) 					//当前数据正在处理
        {
            return;
        }

    if ( UartTimeDelay == 0 )						// 超时，退出
        {
            frame->rcvflag = UART_RCVFLAG_HEAD1;
            frame->count = 0;
            frame->chksum = 0;
        }

    switch ( frame->rcvflag )
        {
            case UART_RCVFLAG_HEAD1:
                {
                    if ( dat == FRAME_HEAD1 )
                        {
                            frame->rcvflag = UART_RCVFLAG_ADDR;
                            frame->chksum = dat;		// 效验
                            frame->count = 0;
                            UartTimeDelay = UartRcvTimeDly; // 接收到第一个字节开始计时
                        }

                    else
                        {
                            frame->rcvflag =   UART_RCVFLAG_HEAD1;
                        }
                }
                break;

            case UART_RCVFLAG_ADDR:
                {
                    frame->addr = dat;
                    frame->chksum = frame->chksum + dat;

                    if ( frame->addr != MainAddr )
                        {
                            frame->rcvflag = UART_RCVFLAG_HEAD1;
                        }

                    else
                        {
                            frame->rcvflag = UART_RCVFLAG_HEAD2;
                        }
                }
                break;

            case UART_RCVFLAG_HEAD2:
                {
                    if ( dat == FRAME_HEAD2 )
                        {
                            frame->rcvflag = UART_RCVFLAG_CTL;
                            frame->chksum = frame->chksum + dat;							// 效验
                        }

                    else
                        {
                            frame->rcvflag =   UART_RCVFLAG_HEAD1;
                        }
                }
                break;

            case UART_RCVFLAG_CTL:
                {
                    frame->ctlcode = dat;
                    frame->chksum = frame->chksum + dat;
                    frame->rcvflag = UART_RCVFLAG_LEN;
                }
                break;

            case UART_RCVFLAG_LEN :
                {
                    frame->len = dat;

                    if ( frame->len > UartFrameLen )
                        {
                            frame->rcvflag =   UART_RCVFLAG_HEAD1;
                        }

                    else if ( frame->len != 0 )
                        {
                            frame->rcvflag =   UART_RCVFLAG_DAT;
                            frame->chksum = frame->chksum + dat;							// 效验
                        }

                    else
                        {
                            frame->rcvflag =   UART_RCVFLAG_CS;
                            frame->chksum = frame->chksum + dat;
                        }
                }
                break;

            case UART_RCVFLAG_DAT:
                {
                    frame->dat[frame->count] = dat;
                    frame->chksum = frame->chksum + dat;
                    frame->count++;

                    if ( frame->count >= frame->len ) 					//
                        {
                            frame->rcvflag = UART_RCVFLAG_CS ;
                            frame->count = 0;
                        }
                }
                break;

            case UART_RCVFLAG_CS:
                {
                    frame->cs = dat;

                    if ( frame->chksum != frame->cs )
                        {
                            frame->chksum = 0;
                            frame->rcvflag =  UART_RCVFLAG_HEAD1;
                        }

                    else
                        {
                            frame->rcvflag =  UART_RCVFLAG_REAR;
                        }
                }
                break;

            case UART_RCVFLAG_REAR:
                {
                    if ( dat == FRAME_REAR )
                        {
                            frame->ProFlag = 1; 		//数据帧已经准备好
                        }

                    frame->rcvflag = UART_RCVFLAG_HEAD1 ;
                }
                break;

            default:
                {
                    frame->rcvflag = UART_RCVFLAG_HEAD1 ;
                }
                break;
        }
}


///*************************************************************
//*函数名称：void UartFramePro(UartRcvStruct *frame)
//*函数功能：接收帧处理
//*参    数：数据帧
//*返    回：null
//*************************************************************/
void UartFramePro ( UartRcvStruct *frame )
{
    unsigned char index = 0;

    if ( frame->ProFlag != 1 )			//没有准备好的数据帧
        {
            return;
        }

    switch ( frame->ctlcode )
        {
            case 0x91 :		//		从站请求帧,发送读取回来的数据
                {
                    switch ( frame->dat[0] )
                        {
                            case ModuAddr1 :
                                {
                                    Modu1.Online = 1;
                                    MainModu.Inserted |= ( 0x01 << 0 );
                                    Modu1.Type = frame->dat[1];
                                    Modu1.ModuType = frame->dat[2];
                                    Modu1.DataLen = frame->len - 4;

                                    for ( index = 0; index < frame->len - 4; index++ )
                                        {
                                            Modu1.ModuData[index] = frame->dat[4 + index];
                                        }
                                }
                                break;

                            case ModuAddr2:
                                {
                                    Modu2.Online = 1;
                                    MainModu.Inserted |= ( 0x01 << 1 );
                                    Modu2.Type = frame->dat[1];
                                    Modu2.ModuType = frame->dat[2];
                                    Modu2.DataLen = frame->len - 4;

                                    for ( index = 0; index < frame->len - 4; index++ )
                                        {
                                            Modu2.ModuData[index] = frame->dat[4 + index];
                                        }
                                }
                                break;

                            case ModuAddr3:
                                {
                                    Modu3.Online = 1;
                                    MainModu.Inserted |= ( 0x01 << 2 );
                                    Modu3.Type = frame->dat[1];
                                    Modu3.ModuType = frame->dat[2];
                                    Modu3.DataLen = frame->len - 4;

                                    for ( index = 0; index < frame->len - 4; index++ )
                                        {
                                            Modu3.ModuData[index] = frame->dat[4 + index];
                                        }
                                }
                                break;

                            case ModuAddr4:
                                {
                                    Modu4.Online = 1;
                                    MainModu.Inserted |= ( 0x01 << 3 );
                                    Modu4.Type = frame->dat[1];
                                    Modu4.ModuType = frame->dat[2];
                                    Modu4.DataLen = frame->len - 4;

                                    for ( index = 0; index < frame->len - 4; index++ )
                                        {
                                            Modu4.ModuData[index] = frame->dat[4 + index];
                                        }
                                }
                                break;

                            case ModuAddr5:
                                {
                                    Modu5.Online = 1;
                                    MainModu.Inserted |= ( 0x01 << 4 );
                                    Modu5.Type = frame->dat[1];
                                    Modu5.ModuType = frame->dat[2];
                                    Modu5.DataLen = frame->len - 4;

                                    for ( index = 0; index < frame->len - 4; index++ )
                                        {
                                            Modu5.ModuData[index] = frame->dat[4 + index];
                                        }
                                }
                                break;

                            case ModuAddr6:
                                {
                                    Modu6.Online = 1;
                                    MainModu.Inserted |= ( 0x01 << 5 );
                                    Modu6.Type = frame->dat[1];
                                    Modu6.ModuType = frame->dat[2];
                                    Modu6.DataLen = frame->len - 4;

                                    for ( index = 0; index < frame->len - 4; index++ )
                                        {
                                            Modu6.ModuData[index] = frame->dat[4 + index];
                                        }
                                }
                                break;

                            case ModuAddr7:
                                {
                                    Modu7.Online = 1;
                                    MainModu.Inserted |= ( 0x01 << 6 );
                                    Modu7.Type = frame->dat[1];
                                    Modu7.ModuType = frame->dat[2];
                                    Modu7.DataLen = frame->len - 4;

                                    for ( index = 0; index < frame->len - 4; index++ )
                                        {
                                            Modu7.ModuData[index] = frame->dat[4 + index];
                                        }
                                }
                                break;

                            case ModuAddr8:
                                {
                                    Modu8.Online = 1;
                                    MainModu.Inserted |= ( 0x01 << 7 );
                                    Modu8.Type = frame->dat[1];
                                    Modu8.ModuType = frame->dat[2];
                                    Modu8.DataLen = frame->len - 4;

                                    for ( index = 0; index < frame->len - 4; index++ )
                                        {
                                            Modu8.ModuData[index] = frame->dat[4 + index];
                                        }
                                }
                                break;

                            case ModuAddr9:
                                {
                                    //								RX_TEMP++;
                                    Modu9.Online = 1;
                                    MainModu.Inserted |= ( 0x01 << 8 );
                                    Modu9.Type = frame->dat[1];
                                    Modu9.ModuType = frame->dat[2];
                                    Modu9.DataLen = frame->len - 4;

                                    for ( index = 0; index < frame->len - 4; index++ )
                                        {
                                            Modu9.ModuData[index] = frame->dat[4 + index];
                                        }
                                }
                                break;

                            case ModuAddrA:
                                {
                                    ModuA.Online = 1;
                                    MainModu.Inserted |= ( 0x01 << 9 );
                                    ModuA.Type = frame->dat[1];
                                    ModuA.ModuType = frame->dat[2];
                                    ModuA.DataLen = frame->len - 4;

                                    for ( index = 0; index < frame->len - 4; index++ )
                                        {
                                            ModuA.ModuData[index] = frame->dat[4 + index];
                                        }
                                }
                                break;
                        }
                }
                break;
        }

    memset ( frame, 0, sizeof ( UartRcvStruct ) );
    frame->ProFlag = 0;								// 数据处理完成
}

unsigned char ModuCheck ( unsigned char num )
{
    ModuLenNumStruct *p_modu;
    p_modu = &ModuTemp;
    *p_modu = I2cSwitchModu ( num );

    if ( p_modu->Online == 0 )
        {
            return FALSE;
        }

    else
        {
            return TRUE;
        }
}

