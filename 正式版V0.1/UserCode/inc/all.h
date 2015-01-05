#ifndef _ALL_H_
#define _ALL_H_
#include <stdio.h>
#include <math.h> 
#include "initialization.h"
#include "M051Series.h"
#include "string.h"
/*--------------------------------------------------------------------*/
/*һЩԤ����Ķ���													  */
/*--------------------------------------------------------------------*/
#define UartFrameLen 50
#define ModuLen	  50
#define Version 0x01
#define TRUE 1
#define FLASE 0
#define FREE  0
#define PRING  1
#define MainAddr 0

#define FRAME_HEAD		0xf5	// ����֡ͷ1
#define FRAME_DES 		0xfa	// ����֡ͷ2
#define FRAME_END		0xaa	// ����֡β	

#define UART_RCVFLAG_HEAD			0x00		// Header
#define UART_RCVFLAG_DES			0x01	// Descr
#define UART_RCVFLAG_LEN			0x02		// Lenght
#define UART_RCVFLAG_CMD			0x03	// Cmd
#define	UART_RCVFLAG_PAY			0x04	// Payload
#define UART_RCVFLAG_SUM			0x05	// Sum
#define UART_RCVFLAG_END			0x06	// EndNum

#define LD_Add_start					0x00100000
#define LD_Add_end						0x00100fff
#define LD_Pages							8

#define INIT_FLAG							'a'
#define PAGE_SIZE							512
/**/

#define UartRcvTimeDly 100 //100*5 = 500MS
#define AdcTimeTaskFlg  20 // 100ms
#define OPrAgcTaskFlg	10//100ms
#define Uart1TimeTaskFlg 20// 200ms
#define OtherTimeTaskFlg 5

#define LOW 0
#define HIGH 1
/*�������⹦�ʲ����л�*/
#define TxA 1
#define TxB 0
#define TxopSwitchTo(x) P02=(x);
/*���������������л�*/
#define IbA 1
#define IbB 0
#define IbSwitchTo(x) P03=(x);
/*����һ�����л�*/
#define SWC1 P01 
#define SWC2 P00
#define SetA()	P01=0,P00=1		
#define SetB()  P01=1,P00=0
#define SetABOff() SWC1=0;DelayUs(10);SWC2=0;
/*��̬�ſ����л�*/
#define A_LL 0
#define A_HH 1
#define A_HL 2
#define B_LL 3
#define B_HH 4
#define B_HL 5
#define TriSwitchTo(x)      (x)>4?(P40 = 1,P24 = 0):((x)>3?(P40=1,P24=1):((x)>2?(P40=0,P24=0):((x)>1?(P23=1,P22 =0):((x)>0?(P23=1,P22=1):(P22=0,P23=0)))))
#define TriSwitchIn(A0,A1)  ((A0)!=(A1)?(3):((A1)==0?(1):(2)))//1 - LL 2 - HH 3- HL
/*PE4304����ܽ�*/
#define PE4304io_LE1			P33
#define PE4304io_LE2			P34
#define PE4304io_CLOCK		P35
#define PE4304io_DATA			P43
#define PE4304_CLOCK(x)		(x)>0?(PE4304io_CLOCK = 1):(PE4304io_CLOCK = 0)
#define PE4304_DATA(x)		(x)>0?(PE4304io_DATA = 1):(PE4304io_DATA = 0)
/*18B20 ����*/
#define DQio P06
#define DQ_OUT(x)					(x)>0?(DQio = 1):(DQio = 0)
/*AD����ֵ���к�*/
#define AD_OPr1 0
#define AD_OPi 1
#define AD_OPt 2
#define AD_OPr2 3
#define AD_Pow5	4
#define AD_Pow24 5
#define AD_AC60 6
#define AD_RF 7
/*--------------------------------------------------------------------*/
/*�ṹ��Ķ���																											  */
/*--------------------------------------------------------------------*/
typedef struct
{
    unsigned char rcvflag;				// ����״̬��־
    unsigned char count;				// ������
    unsigned char cmd;				// CMD
    unsigned char len;					// �����򳤶�
    unsigned char dat[UartFrameLen];	// ���ݴ洢��
    unsigned char chksum;				// Ч�������
    unsigned char cs;					// Ч��λ
    unsigned char ProFlag;				// �����־
} UartRcvStruct;

typedef struct
{
		unsigned char PartNumber[16];					
    unsigned char LogicID[16];				// ID���� ���������д
    unsigned char VendorName[16];				// Vender Name ���� ���������д 16�ֽ�ASCII��
		unsigned char SerialNumber[16];//Serial Number ���� ���������д 16�ֽ�ASCII��
}AsciiStruct;
typedef struct
{
    unsigned char Save_flag;
    unsigned char AB_Switch;					//  AB���أ�Cmd �� 0x01��	0x01��ǿ���л�A 0x02:B 0x03 A���� 0x04 B����
    signed char AB_Switch_door; 		//  AB�л�����ֵ��Cmd: 0x02�� ��Χ ��-6 ~ +3 ����λ dBm ��
		unsigned char Port12;						//  �˿�1+2����̬�ţ�Cmd:0x03��ֵ��0x01 ��˥�� 0x02 ��˥�� 0x03 ��˥��
		unsigned char Port34;						//  �˿�3+4����̬�ţ�Cmd:0x04��ֵ��0x01 ��˥�� 0x02 ��˥�� 0x03 ��˥��
		unsigned char AGCa;							//  ����A��AGCƫ��ֵ ��Cmd:0x05��(��Χ-5~+5,��λ dBm )
		unsigned char AGCb;							//  ����B��AGCƫ��ֵ ��Cmd:0x06��(��Χ-5~+5,��λ dBm )
		unsigned char RF;								//	RF�����ƽУ����Cmd:0x07��(��Χ -10~+10,��λ dBuV)
/********************************************************************************************************************/
} MainWStruct;

typedef struct
{
	unsigned char online;							// Bit0: ����ģ��A�Ƿ����ߣ�0��offline 1��online
																		// Bit1: ����ģ��B�Ƿ����ߣ�0��offline 1��online
																		// Bit2: ����ģ��A�Ƿ����ߣ�0��offline 1��online
																		// Bit3: ����ģ��B�Ƿ����ߣ�0��offline 1��online
	unsigned short AC60;								// AC60V,��λ1V���޷���
	unsigned short DC24;								// DC24V,��λ0.1V���޷���
	unsigned short DC5;								// DC5V,��λ0.1V���޷���
	signed short	Ce;								  // �����¶ȣ���λ1�棬�з���
	unsigned short ORAop;							// ����ģ��A�����չ⹦�ʣ���λ0.1mW���޷���,offlineʱΪ0x00
	unsigned short ORBop;							// ����ģ��B�����չ⹦�ʣ���λ0.1mW���޷���,offlineʱΪ0x00
	unsigned short OTAop;							// ����ģ��A������⹦�ʣ���λ0.1mW���޷���,offlineʱΪ0x00
	unsigned short OTBop;							// ����ģ��B������⹦�ʣ���λ0.1mW���޷���,offlineʱΪ0x00
	unsigned short OTAim;							// ����ģ��A��������ƫ������λ1mA���޷��ţ�offlineʱΪ0x00
	unsigned short OTBim;							// ����ģ��B��������ƫ������λ1mA���޷��ţ�offlineʱΪ0x00
	unsigned short RF;									// RF�����ƽ����λ1dBV���޷���
	unsigned char AB;									// AB״̬		0x01 A,0x02 B
	unsigned char Box;								// ���ǿ��� 0x01 �رգ�0x02 ��
	unsigned char Warning;						// �澯��bit0 : ����ģ��A����⹦�ʱ�����0��normal 1:alarm,offlineʱΪ0
																		//			 bit1 : ����ģ��B����⹦�ʱ�����0��normal 1:alarm,offlineʱΪ0
																		//			 bit2 :����ģ��A�������⹦�ʱ�����0��normal 1:alarm,offlineʱΪ0
																		//			 bit3 :����ģ��B�������⹦�ʱ�����0��normal 1:alarm,offlineʱΪ0
																		//			 bit4 :����ģ��A������ƫ��������0��normal 1:alarm,offlineʱΪ0
																		//			 bit5 :����ģ��B������ƫ��������0��normal 1:alarm,offlineʱΪ0
																		//			 bit6 :RF�����ƽ������0��normal 1:alarm,offlineʱΪ0
} MainRStruct;

/*--------------------------------------------------------------------*/
/*�ⲿ���õĺ����Լ�����											  */
/*--------------------------------------------------------------------*/

extern int32_t RS485_Send ( uint8_t *databuf, uint32_t len );
extern uint32_t I2CountIndex;
extern uint32_t g_I2Cupdate;
extern uint32_t I2Cchanged;
extern void DelayUs ( uint32_t us );
extern void DelayMs ( uint16_t ms );
extern void DelayS ( uint16_t s );
extern void UartFramePro ( UartRcvStruct *frame );
extern void Uart1Task ( void );
extern void FlashTask ( void );
extern int FLASH_WriteNBytes ( uint32_t addr, unsigned char  *dat_buf, unsigned char no );
extern void AdcTask ( void );
extern void OPR_AGC_Task(void);
extern void  FLASH_ReadNByte ( uint32_t addr, unsigned char *dat_buf, unsigned char  no );
extern void ControlInit(void);
extern void PE4304Send ( unsigned char num , unsigned char Dat );
extern void ReadCelsius(void);
extern void TriSwitchTask(void);
extern void CheckMode(void);

extern UartRcvStruct UartRcvFrame;			//����֡
extern MainWStruct MainStructW;
extern MainRStruct MainStructR;
extern AsciiStruct MainAscii;
extern uint32_t UartTimeDelay, AdcTimeTask, Uart1TimeTask,OPrTimeTask,OtherTimeTask;
extern unsigned char indexmk;
extern void TestTX ( void );
extern unsigned char ModuCheckFlg;
extern unsigned char CelsiusComp(void);
void RS485_W ( uint8_t data );
void UartFrameRcv ( UartRcvStruct *frame, unsigned char dat );
unsigned char ModuCheck ( unsigned char num );
unsigned int FourDataAdc ( unsigned int *data );
extern unsigned int ADCcovVal[10];
extern void Inquire ( unsigned char mode );
extern unsigned char PartNumber[16];// = {'T','W','3','-','1','6'};
extern unsigned char LogicID[16];// = {'T','W','3','-','1','6'};
extern unsigned char VendorName[16];// = {'D','O','N','Y','A','N'};
extern unsigned char SerialNumber[16];//={};
extern void ReadInitPara ( void );
#endif
