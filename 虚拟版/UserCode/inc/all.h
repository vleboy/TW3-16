#ifndef _ALL_H_
#define _ALL_H_
#include <stdio.h>
#include "initialization.h"
#include "M051Series.h"
#include "string.h"
/*--------------------------------------------------------------------*/
/*一些预定义的东西													  */
/*--------------------------------------------------------------------*/
#define UartFrameLen 50
#define ModuLen	  50
#define Version 0x01
#define TRUE 1
#define FLASE 0
#define FREE  0
#define PRING  1
#define MainAddr 0

#define FRAME_HEAD		0xf5	// 定义帧头1
#define FRAME_DES 		0xfa	// 定义帧头2
#define FRAME_END		0xaa	// 定义帧尾	

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
/*结构体的定义																											  */
/*--------------------------------------------------------------------*/
typedef struct
{
    unsigned char rcvflag;				// 接收状态标志
    unsigned char count;				// 计数用
    unsigned char cmd;				// CMD
    unsigned char len;					// 数据域长度
    unsigned char dat[UartFrameLen];	// 数据存储区
    unsigned char chksum;				// 效验计算用
    unsigned char cs;					// 效验位
    unsigned char ProFlag;				// 处理标志
} UartRcvStruct;

typedef struct
{
		unsigned char PartNumber[16];					
    unsigned char LogicID[16];				// ID数组 从左→右填写
    unsigned char VendorName[16];				// Vender Name 数组 从左→右填写 16字节ASCII码
		unsigned char SerialNumber[16];//Serial Number 数组 从左→右填写 16字节ASCII码
}AsciiStruct;
typedef struct
{
    unsigned char Save_flag;
    unsigned char AB_Switch;					//  AB开关（Cmd ： 0x01）	0x01：强制切换A 0x02:B 0x03 A优先 0x04 B优先
    signed char AB_Switch_door; 		//  AB切换门限值（Cmd: 0x02） 范围 （-6 ~ +3 ，单位 dBm ）
		unsigned char Port12;						//  端口1+2，三态门（Cmd:0x03）值：0x01 无衰减 0x02 低衰减 0x03 高衰减
		unsigned char Port34;						//  端口3+4，三态门（Cmd:0x04）值：0x01 无衰减 0x02 低衰减 0x03 高衰减
		unsigned char AGCa;							//  光收A光AGC偏移值 （Cmd:0x05）(范围-5~+5,单位 dBm )
		unsigned char AGCb;							//  光收B光AGC偏移值 （Cmd:0x06）(范围-5~+5,单位 dBm )
		unsigned char RF;								//	RF输出电平校正（Cmd:0x07）(范围 -10~+10,单位 dBuV)
/********************************************************************************************************************/
} MainWStruct;

typedef struct
{
	unsigned char online;							// Bit0: 接收模块A是否在线，0：offline 1：online
																		// Bit1: 接收模块B是否在线，0：offline 1：online
																		// Bit2: 发射模块A是否在线，0：offline 1：online
																		// Bit3: 发射模块B是否在线，0：offline 1：online
	unsigned char AC60;								// AC60V,单位1V，无符号
	unsigned char DC24;								// DC24V,单位0.1V，无符号
	unsigned char DC5;								// DC5V,单位0.1V，无符号
	signed char		Temp;								// 机内温度，单位1℃，有符号
	unsigned char ORAop;							// 接收模块A，接收光功率，单位0.1mW，无符号,offline时为0x00
	unsigned char ORBop;							// 接收模块B，接收光功率，单位0.1mW，无符号,offline时为0x00
	unsigned char OTAop;							// 发射模块A，发射光功率，单位0.1mW，无符号,offline时为0x00
	unsigned char OTBop;							// 发射模块B，发射光功率，单位0.1mW，无符号,offline时为0x00
	unsigned char OTAim;							// 发射模块A，激光器偏流，单位1mA，无符号，offline时为0x00
	unsigned char OTBim;							// 发射模块B，激光器偏流，单位1mA，无符号，offline时为0x00
	unsigned char RF;									// RF输出电平，单位1dBV，无符号
	unsigned char AB;									// AB状态		0x01 A,0x02 B
	unsigned char Box;								// 开盖开关 0x01 关闭，0x02 打开
	unsigned char Warning;						// 告警：bit0 : 接收模块A输入光功率报警，0：normal 1:alarm,offline时为0
																		//			 bit1 : 接收模块B输入光功率报警，0：normal 1:alarm,offline时为0
																		//			 bit2 :发射模块A激光器光功率报警，0：normal 1:alarm,offline时为0
																		//			 bit3 :发射模块B激光器光功率报警，0：normal 1:alarm,offline时为0
																		//			 bit4 :发射模块A激光器偏流报警，0：normal 1:alarm,offline时为0
																		//			 bit5 :发射模块B激光器偏流报警，0：normal 1:alarm,offline时为0
																		//			 bit6 :RF输出电平报警，0：normal 1:alarm,offline时为0
} MainRStruct;

/*--------------------------------------------------------------------*/
/*外部调用的函数以及变量											  */
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

extern UartRcvStruct UartRcvFrame;			//定义帧
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
