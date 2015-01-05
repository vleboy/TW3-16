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

#define TRUE 1
#define FLASE 0
#define FREE  0
#define PRING  1
#define MainAddr 0

#define FRAME_HEAD1		0xf5	// ����֡ͷ1
#define FRAME_HEAD2		0xfa	// ����֡ͷ2
#define FRAME_REAR		0xaa	// ����֡β	

#define UART_RCVFLAG_HEAD1			0x00		// ͷ1
#define UART_RCVFLAG_ADDR			0x01	// ��ַ
#define UART_RCVFLAG_HEAD2			0x02		// ͷ2
#define UART_RCVFLAG_CTL			0x03	// ������
#define	UART_RCVFLAG_LEN			0x04	// ����
#define UART_RCVFLAG_DAT			0x05	// ����
#define UART_RCVFLAG_CS				0x06	// Ч��
#define UART_RCVFLAG_REAR			0x07	// β

#define I2C_RCVFLAG_HEAD1		0x00	// ͷ1
#define I2C_RCVFLAG_MODENUM		0x01	// ģ���
#define	I2C_RCVFLAG_LEN			0x02	// ����
#define I2C_RCVFLAG_DAT			0x03	// ����
#define I2C_RCVFLAG_CS			0x04	// Ч��
#define I2C_RCVFLAG_REAR		0x05	// β

#define ZXFS_MODU 0x01						//������ģ��
#define ZXJS_MODU 0x02
#define FXJS_MODU 0x03
#define EDFA_MODU 0x04
#define TP1310 0x01								//1310�ͺ�
#define TP1550 0x02
#define TPMA   0x00								//��ģ��
/**/
#define ModuAddr1	0X01								//��ۺ�1����ַ��
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


/* �ӷ�����ģʽ */
#define  I2C_ST1      (0xA8)               // (SR) 	  �ѽ�������SLA+R,�ѷ���ACK
#define  I2C_ST2      (0xB0)               // (SR) 	  ������ʱ��SLA+R/W �ж�ʧ�ٲã� �ѽ�������SLA+R���ѷ���ACK
#define  I2C_ST3      (0xB8)               // (SR) 	  �ѷ���I2DAT�������ֽڣ��ѽ���ACK
#define  I2C_ST4      (0xC0)               // (ST)	  �ѷ���I2DAT �������ֽڣ��ѽ��շ�ACK
#define  I2C_ST5      (0xC8)               // (ST)	  �ѷ���I2DAT �����������ֽڣ�AA=0��;�ѽ���ACK
/* �ӽ�����ģʽ */
#define  I2C_SR1      (0x60)               // (SR)   �ѽ�������SLA+W; �ѽ���ACK
#define  I2C_SR2      (0x68)               // (SR)	  ������ʱ��SLA+R /W �ж�ʧ�ٲ�;�ѽ�������SLA+W,�ѷ���ACK
#define  I2C_SR3      (0x70)               // (SR)	  �ѽ���ͨ�õ��õ�ַ(00H);�ѷ���ACK
#define  I2C_SR4      (0x78)               // (SR)	  ������ʱ��SLA+R/W �ж�ʧ�ٲ�; �ѽ���ͨ�õ��õ�ַ;�ѷ���ACK
#define  I2C_SR5      (0x80)               // (SR)	  ǰһ��Ѱַʹ������ӵ�ַ���ѽ��������ֽڣ��ѷ���ACK
#define  I2C_SR6      (0x88)               // (SR)	  ǰһ��Ѱַʹ������SLA ��ַ���ѽ��������ֽڣ� �ѷ��ط�ACK
#define  I2C_SR7      (0x90)               // (SR)	  ǰһ��Ѱַʹ��ͨ�õ��ã��ѽ��������ֽڣ��ѷ���ACK
#define  I2C_SR8      (0x98)               // (SR)   ǰһ��Ѱַʹ��ͨ�õ��ã��ѽ��������ֽڣ��ѷ��ط�ACK
#define  I2C_SR9      (0xA0)               // (SR)	  ��ʹ��SLA/REC ��SLA/TRX ��̬Ѱַʱ,���յ�ֹͣ�������ظ�����ʼ����

/*--------------------------------------------------------------------*/
#define I2cSwitchModu(x) x==0x01?Modu1:x==0x02?Modu2:x==0x03?Modu3:x==0x04?Modu4:x==0x05?Modu5:x==0x06?Modu6:x==0x07?Modu7:x==0x08?Modu8:x==0x09?Modu9:x==0x0a?ModuA:ModuA

/*--------------------------------------------------------------------*/
/*�ṹ��Ķ���																											  */
/*--------------------------------------------------------------------*/
typedef struct
{
    unsigned char rcvflag;				// ����״̬��־
    unsigned char count;				// ������
    unsigned char addr;					// ��ַ
    unsigned char ctlcode;				// ������
    unsigned char len;					// �����򳤶�
    unsigned char dat[UartFrameLen];	// ���ݴ洢��
    unsigned char chksum;				// Ч�������
    unsigned char cs;					// Ч��λ
    unsigned char ProFlag;				// �����־
} UartRcvStruct;

typedef struct
{
    unsigned char Online;
    unsigned char Type;					//1310or1550;
    unsigned char ModuType;				// ģ������
    unsigned char DataLen;
    unsigned char ModuData[ModuLen];	// ģ������
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
    unsigned char ModuNum;				// ģ�����
    unsigned char Len;					// �����򳤶�
    unsigned char dat[UartFrameLen];	// ���ݴ洢��
    unsigned char chksum;				// Ч�������
    unsigned char cs;					// Ч��λ
    unsigned char ProFlag;				// �����־
} I2CRcvStruct;


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
extern void I2cTask ( I2CRcvStruct *frame );
extern void AdcTask ( void );

extern void I2cFramePro ( void );
extern UartRcvStruct UartRcvFrame;			//����֡
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
