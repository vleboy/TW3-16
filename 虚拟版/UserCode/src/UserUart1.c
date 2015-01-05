#include "all.h"
/*-----------------------------------------------------------------*/
/* 初始化UART1                                                     */
/*-----------------------------------------------------------------*/
void SAVE_Flash ( void );
UartRcvStruct UartRcvFrame;
unsigned char saveflg = 0;




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
    P00 = 1;
#else
    P01 = 1;
#endif

    for ( u32Count = 0; u32Count != len; u32Count++ )
        {
            RS485_W ( databuf[u32Count] );     /* Send UART Data from buffer */
        }

#ifdef vhe02
    P00 = 0;
#else
    P01 = 0;
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
            frame->rcvflag = UART_RCVFLAG_HEAD;
            frame->count = 0;
            frame->chksum = 0;
        }

    switch ( frame->rcvflag )
        {
            case UART_RCVFLAG_HEAD:
                {
                    if ( dat == FRAME_HEAD )
                        {
                            frame->rcvflag = UART_RCVFLAG_DES;
                          //  frame->chksum = 0;		// 效验位不包含HEAD
                            frame->count = 0;
                            UartTimeDelay = UartRcvTimeDly; // 接收到第一个字节开始计时
                        }

                    else
                        {
                            frame->rcvflag =   UART_RCVFLAG_HEAD;
                        }
                }
                break;

            case UART_RCVFLAG_DES:
                {
                    if(FRAME_DES == dat)
										{
											frame->rcvflag = UART_RCVFLAG_LEN;
                      frame->chksum = dat;		
                      
                           
										}
										else
                        {
                            frame->rcvflag =   UART_RCVFLAG_HEAD;
                        }
                break;

            case UART_RCVFLAG_LEN:
                {
                    frame->len = dat;

                    if ( frame->len > UartFrameLen )
                        {
                            frame->rcvflag =   UART_RCVFLAG_HEAD;
                        }

                    else if ( frame->len != 0 )
                        {
                            frame->rcvflag =   UART_RCVFLAG_PAY;
                            frame->chksum = frame->chksum + dat;							// 效验
                        }
                }
                break;

            case UART_RCVFLAG_PAY:
                {
                    frame->dat[frame->count] = dat;
                    frame->chksum = frame->chksum + dat;
                    frame->count++;

                    if ( frame->count >= frame->len ) 					//
                        {
                            frame->rcvflag = UART_RCVFLAG_SUM ;
                            frame->count = 0;
														frame->cmd = frame->dat[0];
														
                        }
                }
                break;

            case UART_RCVFLAG_SUM:
                {
                    frame->cs = dat;

                    if ( frame->chksum != frame->cs )
                        {
                            frame->chksum = 0;
                            frame->rcvflag =  UART_RCVFLAG_HEAD;
                        }

                    else
                        {
                            frame->rcvflag =  UART_RCVFLAG_END;
                        }
                }
                break;

            case UART_RCVFLAG_END:
                {
                    if ( dat == FRAME_END )
                        {
                            frame->ProFlag = 1; 		//数据帧已经准备好
                        }

                    frame->rcvflag = UART_RCVFLAG_HEAD ;
                }
                break;

            default:
                {
                    frame->rcvflag = UART_RCVFLAG_HEAD ;
                }
                break;
        }
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
unsigned char Sender[50]={0};
    if ( frame->ProFlag != 1 )			//没有准备好的数据帧
        {
            return;
        }

    switch ( frame->dat[0] )
        {
         case 0x81:
				 {
					  Sender[0] = FRAME_HEAD;
						Sender[1] = FRAME_DES;
						Sender[2] = 0x02;
						Sender[3] = 0x81;
					 if(frame->len >17)
					 {
							Sender[4] = 0x01;
						}
					 else
					 {
						Sender[4] = 0x00;
						for(index = 0;index<frame->len-1;index++)
						{
							MainAscii.PartNumber[index] = frame->dat[index+1];
						}
					   saveflg = 1;
				 }
					 for(index = 1;index<5;index++)
					{
						Sender[5]+=Sender[index]; 
					}
					Sender[6] = FRAME_END;	
					
						RS485_Send(Sender,7);
					}
				 break;
				 case 0x01:
				 {
						Sender[0] = FRAME_HEAD;
						Sender[1] = FRAME_DES;
						Sender[2] = 0x11;
						Sender[3] = 0x01;
						for(index = 0;index < 16;index++)
						{
							Sender[4+index] = MainAscii.PartNumber[index];
						}
						for(index = 1;index<20;index++)
						{
							Sender[20] += Sender[index];
						}
						Sender[21] = FRAME_END;
						RS485_Send(Sender,22);
					}
				 break;
				 case 0x82:
				 {
						Sender[0] = FRAME_HEAD;
						Sender[1] = FRAME_DES;
						Sender[2] = 0x02;
						Sender[3] = 0x82;
					 if(frame->len >17)
					 {
							Sender[4] = 0x01;
						}
					 else
					 {
						Sender[4] = 0x00;
						for(index = 0;index<frame->len-1;index++)
						{
							MainAscii.LogicID[index] = frame->dat[index+1];
						}
					   saveflg = 1;
				 }
					 for(index = 1;index<5;index++)
					{
						Sender[5]+=Sender[index]; 
					}
					Sender[6] = FRAME_END;	
					
						RS485_Send(Sender,7);
					}
				 break;
				 case 0x02:
				 {
						Sender[0] = FRAME_HEAD;
						Sender[1] = FRAME_DES;
						Sender[2] = 0x11;
						Sender[3] = 0x02;
						for(index = 0;index < 16;index++)
						{
							Sender[4+index] = MainAscii.LogicID[index];
						}
						for(index = 1;index<20;index++)
						{
							Sender[20] += Sender[index];
						}
						Sender[21] = FRAME_END;
						RS485_Send(Sender,22);
					}
				 break;
				 case 0x83:
				 {
						Sender[0] = FRAME_HEAD;
						Sender[1] = FRAME_DES;
						Sender[2] = 0x02;
						Sender[3] = 0x83;
					 if(frame->len >17)
					 {
							Sender[4] = 0x01;
						}
					 else
					 {
						Sender[4] = 0x00;
						for(index = 0;index<frame->len-1;index++)
						{
							MainAscii.VendorName[index] = frame->dat[index+1];
						}
					   saveflg = 1;
				 }
					 for(index = 1;index<5;index++)
					{
						Sender[5]+=Sender[index]; 
					}
					Sender[6] = FRAME_END;	
					
						RS485_Send(Sender,7);
					}
				
				 break;
				 case 0x03:
				 {
						Sender[0] = FRAME_HEAD;
						Sender[1] = FRAME_DES;
						Sender[2] = 0x11;
						Sender[3] = 0x03;
						for(index = 0;index < 16;index++)
						{
							Sender[4+index] = MainAscii.VendorName[index];
						}
						for(index = 1;index<20;index++)
						{
							Sender[20] += Sender[index];
						}
						Sender[21] = FRAME_END;
						RS485_Send(Sender,22);
					}
				 break;
				 case 0x84:
				 {
				Sender[0] = FRAME_HEAD;
						Sender[1] = FRAME_DES;
						Sender[2] = 0x02;
						Sender[3] = 0x84;
					 if(frame->len >17)
					 {
							Sender[4] = 0x01;
						}
					 else
					 {
						Sender[4] = 0x00;
						for(index = 0;index<frame->len-1;index++)
						{
							MainAscii.SerialNumber[index] = frame->dat[index+1];
						}
					   saveflg = 1;
				 }
					 for(index = 1;index<5;index++)
					{
						Sender[5]+=Sender[index]; 
					}
					Sender[6] = FRAME_END;	
					
						RS485_Send(Sender,7);
					}
				 break;
				 case 0x04:
				 {
						Sender[0] = FRAME_HEAD;
						Sender[1] = FRAME_DES;
						Sender[2] = 0x11;
						Sender[3] = 0x04;
						for(index = 0;index < 12;index++)
						{
							Sender[4+index] = MainAscii.SerialNumber[index];
						}
						for(index = 1;index<20;index++)
						{
							Sender[20] += Sender[index];
						}
						Sender[21] = FRAME_END;
						RS485_Send(Sender,22);
				 }
				 break;
				 case 0x91:
				 {
							
						switch(frame->dat[1])
						{
						case 0x01:
						{
								MainStructW.AB_Switch = frame->dat[2];
						}
						break;
						case 0x02:
						{
								MainStructW.AB_Switch_door = frame->dat[2];
						}
						break;
						case 0x03:
						{
								MainStructW.Port12 = frame->dat[2];
						}
						break;
						case 0x04:
						{
								MainStructW.Port34 = frame->dat[2];
						}
						break;
						case 0x05:
						{
								MainStructW.AGCa = frame->dat[2];
						}
						break;
						case 0x06:
						{
								MainStructW.AGCb = frame->dat[2];
						}
						break;
						case 0x07:
						{
								MainStructW.RF = frame->dat[2];
						}
						break;
				 }
							Sender[0] = FRAME_HEAD;
							Sender[1] = FRAME_DES;
							Sender[2] = 0x02;
							Sender[3] = 0x91;
						if(frame->len != 0x03)
							{
								Sender[4] = 0x01;
							}
							else
							{
								saveflg = 1;
								Sender[4] = 0x00;
							}
						for(index=1;index<5;index++)
							{
								Sender[5] +=Sender[index]; 
							}
							Sender[6] = FRAME_END;
							RS485_Send(Sender,7);
				}
				 break;
				 case 0x11:
				 {
						Sender[0] = FRAME_HEAD;
						Sender[1] = FRAME_DES;
						Sender[2] = 0x18;
						Sender[3] = 0x11;
						Sender[4] = MainStructR.online;
						Sender[5] = MainStructR.AC60;
						Sender[6] = MainStructR.DC24;
						Sender[7] = MainStructR.DC5;
						Sender[8] = MainStructR.Temp;
						Sender[9] = MainStructR.ORAop;
						Sender[10]= MainStructR.ORBop;
						Sender[11]= MainStructR.OTAop;
						Sender[12]= MainStructR.OTBop;
						Sender[13]= MainStructR.OTAim;
						Sender[14]= MainStructR.OTBim;
						Sender[15]= MainStructR.RF;
						Sender[16]= MainStructW.AGCa;
						Sender[17]= MainStructW.AGCb;
						Sender[18]= MainStructW.RF;
						Sender[19]= MainStructW.AB_Switch;
						Sender[20]= MainStructR.AB;
						Sender[21]= MainStructW.AB_Switch_door;
						Sender[22]= MainStructW.Port12;
						Sender[23]= MainStructW.Port34;
						Sender[24]= MainStructR.Box;
						Sender[25]= MainStructR.Warning;
						Sender[26]= Version;
						
						for(index=1;index<27;index++)
						{
							Sender[27]+=Sender[index];
						}
						Sender[28] = FRAME_END;
						RS485_Send(Sender,29);
				}
				 break;
				 case 0x92:
				 {
						MainStructW.AB_Switch = frame->dat[1];
						MainStructW.AB_Switch_door = frame->dat[2];
						MainStructW.Port12 = frame->dat[3];
						MainStructW.Port34 = frame->dat[4];
						MainStructW.AGCa = frame->dat[5];
						MainStructW.AGCb = frame->dat[6];
						MainStructW.RF = frame->dat[7];
						
						
						Sender[0] = FRAME_HEAD;
						Sender[1] = FRAME_DES;
						Sender[2] = 0x02;
						Sender[3] = 0x92;
						if(frame->len != 0x08)
						{
							Sender[4] = 0x01;
						}
						else
						{
							Sender[4] = 0x00;
							saveflg = 1;
						}
						for(index=1;index<5;index++)
						{
							Sender[5]+=Sender[index];
						}
						Sender[6] = FRAME_END;
						RS485_Send(Sender,7);
				 }
				 break;
        }
		memset(Sender,0,50);
    memset ( frame, 0, sizeof ( UartRcvStruct ) );
		 SAVE_Flash();
    frame->ProFlag = 0;								// 数据处理完成
}
void SAVE_Flash ( void )
{
    if ( saveflg != 0 )
        {
					 FMC_Erase(LD_Add_start);
           FLASH_WriteNBytes ( LD_Add_start, ( unsigned char * ) &MainStructW.Save_flag, ( sizeof ( MainStructW ) / sizeof ( char ) ) );
					 FLASH_WriteNBytes((LD_Add_start+ sizeof ( MainStructW ) / sizeof ( char ) ),(unsigned char *)&MainAscii.PartNumber[0],(sizeof(MainAscii)/sizeof(char)));
					 saveflg = 0;
        }
}
