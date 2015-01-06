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

	/*485����λ pa.11*/
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
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);// DMAʱ��
		
		DMA_DeInit(DMA1_Channel4);												//ʹ��DMA1��1��ͨ��
		
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  //��ADC��������
		
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart_Send_Buffer;  //UART,SEND�ڴ�ӳ��
		
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; ;							//����ΪDMA����uart
		
		DMA_InitStructure.DMA_BufferSize = 30;													//���䵥λ���������ݴ�С

		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�ر������������ͨ��ֻ��USART1
		
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//���ڴ��ַ����
		
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //Byte

		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//Byte
		
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;															//��ͨģʽ
		
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;													//�����ȼ�

		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;																//�ر�M2M

		DMA_Init(DMA1_Channel4,&DMA_InitStructure);																	//ʹ��DMA

		//DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE); 
		
		DMA_Cmd(DMA1_Channel4, ENABLE);
}

void UART_CON(void)
{
	UART_GPIO_CON();

	//UART_DMA_CON();
	
//	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);			  //ʹ��USART1����DMA���� 
  
	USART_Cmd(USART1, ENABLE);                                                  //�������� 

NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);



}




/*************************************************************
*�������ƣ�void UART_FrameRcv(UartRcvStruct *frame,unsigned char dat)
*�������ܣ�����֡��֤�������ж��е���
*��    ��������֡������
*��    �أ�null
*************************************************************/
void UART_FrameRcv(UartRcvStruct *frame,unsigned char dat)
{
	if(frame->proflag != 0)					    //��ǰ�������ڴ���
	{
		return;
	}

	if(UartTimeDelay == 0)						// ��ʱ���˳�
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
					frame->chksum = dat;		// Ч��
					frame->count = 0;
					UartTimeDelay = UartRcvTimeDly;	// ���յ���һ���ֽڿ�ʼ��ʱ
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
					frame->chksum = frame->chksum + dat;							// Ч��
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
					frame->proflag = 1;			//����֡�Ѿ�׼����
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
*�������ƣ�void UartFramePro(UartRcvStruct *frame)
*�������ܣ�����֡����
*��    ��������֡
*��    �أ�null
*************************************************************/
void UartFramePro(UartRcvStruct *frame)
{

	unsigned char i=0, cs=0;
	unsigned short temp = 0;
	unsigned char dat[50];
	unsigned long DI; 

	if (frame->proflag != 1)			//ľ�����ݰ���ɣ�ķ��ذɡ�
	{
		return;
	}
	switch (frame->dat[0])
	{
	case 0x00 :		//		��վ����֡,��ȡ����
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

				dat[9] = Dataflash.OP>>8;// ��ȡ����������⹦�� ��AD�ɼ�ֵ ��ʵֵ ���� ��ֵ X 3.3 / 256
				dat[10]= Dataflash.OP;

				dat[11]= Dataflash.I>>8;// ��ȡ�������������ֵ ��AD�ɼ�ֵ ��ʵֵ ���� ��ֵ X 3.3 / 256
				dat[12]= Dataflash.I;

				dat[13]= Dataflash.TEMP>>8;// ��ȡ����������¶� ��AD�ɼ�ֵ ��ʵֵ ���� ��ֵ X 3.3 / 256
				dat[14]= Dataflash.TEMP;

				dat[15]= Dataflash.dBUV>>8;// ��ȡ���������RF��ѹ ��AD�ɼ�ֵ ��ʵֵ ���� ��ֵ X 3.3 / 256
				dat[16]= Dataflash.dBUV;

				dat[17]= Dataflash.TEC>>8;
				dat[18]= Dataflash.TEC;
				dat[19]= Dataflash.AGC;		//��ģ��AGCģʽ��RF��׼����ƫ��ֵ 
				
				
				temp = Dataflash.MGC;		
				temp = temp * 5;	
				dat[20] = temp >> 8;
				dat[21] = temp;				//��ģ����MGC��˥��ֵ ��ֵ������ʵ˥��ֵ��10��	 	2�ֽ�

				dat[22] = Config.AGC;// ��ȡ��ǰ��AGC 0����MGC 1
				
				dat[22] |= (Config.laserSWflag<<1);// ��ȡ��ǰ����������״̬ 1 open 0 clean
				
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
	case 0x12:		//		��վ����֡����ȡ��������
		{
		

		}
	break;
	case 0x01:		//		��վ����֡,д������
		{
			
			DI = frame->dat[1];

		
			switch(DI)
			{
				case 0x01:	 	// ���õ�ǰ��AGC����MGC
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
				case 0x02:						//���ü���������
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
	case 0x13:		//����	��վ����֡,�����ģ��ͨ�ŵ�ַ		(����)
	{
	;
	}
	break;
	case 0x1a:		//		��վ����֡,��ʼ��ģ���ڴ�Ϊ��������
		{
	//		RestorePara();
		}
	break;
	case 0x1b:		//���浱ǰ����Ϊ��������
		{
//			Config.saveflag 	= INIT_FLAG;			  // �洢��־λyes
//			FLASH_WriteNBytes(ADDR_APPPARA,(unsigned char *)&Config.saveflag,(sizeof( Config ) / sizeof( char )));
//			Delayms(50);
//			FLASH_WriteNBytes(ADDR_APPPARA+5,(unsigned char *)&Dataflash.ID,(sizeof( Dataflash ) / sizeof( char )));
//			Delayms(50);
		}
	break;
	default:
		break;
		}
	frame->proflag = 0;								// ���ݴ������
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
