#include "all.h"
int FLASH_WriteNBytes ( uint32_t addr, unsigned char  *dat_buf, unsigned char no );
uint32_t FMC_Read(uint32_t dwiAddr);
unsigned char RXTEMP[40] = {0};
void FMC_Init()
{
   
    FLASH_Unlock();
		 FLASH_SetLatency(FLASH_Latency_2); 
   FLASH_Lock();
    	
}
void RestorePara(void)
{
		int i;
    // 恢复设置参数
		for(i=0;i<16;i++)
		{
			MainAscii.PartNumber[i] = PartNumber[i];
			MainAscii.LogicID[i] = LogicID[i];
			MainAscii.SerialNumber[i] = SerialNumber[i];
			MainAscii.VendorName[i] = VendorName[i];
		}
		
		
    MainStructW.Save_flag 	= INIT_FLAG;			  // 存储标志位yes
    MainStructW.AB_Switch		= 0x03;					    //  0x01:强制切换到A，0x02:强制切换到B，0x03:A优先，0x04:B优先 
		MainStructW.AB_Switch_door = 0;							//  -6 ~ +3 
		MainStructW.Port12				 = 0x01;					//  0x01:无衰减，0x02:低衰减，0x03:高衰减
		MainStructW.Port34				 = 0x01;
		MainStructW.AGCa					 = 0;
		MainStructW.AGCb					 = 0;
		MainStructW.RF						 = 0;
		FLASH_ErasePage(SAVE_FLASH_ADDRESS);
		
    FLASH_WriteNBytes ( SAVE_FLASH_ADDRESS, ( unsigned char * ) &MainStructW.Save_flag, ( sizeof ( MainStructW ) / sizeof ( char ) ) );
    FLASH_WriteNBytes((SAVE_FLASH_ADDRESS+ sizeof ( MainStructW ) / sizeof ( char ) ),(unsigned char *)&MainAscii.PartNumber[0],(sizeof(MainAscii)/sizeof(char)));
}
void ReadInitPara(void)
{

	//   FLASH_ReadNByte ( SAVE_FLASH_ADDRESS, RXTEMP, ( sizeof ( MainStructW ) / sizeof ( char ) ) );
 //   Delayms ( 50 );
    MainStructW = * ( MainWStruct * ) &RXTEMP;
	//	FLASH_ReadNByte (( SAVE_FLASH_ADDRESS+sizeof ( MainStructW ) / sizeof ( char ) ), RXTEMP, ( sizeof ( MainAscii ) / sizeof ( char ) ) );
		MainAscii = * (AsciiStruct *) &RXTEMP;

/*

MONI

*/
/*--------------------------------------------*/

//				 MainStructR.online = 0x0f;
//				 MainStructR.AC60 = 60;
//				 MainStructR.DC24 = 240;
//				 MainStructR.DC5 = 50;
//				 MainStructR.Temp = 33;
//				 MainStructR.ORAop = 1;
//				 MainStructR.ORBop = 1;
//				 MainStructR.OTAop = 30;
//				 MainStructR.OTBop = 30;
//				 MainStructR.OTAim = 20;
//				 MainStructR.OTBim = 20;
//				 MainStructR.RF = 100;
//				 MainStructR.Box = 0x01;
//				 MainStructR.Warning = 0;
/*-------------------------------------------*/

    if ( MainStructW.Save_flag != INIT_FLAG )
        {
            RestorePara();
        }
}

int FLASH_WriteNBytes ( uint32_t addr, unsigned char  *dat_buf, unsigned char no )
{
	uint32_t wdata = 0;
	
	
    if ( 0 == no ) { return ( 0 ); }
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR); 
    while ( 1 )
        {
	  				wdata = (uint32_t)(*(dat_buf)<<0)+(uint32_t)(*(dat_buf+1)<<8)+(uint32_t)(*(dat_buf+2)<<16)+(uint32_t)(*(dat_buf+3)<<24);
            FLASH_ProgramWord(addr,wdata ); 	// 写入数据
            dat_buf+=4;			// 指向下一个待写入的数据
            addr+=4;
            no-=4;

            if ( 0 == no ) { break; }			// 判断是否写完所有数据，若是则退出

            if ( ( addr / PAGE_SIZE ) > 2048 )		// 判断是否要换页
                {
                    FLASH_ErasePage(addr);
                }
        }
				FLASH_Lock();
				return 1;
}
void  FLASH_ReadNByte ( uint32_t addr, unsigned char *dat_buf, unsigned char  no )
{

    uint32_t  *readp;
		uint32_t xxx=0;
    if ( 0 == no ) { return; }
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR); 
    readp = ( uint32_t * ) addr;				// 设置读取的地址

    for ( ; no > 0;  )
        {
				xxx	 = FMC_Read(addr);
				*(uint32_t *)dat_buf = xxx;
						addr+=4;
            dat_buf+=4;
            readp+=4;				// 指向下一地址
						no-=4;
        }
}
uint32_t FMC_Read(uint32_t dwiAddr)
{
 uint32_t *ptrFlash;
 ptrFlash = (uint32_t*)(dwiAddr);
 return *ptrFlash;
}



