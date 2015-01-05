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

#define TRUE 1
#define FLASE 0
#define FREE  0
#define PRING  1
#define MainAddr 0

#define FRAME_HEAD1		0xf5	// 定义帧头1
#define FRAME_HEAD2		0xfa	// 定义帧头2
#define FRAME_REAR		0xaa	// 定义帧尾	

#define UART_RCVFLAG_HEAD1			0x00		// 头1
#define UART_RCVFLAG_ADDR			0x01	// 地址
#define UART_RCVFLAG_HEAD2			0x02		// 头2
#define UART_RCVFLAG_CTL			0x03	// 控制码
#define	UART_RCVFLAG_LEN			0x04	// 长度
#define UART_RCVFLAG_DAT			0x05	// 数据
#define UART_RCVFLAG_CS				0x06	// 效验
#define UART_RCVFLAG_REAR			0x07	// 尾

#define I2C_RCVFLAG_HEAD1		0x00	// 头1
#define I2C_RCVFLAG_MODENUM		0x01	// 模块号
#define	I2C_RCVFLAG_LEN			0x02	// 长度
#define I2C_RCVFLAG_DAT			0x03	// 数据
#define I2C_RCVFLAG_CS			0x04	// 效验
#define I2C_RCVFLAG_REAR		0x05	// 尾

#define ZXFS_MODU 0x01						//正向发射模块
#define ZXJS_MODU 0x02
#define FXJS_MODU 0x03
#define EDFA_MODU 0x04
#define TP1310 0x01								//1310型号
#define TP1550 0x02
#define TPMA   0x00								//主模块
/**/
#define ModuAddr1	0X01								//插槽号1（地址）
#define ModuAddr2	0X02
#define ModuAddr3	0X03
#define ModuAddr4	0X04
#define ModuAddr5	0X05
#define ModuAddr6	0X06
#define ModuAddr7	0X07
#define ModuAddr8	0X08
#define ModuAddr9	0X09
#define ModuAddrA	0X0A







//#define DataFlashStartAddr 0x0001F000
//#define PageSize 128


#define UartRcvTimeDly 100 //100*5 = 500MS
#define AdcTimeTaskFlg  20 // 100ms
#define Uart1TimeTaskFlg 20// 200ms
#define I2cTimeTaskFlg 400  //250ms
/*--------------------------------------------------------------------*/
//							IIC
//
// Status vector - top 4 bits only
#define  WRITE        (0x00)               // I2C WRITE command
#define  READ          (0x01)               // I2C READ command

#define  SlaAddr    (0x02)               // Device addresses (7 bits, lsb is a don't care)


/* 从发送器模式 */
#define  I2C_ST1      (0xA8)               // (SR) 	  已接收自身SLA+R,已返回ACK
#define  I2C_ST2      (0xB0)               // (SR) 	  主控器时在SLA+R/W 中丢失仲裁； 已接收自身SLA+R，已返回ACK
#define  I2C_ST3      (0xB8)               // (SR) 	  已发送I2DAT中数据字节；已接收ACK
#define  I2C_ST4      (0xC0)               // (ST)	  已发送I2DAT 中数据字节；已接收非ACK
#define  I2C_ST5      (0xC8)               // (ST)	  已发送I2DAT 中最后的数据字节（AA=0）;已接收ACK
/* 从接收器模式 */
#define  I2C_SR1      (0x60)               // (SR)   已接收自身SLA+W; 已接收ACK
#define  I2C_SR2      (0x68)               // (SR)	  主控器时在SLA+R /W 中丢失仲裁;已接收自身SLA+W,已返回ACK
#define  I2C_SR3      (0x70)               // (SR)	  已接收通用调用地址(00H);已返回ACK
#define  I2C_SR4      (0x78)               // (SR)	  主控器时在SLA+R/W 中丢失仲裁; 已接收通用调用地址;已返回ACK
#define  I2C_SR5      (0x80)               // (SR)	  前一次寻址使用自身从地址；已接收数据字节；已返回ACK
#define  I2C_SR6      (0x88)               // (SR)	  前一次寻址使用自身SLA 地址；已接收数据字节； 已返回非ACK
#define  I2C_SR7      (0x90)               // (SR)	  前一次寻址使用通用调用；已接收数据字节；已返回ACK
#define  I2C_SR8      (0x98)               // (SR)   前一次寻址使用通用调用；已接收数据字节；已返回非ACK
#define  I2C_SR9      (0xA0)               // (SR)	  当使用SLA/REC 或SLA/TRX 静态寻址时,接收到停止条件或重复的起始条件

/*--------------------------------------------------------------------*/
#define I2cSwitchModu(x) x==0x01?Modu1:x==0x02?Modu2:x==0x03?Modu3:x==0x04?Modu4:x==0x05?Modu5:x==0x06?Modu6:x==0x07?Modu7:x==0x08?Modu8:x==0x09?Modu9:x==0x0a?ModuA:ModuA

/*--------------------------------------------------------------------*/
/*结构体的定义																											  */
/*--------------------------------------------------------------------*/
typedef struct
{
    unsigned char rcvflag;				// 接收状态标志
    unsigned char count;				// 计数用
    unsigned char addr;					// 地址
    unsigned char ctlcode;				// 控制码
    unsigned char len;					// 数据域长度
    unsigned char dat[UartFrameLen];	// 数据存储区
    unsigned char chksum;				// 效验计算用
    unsigned char cs;					// 效验位
    unsigned char ProFlag;				// 处理标志
} UartRcvStruct;

typedef struct
{
    unsigned char Online;
    unsigned char Type;					//1310or1550;
    unsigned char ModuType;				// 模块类型
    unsigned char DataLen;
    unsigned char ModuData[ModuLen];	// 模块数据
} ModuLenNumStruct;
typedef struct
{

    unsigned char Type;					// VHE01 OR VHE02
    unsigned short POWERA;				// POWERA
    unsigned short POWERB;				// POWERB
    unsigned short Inserted;
    unsigned short SysConfigBuf[128];
} MainStruct;

typedef struct
{
    unsigned char ModuNum;				// 模块号码
    unsigned char Len;					// 数据域长度
    unsigned char dat[UartFrameLen];	// 数据存储区
    unsigned char chksum;				// 效验计算用
    unsigned char cs;					// 效验位
    unsigned char ProFlag;				// 处理标志
} I2CRcvStruct;


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
extern void I2cTask ( I2CRcvStruct *frame );
extern void AdcTask ( void );

extern void I2cFramePro ( void );
extern UartRcvStruct UartRcvFrame;			//定义帧
extern ModuLenNumStruct Modu1, Modu2, Modu3, Modu4, Modu5, Modu6, Modu7, Modu8, Modu9, ModuA, ModuTemp;
extern MainStruct MainModu;
extern uint32_t UartTimeDelay, AdcTimeTask, Uart1TimeTask, I2cTimeTask;
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
#endif
