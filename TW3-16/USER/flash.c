#include "all.h"
void FLASH_W(u32 Add,u32 len,u16 *data);
void FLASH_R(u32 Add,u32 len ,u16 *data);
void RestorePara(void)
{
	// 恢复设置参数
	Config.Ver          = Version;
	Config.saveflag 	= INIT_FLAG;			  // 存储标志位yes
	Config.AGC			= AGC_ON_MGC_OFF;		  // 0==AGC,1==MGC
	Config.laserSWflag	= OPEN;					  // 0==Laser close,1==laser open
	// 恢复出厂数据
	Dataflash.ID		= 0x01020304;
	Dataflash.AGC		= AGC_RF_DEFAULT;		  //宽带模拟AGC-att
	Dataflash.MGC		= MGC_ATT_DEFAULT;		  //宽带模拟MGC-att
	Dataflash.Err		= 0;
	
	FLASH_WW(SAVE_FLASH_ADDRESS,(sizeof( Config ) / sizeof( short )),(unsigned short *)&Config.saveflag);
	Delayms(50);
	FLASH_WW(SAVE_FLASH_ADDRESS+10,(sizeof( Dataflash ) / sizeof( short )),(unsigned short *)&Dataflash.ID);
	Delayms(50);
}
void ReadInitPara(void)
{

	#if 1
 	FLASH_RR(SAVE_FLASH_ADDRESS,(sizeof( Config ) / sizeof( short )),(unsigned short *)&Config.saveflag);
	Delayms(50);
	FLASH_RR(SAVE_FLASH_ADDRESS+10,(sizeof( Dataflash ) / sizeof( short )),(unsigned short *)&Dataflash.ID);
	Delayms(50);
	if(Config.saveflag !='a')
	{

		RestorePara();

	}

	#else
	Dataflash.ID = 0x01020304;
	#endif 
}

void FLASH_WW(u32 Add,u32 len,u16 *data)
{
	FLASH_Unlock();

	
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);  

	FLASH_ErasePage(Add); 

	while(len)
	{
		FLASH_ProgramHalfWord(Add,*data);  //flash  为一个字节存储，16位数据必须地址加2 
		Add+=2;
		len--;
	}

	 FLASH_Lock(); 
	
}

void FLASH_RR(u32 Add,u32 len ,u16 *data)
{
	while(len)
	{
		*data = *(u16 *)(Add); 
		data++;
		Add+=2;
		len--;
	}
}
