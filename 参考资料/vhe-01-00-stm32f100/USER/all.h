#ifndef _ALL_H_
#define _ALL_H_
#include "stm32f10x.h"
#include <stdio.h>
#include "adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_adc.h"
#define SAVE_FLASH_ADDRESS 0x84000fff
#define SYSTICK_TIME_FLAG 100
//版本号定义
#define Version				0x10			// 版本号
// 默认参数
#define AGC_RF_DEFAULT 39//0x2B 840 mV
#define MGC_ATT_DEFAULT		20	//10 dB
#define MGC_ATT_DEFAULT2	20	//10 dB
//配置参数
#define AGC_ON_MGC_OFF 		0
#define AGC_OFF_MGC_ON 		1
#define LASER_OPEN			0
#define LASER_CLOSE			1
#define BUZZER_OPEN			0
#define BUZZER_CLOSE		1
#define INIT_FLAG 			'a'
#define OPEN 1				 	
#define CLOSE 0
#define OP_ON 1
#define OP_OFF 0
#define flg_AGC 0
#define flg_MGC 1
#define LED(x) (x)>0?(GPIO_SetBits(GPIOB,GPIO_Pin_9)):(GPIO_ResetBits(GPIOB,GPIO_Pin_9))
#define OP_SW(x) (x)>0?(GPIO_SetBits(GPIOD,GPIO_Pin_1)):(GPIO_ResetBits(GPIOD,GPIO_Pin_1))
#define RS485_R()  GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define RS485_W()  GPIO_SetBits(GPIOA,GPIO_Pin_11)
#define AGC_TIME_FLG 20
#define ADC_TIME_FLG 60
#define WDT_TIME_FLG 100 
#define FREE 0//485定义
#define BUSY 1
#define UartRcvTimeDly 		100			//串口延迟函数值 100*5 ms
#define UART_FRAME_HEAD1		0xf5	// 定义帧头1
#define UART_FRAME_HEAD2		0xfa	// 定义帧头2
#define UART_FRAME_REAR			0xaa	// 定义帧尾	

#define UART_RCVFLAG_HEAD1		0x00	// 头1
#define UART_RCVFLAG_ADDR			0x01	// 地址
#define UART_RCVFLAG_HEAD2		0x02	// 头2
#define UART_RCVFLAG_CTL			0x03	// 控制码
#define	UART_RCVFLAG_LEN			0x04	// 长度
#define UART_RCVFLAG_DAT			0x05	// 数据
#define UART_RCVFLAG_CS				0x06	// 效验
#define UART_RCVFLAG_REAR			0x07	// 尾

#define ADC_ID 0
#define ADC_OP 1
#define ADC_TEM 2
#define ADC_IB 3
#define ADC_RF 4

#define ZXFS_MODE 0x01
#define ZXJS_MODE 0X02
#define FXJS_MODE 0x03
#define EDFA_MODE 0X04

#define TP1310 0x01
#define TP1550 0x02
#define TPMA   0x00

#define LASER_OP_LOW		0x17//电压<=0.3V，则报警
#define LASER_IA_LOW		0x17//0x2E//30ma-115ma为正常范围，超过范围则报警
#define LASER_IA_HIGH		0x59//0xB1//0x2e-30ma 0x -115ma 1V/50mA    0.2V/10mA
#define LASER_TEMP_LOW		0x4D//1.25V/25℃   0.05V/℃ 20---27℃, 为正常范围，超出范围则报警
#define LASER_TEMP_HIGH		0x68//
#define LASER_RF_LOW		0x32//60%-110%为正常范围，超出范围则报警
#define LASER_RF_HIGH		0x46//110%
#define AGC_RFVOL_HIGH		42//AGC 时RF的电压最大值索引
#define AGC_RFVOL_LOW		36//AGC 时RF的电压最小值索引

#define AGC_ATT_HIGH		63//AGC 时衰减器的最大值
#define AGC_ATT_LOW			0  //AGC 时衰减器的最小值

#define MGC_ATT_HIGH		63//MGC 时衰减器的最大值
#define MGC_ATT_LOW			0  //MGC 时衰减器的最小值
//定义结构体
typedef struct{
	unsigned char saveflag;				// 存储标志，
	unsigned char AGC;					// 宽带模拟AGC/MGC选择状态
	unsigned char laserSWflag;			// 激光器开关状态
	unsigned char Ver;
	}ConfigStruct;
typedef struct{
	unsigned long ID;						// 存储设备序列号			 4
	unsigned short OP;						// 当前输出光功率			 2
	unsigned short I;						// 激光器工作电流			 2
	unsigned short TEMP;					// 激光器管芯温度			 2
	unsigned short dBUV;					// 激光器驱动RF电平 电压基准？2	
	unsigned short TEC;
	unsigned char AGC;						// 模拟宽带AGC的衰减量偏移值  1
	unsigned char MGC;						// 宽带模拟MGC的衰减量  
	unsigned char Err;						// 参数异常状态字			  1
}DataStruct;
typedef struct{
	unsigned char rcvflag;				// 接收状态标志
	unsigned char count;				// 计数用
	unsigned char addr;					// 地址
	unsigned char ctlcode;				// 控制码
	unsigned char len;					// 数据域长度
	unsigned char dat[50];	// 数据存储区
	unsigned char chksum;				// 效验计算用
	unsigned char cs;					// 效验位
	unsigned char proflag;				// 处理标志
}UartRcvStruct;
extern  UartRcvStruct UartRcvFrame;			//定义帧
extern ConfigStruct Config;
extern DataStruct Dataflash;
extern u16 SYSTICK_TIME;
extern unsigned char MediafilterID;			
extern  unsigned short UartTimeDelay;
void SYSTEM_INIT(void);
void RCC_CON(void);
void CONTROL_CON(void);
void UART_CON(void);
void ADC_INIT(void);
void SYSTICK_CON(void);
void ReadInitPara(void);
void Delayms(u32 ms);
void Delayus(u32 us);
void ReadInitPara(void);
void RestorePara(void);
void UartFramePro(UartRcvStruct *frame);
void UART_FrameRcv(UartRcvStruct *frame,unsigned char dat);
void ADCtask(void);
extern u16 ADC_RUN(unsigned char ch);
extern u16 ADC_ConvertedValue[40];
extern unsigned short AGC_Task_Time; //100MS = 5*20
extern unsigned short ADCtask_Time; //300ms = 5 * 60
extern GPIO_InitTypeDef  GPIO_InitStructure; 
extern void ATTControl(u8 att);
extern void AGC_Task(void);
extern void FLASH_WW(u32 Add,u32 len,u16 *data);
extern void FLASH_RR(u32 Add,u32 len ,u16 *data);
#endif
