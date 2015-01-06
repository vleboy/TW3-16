#include "all.h"
static DMA_InitTypeDef DMA_InitStructure; 
UartRcvStruct UartRcvFrame;
static NVIC_InitTypeDef NVIC_InitStructure; 
static USART_InitTypeDef USART_InitStructure;
u8 Uart_Send_Buffer[50]={0};
u8 Uart_Recv_Buffer[50]={0};
unsigned char UARTbusy = FREE ;
unsigned short UartTimeDelay=0;
unsigned char ystxd=0;
unsigned char saveflg = 0;
void SAVE_Flash(void);



void UART_GPIO_CON(void)
{
	/*tx pa.9*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	/*rx pa.10*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	/*485方向位 pa.11*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	RS485_R();
	//RS485_W();

}
void UART_INIT_CON(void)
{
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		USART_ITConfig(USART1, USART_IT_IDLE , ENABLE);

}
void UART_DMA_CON(void)
{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);// DMA时钟
		
		DMA_DeInit(DMA1_Channel4);												//使能DMA1第1个通道
		
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  //与ADC建立连接
		
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart_Send_Buffer;  //UART,SEND内存映射
		
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; ;							//方向为DMA发送uart
		
		DMA_InitStructure.DMA_BufferSize = 30;													//传输单位次数的数据大小

		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//关闭外设递增，该通道只用USART1
		
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//打开内存地址递增
		
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //Byte

		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//Byte
		
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;															//普通模式
		
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;													//高优先级

		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;																//关闭M2M

		DMA_Init(DMA1_Channel4,&DMA_InitStructure);																	//使能DMA

		//DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE); 
		
		DMA_Cmd(DMA1_Channel4, ENABLE);
}

void UART_CON(void)
{
	UART_GPIO_CON();

	//UART_DMA_CON();
	
//	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);			  //使能USART1发送DMA请求 
  
	USART_Cmd(USART1, ENABLE);                                                  //启动串口 

NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);



}




/*************************************************************
*函数名称：void UART_FrameRcv(UartRcvStruct *frame,unsigned char dat)
*函数功能：接收帧验证，串口中断中调用
*参    数：数据帧，数据
*返    回：null
*************************************************************/
void UART_FrameRcv(UartRcvStruct *frame,unsigned char dat)
{
	if(frame->proflag != 0)					    //当前数据正在处理
	{
		return;
	}

	if(UartTimeDelay == 0)						// 超时，退出
	{
		frame->rcvflag = UART_RCVFLAG_HEAD1;
		frame->count = 0;
		frame->chksum = 0;
	}

	switch(frame->rcvflag)
	{
		case UART_RCVFLAG_HEAD1:
			{
				if( dat == UART_FRAME_HEAD1 )
				{
				  frame->rcvflag = UART_RCVFLAG_ADDR;
					frame->chksum = dat;		// 效验
					frame->count = 0;
					UartTimeDelay = UartRcvTimeDly;	// 接收到第一个字节开始计时
				}
				else
				{
					frame->rcvflag =   UART_RCVFLAG_HEAD1;
				}
			 }
		break;
		case UART_RCVFLAG_ADDR:
			{
				frame->addr	= dat;
				frame->chksum = frame->chksum + dat;
				if (frame->addr != MediafilterID)
				{
					frame->rcvflag = UART_RCVFLAG_HEAD1;
				}
				else
				{
					frame->rcvflag = UART_RCVFLAG_LEN; 
				}
			}
		break;
		case UART_RCVFLAG_LEN :
			{
				frame->len = dat;
				if (frame->len > 50 )									
				{
					frame->rcvflag =   UART_RCVFLAG_HEAD1;
				}
				else if (frame->len != 0)
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
				frame->dat[frame->count]=dat;
				frame->chksum = frame->chksum + dat;
				frame->count++;
				if (frame->count + 5 == (frame->len))						//
				{
					frame->rcvflag = UART_RCVFLAG_CS ;
					frame->count = 0;
				}
			}
		break;
		case UART_RCVFLAG_CS:
			{
				frame->cs = dat;
				if (frame->chksum != frame->cs )
				{
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
				if (dat == UART_FRAME_REAR )
				{
					frame->proflag = 1;			//数据帧已经准备好
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




/*************************************************************
*函数名称：void UartFramePro(UartRcvStruct *frame)
*函数功能：接收帧处理
*参    数：数据帧
*返    回：null
*************************************************************/
void UartFramePro(UartRcvStruct *frame)
{

	unsigned char i=0, cs=0;
	unsigned short temp = 0;
	unsigned char dat[50];
	unsigned long DI; 

	if (frame->proflag != 1)			//木有数据啊，桑心返回吧。
	{
		return;
	}
	switch (frame->dat[0])
	{
	case 0x00 :		//		主站请求帧,读取数据
		{
				dat[0] = UART_FRAME_HEAD1;
				dat[1] = 0x00;
				dat[2] = UART_FRAME_HEAD2;
				dat[3] = 0x91;
				dat[4] = 20;
				
				dat[5] = MediafilterID;//DI3,DI2,DI1,DI0
				dat[6] = TP1310;
				dat[7] = ZXFS_MODE;
				dat[8] = 0xff;

				dat[9] = Dataflash.OP>>8;// 读取激光器输出光功率 的AD采集值 真实值 等于 该值 X 3.3 / 256
				dat[10]= Dataflash.OP;

				dat[11]= Dataflash.I>>8;// 读取激光器输出电流值 的AD采集值 真实值 等于 该值 X 3.3 / 256
				dat[12]= Dataflash.I;

				dat[13]= Dataflash.TEMP>>8;// 读取激光器输出温度 的AD采集值 真实值 等于 该值 X 3.3 / 256
				dat[14]= Dataflash.TEMP;

				dat[15]= Dataflash.dBUV>>8;// 读取激光器输出RF电压 的AD采集值 真实值 等于 该值 X 3.3 / 256
				dat[16]= Dataflash.dBUV;

				dat[17]= Dataflash.TEC>>8;
				dat[18]= Dataflash.TEC;
				dat[19]= Dataflash.AGC;		//该模块AGC模式下RF基准表格的偏移值 
				
				
				temp = Dataflash.MGC;		
				temp = temp * 5;	
				dat[20] = temp >> 8;
				dat[21] = temp;				//该模块下MGC的衰减值 该值等于真实衰减值的10倍	 	2字节

				dat[22] = Config.AGC;// 读取当前是AGC 0还是MGC 1
				
				dat[22] |= (Config.laserSWflag<<1);// 读取当前激光器开关状态 1 open 0 clean
				
				dat[23]= Dataflash.Err;

				dat[24]= Config.Ver;
				for(i=0;i<25;i++)
				{
					 	cs += dat[i];
				}
				dat[25]= cs;
				dat[26]= UART_FRAME_REAR;
			//	SendFrame(dat,27);
			*Uart_Send_Buffer = *dat;
		}
	break;
	case 0x12:		//		主站请求帧，读取后续数据
		{
		

		}
	break;
	case 0x01:		//		主站请求帧,写入数据
		{
			
			DI = frame->dat[1];

		
			switch(DI)
			{
				case 0x01:	 	// 设置当前是AGC还是MGC
				{	 
					if (Config.AGC != frame->dat[2])
					{
						Config.AGC = frame->dat[2];
						saveflg = 1;
				}
						
				}
				break;
				case 0x0c :	 	// RF+/RF-
				{	 
					if (Config.AGC == 0x01)		//MGC
					{
					  	 if (frame->dat[2] == 0x10)
						{
							if(Dataflash.MGC>=63)
							{
								Dataflash.MGC = 63;
							}
							else
							{
								Dataflash.MGC++;
							}
						}
						else if (frame -> dat[2] == 0x01)
						{
							if(Dataflash.MGC==0)
							{
							   Dataflash.MGC = 0;
							}
							else
							{
								Dataflash.MGC--;
							}
						}
					}
					else				//AGC
					{
						if (frame->dat[2] == 0x01)
						{
							if(Dataflash.AGC>=86)
							{
								Dataflash.AGC = 86;
							}
							else
							{
								Dataflash.AGC++;
							}
						}
						else if (frame -> dat[2] == 0x10)
						{
							if(Dataflash.AGC==0)
							{
							   Dataflash.AGC = 0;
							}
							else
							{
								Dataflash.AGC--;
							}
						}
					}
					saveflg = 1;
				}
				break;
				case 0x02:						//设置激光器开关
				{
					if (frame->dat[2] != Config.laserSWflag)
					{
						Config.laserSWflag = frame->dat[2];
							if(OPEN == Config.laserSWflag)	//open laser
								{	OP_SW(OP_ON);}
								else					//close laser
							{OP_SW(OP_OFF);}
						saveflg = 1;
					}
					
				}
				break;
				case 0x0d:
				{
					if(frame->dat[2] == 0x10)
					{
						RestorePara();
					}
				}
				break;
			}
		}
	break;
	case 0x13:		//　　	主站请求帧,请求读模块通信地址		(备用)
	{
	;
	}
	break;
	case 0x1a:		//		主站请求帧,初始化模块内存为出厂设置
		{
	//		RestorePara();
		}
	break;
	case 0x1b:		//保存当前配置为出厂配置
		{
//			Config.saveflag 	= INIT_FLAG;			  // 存储标志位yes
//			FLASH_WriteNBytes(ADDR_APPPARA,(unsigned char *)&Config.saveflag,(sizeof( Config ) / sizeof( char )));
//			Delayms(50);
//			FLASH_WriteNBytes(ADDR_APPPARA+5,(unsigned char *)&Dataflash.ID,(sizeof( Dataflash ) / sizeof( char )));
//			Delayms(50);
		}
	break;
	default:
		break;
		}
	frame->proflag = 0;								// 数据处理完成
	//SAVE_Flash();
	}

void SAVE_Flash(void)
{
//	LED(OPEN);
	if (saveflg != 0)
	{
	//	EA =0;
	FLASH_WW(SAVE_FLASH_ADDRESS,(sizeof( Config ) / sizeof( short )),(unsigned short *)&Config.saveflag);
	Delayms(50);
	FLASH_WW(SAVE_FLASH_ADDRESS+10,(sizeof( Dataflash ) / sizeof( short )),(unsigned short *)&Dataflash.ID);
	Delayms(50);
	saveflg = 0;
//	EA = 1;
	}
}
