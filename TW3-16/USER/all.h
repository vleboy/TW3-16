#ifndef _ALL_H_
#define _ALL_H_
#include "stm32f10x.h"
#include <stdio.h>
#include "adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_adc.h"


/*--------------------------------------------------------------------*/
/*一些预定义的东西													  */
/*--------------------------------------------------------------------*/
#define SAVE_FLASH_ADDRESS 0x84000fff
#define SYSTICK_TIME_FLAG 100
#define UartFrameLen 50
#define ModuLen	  50
#define Version 0x01
#define TRUE 1
#define FLASE 0
#define FREE  0
#define PRING  1
#define MainAddr 0
#define UART_FRAME_HEAD1		0xf5	// 定义帧头1
#define UART_FRAME_HEAD2		0xfa	// 定义帧头2
#define UART_FRAME_REAR			0xaa	// 定义帧尾	
#define UartRcvTimeDly 100 //100*5 = 500MS
#define AdcTimeTaskFlg  20 // 100ms
#define OPrAgcTaskFlg	10//100ms
#define Uart1TimeTaskFlg 20// 200ms
#define OtherTimeTaskFlg 5
#define LOW 0
#define HIGH 1
#define INIT_FLAG 			'a'
#define OPEN 1				 	
#define CLOSE 0
#define OP_ON 1
#define OP_OFF 0
#define LOW 0
#define HIGH 1
#define PAGE_SIZE 512
#define OP_SW(x) (x)>0?(GPIO_SetBits(GPIOD,GPIO_Pin_1)):(GPIO_ResetBits(GPIOD,GPIO_Pin_1))
#define RS485_R()  GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define RS485_W()  GPIO_SetBits(GPIOA,GPIO_Pin_11)
/*激光器光功率采样切换*/
#define TxA 1
#define TxB 0
#define TxopSwitchTo(x) (x)>0?(GPIO_SetBits(GPIOA,GPIO_Pin_2)):(GPIO_ResetBits(GPIOA,GPIO_Pin_2));
/*激光器电流采样切换*/
#define IbA 1
#define IbB 0
#define IbSwitchTo(x) (x)>0?(GPIO_SetBits(GPIOA,GPIO_Pin_2)):(GPIO_ResetBits(GPIOA,GPIO_Pin_2));
/*二切一开关切换*/
#define SWC1 P01 
#define SWC2 P00
#define SetA()	P01=0,P00=1		
#define SetB()  P01=1,P00=0
#define SetABOff() SWC1=0;DelayUs(10);SWC2=0;
/*三态门开关切换*/
#define A_LL 0
#define A_HH 1
#define A_HL 2
#define B_LL 3
#define B_HH 4
#define B_HL 5
#define TriSwitchTo(x)      (x)>4?(P40 = 1,P24 = 0):((x)>3?(P40=1,P24=1):((x)>2?(P40=0,P24=0):((x)>1?(P23=1,P22 =0):((x)>0?(P23=1,P22=1):(P22=0,P23=0)))))
#define TriSwitchIn(A0,A1)  ((A0)!=(A1)?(3):((A1)==0?(1):(2)))//1 - LL 2 - HH 3- HL
/*PE4304定义管脚*/
#define PE4304io_LE1_ON			P33
#define PE4304io_LE1_OFF			P33
#define PE4304io_LE2_ON			P34
#define PE4304io_LE2_OFF			P34
#define PE4304io_CLOCK_SET_H		P35
#define PE4304io_CLOCK_SET_L		P35
#define PE4304io_DATA_SET_H			P43
#define PE4304io_DATA_SET_L			P43
#define PE4304_CLOCK(x)		(x)>0?(PE4304io_CLOCK_SET_H):(PE4304io_CLOCK_SET_L)
#define PE4304_DATA(x)		(x)>0?(PE4304io_DATA_SET_H):(PE4304io_DATA_SET_L)
/*18B20 定义*/
#define DQio P06
#define DQ_OUT(x)					(x)>0?(DQio = 1):(DQio = 0)
/*AD采样值序列号*/
#define AD_OPr1 0
#define AD_OPi 1
#define AD_OPt 2
#define AD_OPr2 3
#define AD_Pow5	4
#define AD_Pow24 5
#define AD_AC60 6
#define AD_RF 7




#define UART_RCVFLAG_HEAD			0x00		// Header
#define UART_RCVFLAG_DES			0x01	// Descr
#define UART_RCVFLAG_LEN			0x02		// Lenght
#define UART_RCVFLAG_CMD			0x03	// Cmd
#define	UART_RCVFLAG_PAY			0x04	// Payload
#define UART_RCVFLAG_SUM			0x05	// Sum
#define UART_RCVFLAG_END			0x06	// EndNum

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
} MainWStruct;

typedef struct
{
	unsigned char online;							// Bit0: 接收模块A是否在线，0：offline 1：online
																		// Bit1: 接收模块B是否在线，0：offline 1：online
																		// Bit2: 发射模块A是否在线，0：offline 1：online
																		// Bit3: 发射模块B是否在线，0：offline 1：online
	unsigned short AC60;								// AC60V,单位1V，无符号
	unsigned short DC24;								// DC24V,单位0.1V，无符号
	unsigned short DC5;								// DC5V,单位0.1V，无符号
	signed short	Ce;								  // 机内温度，单位1℃，有符号
	unsigned short ORAop;							// 接收模块A，接收光功率，单位0.1mW，无符号,offline时为0x00
	unsigned short ORBop;							// 接收模块B，接收光功率，单位0.1mW，无符号,offline时为0x00
	unsigned short OTAop;							// 发射模块A，发射光功率，单位0.1mW，无符号,offline时为0x00
	unsigned short OTBop;							// 发射模块B，发射光功率，单位0.1mW，无符号,offline时为0x00
	unsigned short OTAim;							// 发射模块A，激光器偏流，单位1mA，无符号，offline时为0x00
	unsigned short OTBim;							// 发射模块B，激光器偏流，单位1mA，无符号，offline时为0x00
	unsigned short RF;									// RF输出电平，单位1dBV，无符号
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

extern MainWStruct MainStructW;
extern MainRStruct MainStructR;
extern AsciiStruct MainAscii;
extern unsigned char PartNumber[16];
extern unsigned char LogicID[16] ;
extern unsigned char VendorName[16];
extern unsigned char SerialNumber[16];
extern GPIO_InitTypeDef  GPIO_InitStructure;
//extern  UartRcvStruct UartRcvFrame;			//定义帧

//extern u16 SYSTICK_TIME;
//extern unsigned char MediafilterID;			
//extern  unsigned short UartTimeDelay;
void System_Init(void);
void RCC_CON(void);
void CONTROL_CON(void);
//void UART_CON(void);
//void ADC_INIT(void);
void SYSTICK_CON(void);
void ReadInitPara(void);
void Delayms(u32 ms);
void Delayus(u32 us);
//void ReadInitPara(void);
//void RestorePara(void);
//void UartFramePro(UartRcvStruct *frame);
//void UART_FrameRcv(UartRcvStruct *frame,unsigned char dat);
//void ADCtask(void);
//extern u16 ADC_RUN(unsigned char ch);
//extern u16 ADC_ConvertedValue[40];
//extern unsigned short AGC_Task_Time; //100MS = 5*20
//extern unsigned short ADCtask_Time; //300ms = 5 * 60
//extern GPIO_InitTypeDef  GPIO_InitStructure; 
//extern void ATTControl(u8 att);
//extern void AGC_Task(void);
//extern void FLASH_WW(u32 Add,u32 len,u16 *data);
//extern void FLASH_RR(u32 Add,u32 len ,u16 *data);
#endif
