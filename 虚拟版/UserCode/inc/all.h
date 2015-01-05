#ifndef _ALL_H_
#define _ALL_H_
#include <stdio.h>
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








//#define DataFlashStartAddr 0x0001F000
//#define PageSize 128


#define UartRcvTimeDly 100 //100*5 = 500MS
#define AdcTimeTaskFlg  20 // 100ms
#define Uart1TimeTaskFlg 20// 200ms


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
	unsigned char AC60;								// AC60V,��λ1V���޷���
	unsigned char DC24;								// DC24V,��λ0.1V���޷���
	unsigned char DC5;								// DC5V,��λ0.1V���޷���
	signed char		Temp;								// �����¶ȣ���λ1�棬�з���
	unsigned char ORAop;							// ����ģ��A�����չ⹦�ʣ���λ0.1mW���޷���,offlineʱΪ0x00
	unsigned char ORBop;							// ����ģ��B�����չ⹦�ʣ���λ0.1mW���޷���,offlineʱΪ0x00
	unsigned char OTAop;							// ����ģ��A������⹦�ʣ���λ0.1mW���޷���,offlineʱΪ0x00
	unsigned char OTBop;							// ����ģ��B������⹦�ʣ���λ0.1mW���޷���,offlineʱΪ0x00
	unsigned char OTAim;							// ����ģ��A��������ƫ������λ1mA���޷��ţ�offlineʱΪ0x00
	unsigned char OTBim;							// ����ģ��B��������ƫ������λ1mA���޷��ţ�offlineʱΪ0x00
	unsigned char RF;									// RF�����ƽ����λ1dBV���޷���
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
extern void  FLASH_ReadNByte ( uint32_t addr, unsigned char *dat_buf, unsigned char  no );

extern UartRcvStruct UartRcvFrame;			//����֡
extern MainWStruct MainStructW;
extern MainRStruct MainStructR;
extern AsciiStruct MainAscii;
extern uint32_t UartTimeDelay, AdcTimeTask, Uart1TimeTask;
extern unsigned char indexmk;
extern void TestTX ( void );
extern unsigned char ModuCheckFlg;
void RS485_W ( uint8_t data );
void UartFrameRcv ( UartRcvStruct *frame, unsigned char dat );
unsigned char ModuCheck ( unsigned char num );
unsigned int FourDataAdc ( unsigned int *data );
extern uint32_t ADCtempA[4];
extern uint32_t ADCtempB[4];
extern void Inquire ( unsigned char mode );
extern unsigned char PartNumber[16];// = {'T','W','3','-','1','6'};
extern unsigned char LogicID[16];// = {'T','W','3','-','1','6'};
extern unsigned char VendorName[16];// = {'D','O','N','Y','A','N'};
extern unsigned char SerialNumber[16];//={};
extern void ReadInitPara ( void );
#endif
