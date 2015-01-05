#include "all.h"
//unsigned char Celsius_H=0,Celsius_L=0;   
//void l8b20_init(void)
//{
//	 volatile unsigned char f;
//	 DQio=1;
//	 DelayUsUs(16);
//	 DQio=0;		  //复位脉冲
//	 DelayUsUs(160);	  //延时
//	 DQio=1;		  //恢复
//	 DelayUsUs(28);
//	 f=DQio;		  //读存在脉冲
//	 DelayUsUs(40);
//}

//void l8b20_write_cmd(unsigned char cmd)
//{
//	unsigned char i;
//	for(i=8;i>0;i--)
//	  {
//	  	DQio=0;			//开始
//		  DQio=cmd&0x01;	//传送命令
//		  DelayUsUs(10);	
//		  DQio=1;			//传送结束
//		  cmd=cmd>>1;		//命令右移一位
//		  DelayUsUs(8);
//	  }
//}

//unsigned char l8b20_Read_dat()
//{
//	unsigned char i,dat=0;
//	for(i=8;i>0;i--)
//	  {
//	  	DQio=0;		   //开始读取
//		  dat>>=1;
//		  DQio=1;
//		 if(DQio) dat|=0x80;
//		 DelayUsUs(10);
//	  }
//	return(dat);
//}

//void ReadCelsius(void)
//{	
//    unsigned char CL,CH;  float tF=0; unsigned short tI=0;
//		l8b20_init();	 //初始化
//	  l8b20_write_cmd(0xcc); //跳过检测序列号
//	  l8b20_write_cmd(0x44);//启动温度转换
//	  DelayUsUs(20);
//	  l8b20_init();
//	  l8b20_write_cmd(0xcc);
//	  l8b20_write_cmd(0xbe); //读寄存器
//	  DelayUsUs(20);
//	
//	  CL=l8b20_Read_dat();//读温度低位
//	  CH=l8b20_Read_dat();//读温度高位
//    
//	   
//   tI = CH;
//   tI <<= 8;
//   tI = tI | CL;
//   if( tI > 0x8000 ) {     //负温度
//       tI = ~tI + 1;       //取反加1
//       tF = tI * 0.0625;
//       tI = tF * 10 + 0.5; //放大10倍输出并四舍五入
//       tI = tI | 0x8000;   //最高位为负温度标志位
//   } else {
//       tF = tI * 0.0625;
//       tI = tF * 10 + 0.5; //放大10倍输出并四舍五入
//   }
//	 MainStructR.Ce = (signed short)tI;
////   return(tI);
//}



//void DelayUs(unsigned int i)   //us延时
//{
//	while(i-->0)
//	{
//		SysTick->LOAD = 1 * CyclesPerUs;
//	    SysTick->VAL  = (0x00);
//	    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
//	
//	    /* Waiting for down-count to zero */
//	    while((SysTick->CTRL & (1 << 16)) == 0);
//	}
//}

// 18B20复位函数
void l8b20_init(void)
{
    unsigned char flag=1;
	short count;

	count = 0;
	while((flag)&&(count<10))
	{
	  while((flag)&&(count<10))
	  {
	     DQio = 1;
	     DelayUs(1);
	     DQio = 0;	//MCU置0 550us,向18B20发出reset信号
	     DelayUs(500); 	
	     DQio = 1; //MCU复位
	     DelayUs(50);  //66us  50 
	     flag = DQio; //延时66us,18B20发出存在脉冲"0"，复位成功
		// RS485Send_one_char(flag,1);
		 count++;
	  }
      DelayUs(400);   //延时500us,18B20重新置1	 450
	  if(DQio) flag = 0;
	  else flag = 1;
   }
   DQio=1;
}

//18B20写1个字节函数 * 向1-WIRE总线上写一个字节
void l8b20_write_cmd(unsigned char val)
{
	unsigned char i;
   	for (i=0; i<8; i++)
	{
		DQio = 1;
		DelayUs(2);  // _nop_()
		DQio = 0;
		DelayUs(6);   //4us
		DQio = val & 0x01; //最低位移出
		DelayUs(60);  //66us
		val >>= 1; //右移一位
	}
	DQio = 1;
	DelayUs(1);  
}

//18B20读1个字节函数 * 从1-WIRE总线上读取一个字节
unsigned char l8b20_Read_dat()
{
	unsigned char i, value=0;
   	for (i=0; i<8; i++)
	{
		DQio=1;
		DelayUs(1);  	  // _nop_()
		value >>= 1;
		DQio = 0;
		DelayUs(4);   //4us
		DQio = 1;
		DelayUs(4);   //4us 	 
		if (DQio)
			value|=0x80;
		DelayUs(60); //66us	
	}				
	DQio=1;
  	return(value);
}

///////////////////////////////////////////////
////  采样结果放大了10倍
////  低于25度 不做温度补偿
///////////////////////////////////////////////
//short read_temp(void) //读取温度值
void ReadCelsius(void) //读取温度值
{ unsigned char CL,CH;  float tF=0; unsigned short tI=0;
//		unsigned char temp_data[2] = {0};//读出温度暂放
		//unsigned int Flag=0;
//		unsigned short Temper;
	//	short Temperature1;

		l8b20_init(); //复位
		l8b20_write_cmd(0xCC); // 发Skip ROM命令
 	    l8b20_write_cmd(0x44); // 发转换命令

		l8b20_init(); //复位
		l8b20_write_cmd(0xCC); // 跳过ROM
		l8b20_write_cmd(0xBE);//读RAM命令，共9个字节，只取前两温度字节

		CL=l8b20_Read_dat();  //温度低8位
		CH=l8b20_Read_dat();  //温度高8位
		//DS18b20_reset(); //复位
	//	Temper=((temp_data[1]<<8)|temp_data[0]);


//		if(Temper&0xF800)  //有负号
//		{
//			//Flag=1;
//			Temper=((~Temper)+1);
//			Temper*=0.625; //小数点后面一位
//			Temperature1=Temper; 
//			Temperature1*=-1;

//	//		dat	= 0;		//

//		}
//		else
//		{
//			//Flag=0;
//			Temperature1=Temper*0.625;
////	dat = Temperature1;
//			//Temperature1*=-1;
//		}


//	   //RS485Send_one_char(temp_data[1],1);
//	   //RS485Send_one_char(temp_data[0],1);
//	   //RS485Send_one_char(Temperature1>>8,1);
//	   //RS485Send_one_char(Temperature1,1);
//		return (Temperature1);

   tI = CH;
   tI <<= 8;
   tI = tI | CL;
   if( tI > 0x8000 ) {     //负温度
       tI = ~tI + 1;       //取反加1
       tF = tI * 0.0625;
       tI = tF * 10 + 0.5; //放大10倍输出并四舍五入
       tI = tI | 0x8000;   //最高位为负温度标志位
   } else {
       tF = tI * 0.0625;
       tI = tF * 10 + 0.5; //放大10倍输出并四舍五入
   }
	 MainStructR.Ce = (signed short)tI;
}


