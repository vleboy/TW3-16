#include "all.h"
//unsigned char Celsius_H=0,Celsius_L=0;   
//void l8b20_init(void)
//{
//	 volatile unsigned char f;
//	 DQio=1;
//	 DelayUsUs(16);
//	 DQio=0;		  //��λ����
//	 DelayUsUs(160);	  //��ʱ
//	 DQio=1;		  //�ָ�
//	 DelayUsUs(28);
//	 f=DQio;		  //����������
//	 DelayUsUs(40);
//}

//void l8b20_write_cmd(unsigned char cmd)
//{
//	unsigned char i;
//	for(i=8;i>0;i--)
//	  {
//	  	DQio=0;			//��ʼ
//		  DQio=cmd&0x01;	//��������
//		  DelayUsUs(10);	
//		  DQio=1;			//���ͽ���
//		  cmd=cmd>>1;		//��������һλ
//		  DelayUsUs(8);
//	  }
//}

//unsigned char l8b20_Read_dat()
//{
//	unsigned char i,dat=0;
//	for(i=8;i>0;i--)
//	  {
//	  	DQio=0;		   //��ʼ��ȡ
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
//		l8b20_init();	 //��ʼ��
//	  l8b20_write_cmd(0xcc); //����������к�
//	  l8b20_write_cmd(0x44);//�����¶�ת��
//	  DelayUsUs(20);
//	  l8b20_init();
//	  l8b20_write_cmd(0xcc);
//	  l8b20_write_cmd(0xbe); //���Ĵ���
//	  DelayUsUs(20);
//	
//	  CL=l8b20_Read_dat();//���¶ȵ�λ
//	  CH=l8b20_Read_dat();//���¶ȸ�λ
//    
//	   
//   tI = CH;
//   tI <<= 8;
//   tI = tI | CL;
//   if( tI > 0x8000 ) {     //���¶�
//       tI = ~tI + 1;       //ȡ����1
//       tF = tI * 0.0625;
//       tI = tF * 10 + 0.5; //�Ŵ�10���������������
//       tI = tI | 0x8000;   //���λΪ���¶ȱ�־λ
//   } else {
//       tF = tI * 0.0625;
//       tI = tF * 10 + 0.5; //�Ŵ�10���������������
//   }
//	 MainStructR.Ce = (signed short)tI;
////   return(tI);
//}



//void DelayUs(unsigned int i)   //us��ʱ
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

// 18B20��λ����
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
	     DQio = 0;	//MCU��0 550us,��18B20����reset�ź�
	     DelayUs(500); 	
	     DQio = 1; //MCU��λ
	     DelayUs(50);  //66us  50 
	     flag = DQio; //��ʱ66us,18B20������������"0"����λ�ɹ�
		// RS485Send_one_char(flag,1);
		 count++;
	  }
      DelayUs(400);   //��ʱ500us,18B20������1	 450
	  if(DQio) flag = 0;
	  else flag = 1;
   }
   DQio=1;
}

//18B20д1���ֽں��� * ��1-WIRE������дһ���ֽ�
void l8b20_write_cmd(unsigned char val)
{
	unsigned char i;
   	for (i=0; i<8; i++)
	{
		DQio = 1;
		DelayUs(2);  // _nop_()
		DQio = 0;
		DelayUs(6);   //4us
		DQio = val & 0x01; //���λ�Ƴ�
		DelayUs(60);  //66us
		val >>= 1; //����һλ
	}
	DQio = 1;
	DelayUs(1);  
}

//18B20��1���ֽں��� * ��1-WIRE�����϶�ȡһ���ֽ�
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
////  ��������Ŵ���10��
////  ����25�� �����¶Ȳ���
///////////////////////////////////////////////
//short read_temp(void) //��ȡ�¶�ֵ
void ReadCelsius(void) //��ȡ�¶�ֵ
{ unsigned char CL,CH;  float tF=0; unsigned short tI=0;
//		unsigned char temp_data[2] = {0};//�����¶��ݷ�
		//unsigned int Flag=0;
//		unsigned short Temper;
	//	short Temperature1;

		l8b20_init(); //��λ
		l8b20_write_cmd(0xCC); // ��Skip ROM����
 	    l8b20_write_cmd(0x44); // ��ת������

		l8b20_init(); //��λ
		l8b20_write_cmd(0xCC); // ����ROM
		l8b20_write_cmd(0xBE);//��RAM�����9���ֽڣ�ֻȡǰ���¶��ֽ�

		CL=l8b20_Read_dat();  //�¶ȵ�8λ
		CH=l8b20_Read_dat();  //�¶ȸ�8λ
		//DS18b20_reset(); //��λ
	//	Temper=((temp_data[1]<<8)|temp_data[0]);


//		if(Temper&0xF800)  //�и���
//		{
//			//Flag=1;
//			Temper=((~Temper)+1);
//			Temper*=0.625; //С�������һλ
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
   if( tI > 0x8000 ) {     //���¶�
       tI = ~tI + 1;       //ȡ����1
       tF = tI * 0.0625;
       tI = tF * 10 + 0.5; //�Ŵ�10���������������
       tI = tI | 0x8000;   //���λΪ���¶ȱ�־λ
   } else {
       tF = tI * 0.0625;
       tI = tF * 10 + 0.5; //�Ŵ�10���������������
   }
	 MainStructR.Ce = (signed short)tI;
}


