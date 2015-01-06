#include "all.h"

void ADC_DMA_CON(void);
void ADC_INIT_CON(void);
void alignmentADC(void);
void ADC_GPIO_CON(void);
static DMA_InitTypeDef DMA_InitStructure; 

static ADC_InitTypeDef ADC_InitStructure;
//unsigned short ModulenumTable[10]={0x00F7,0x01EF,0x02E7,0x03DF,0X04D8,0X05D0,0X06C8,0X07C0,0X08B9,0X09B1};    //0.2V ~ 2.0V
//u16 ADC_ConvertedValue[40]={0};
//u16 temp[4]={0};
//unsigned char MediafilterID = 0;				    //���ID��ȫ�� ��ΪͨѶ��ַ

u32 ADCvalue[10][4];/*AIN0 ���ջ��⹦��1
													AIN1 ����������A
													AIN2 ������⹦��A
													AIN3 ���ջ��⹦��2
													AIN4 +5V
													AIN5 +24V
													AIN6 AC60V
													AIN7 RF_POW 
													8:	 ����������B
													9:   ������⹦��B*/
u32 ADCcovVal[10];
/*-----------------------------------------------------------------*/
/* ��ʼ��ADC                                                       */
/*-----------------------------------------------------------------*/
void ADC_Init(void)
{
	ADC_GPIO_CON();
	ADC_DMA_CON();
	ADC_INIT_CON();
	alignmentADC();
	
	while(2012)
	{
		Delayms(50);
				temp[0]= Sample_Modulenum();
				Delayms(50);
				temp[1]= Sample_Modulenum();
				Delayms(50);
				temp[2]=Sample_Modulenum();
				Delayms(50);
				temp[3]= Sample_Modulenum();
				Delayms(50);
				if((temp[0] == temp[1]) && (temp[0] == temp[2]) && (temp[0] == temp[3]) && (temp[0] != 0))
				{
				
					MediafilterID = temp[0];
					break;
				}
	}
	
}
u16 ADC_RUN(unsigned char ch)
{
	u16 temp1[4];
	u16 max,min,i;
	u16 filterdata;
	temp1[0]=ADC_ConvertedValue[ch];
	temp1[1]=ADC_ConvertedValue[ch+8];
	temp1[2]=ADC_ConvertedValue[ch+16];
	temp1[3]=ADC_ConvertedValue[ch+24];
	max=min=temp1[0];
	for (i=0;i<4;i++)
	{
		if(max<=temp1[i]) max=temp1[i];
		if(min>=temp1[i]) min=temp1[i];
	}
	filterdata = ((0ul+temp1[0]+temp1[1]+temp1[2]+temp1[3]-max-min)>>1);
	return filterdata;
}
unsigned char Sample_Modulenum(void)
	{
		unsigned short temp = 0;	
		temp = ADC_RUN(ADC_ID);//��ֵ����ֵ�˲�
		  
		if(temp <= ModulenumTable[0]+128 && temp >= ModulenumTable[0] - 128)
		{
			return 0x01;
		}
		else if(temp <= ModulenumTable[1]+128 && temp >= ModulenumTable[1] - 128)
		{
		
			return 0x02;
		}
		else if(temp <= ModulenumTable[2]+128 && temp >= ModulenumTable[2] - 128)
		{
		
			return 0x03;
		}
		else if(temp <= ModulenumTable[3]+128 && temp >= ModulenumTable[3] - 128)
		{
		
			return 0x04;
		}
		else if (temp <= ModulenumTable[4]+128 && temp >= ModulenumTable[4] - 128)
		{
			return 0x05;
		}
		else if (temp <= ModulenumTable[5]+128 && temp >= ModulenumTable[5] - 128)
		{
			return 0x06;
		}
		else if (temp <= ModulenumTable[6]+128 && temp >= ModulenumTable[6] - 128)
		{
			return 0x07;
		}

		else if (temp <= ModulenumTable[7]+128 && temp >= ModulenumTable[7] - 128)
		{
			return 0x08;
		}
		else if (temp <= ModulenumTable[8]+128 && temp >= ModulenumTable[8] - 128)
		{
			return 0x09;
		}
		else if (temp <= ModulenumTable[9]+128 && temp >= ModulenumTable[9] - 128)
		{
			return 0x0A;
		}
		return 0;
	}

void ADC_GPIO_CON(void)
{
	/*PA0123567Ϊģ������*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	/*PB01*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}
void ADC_DMA_CON(void)
{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//����ADC��DMAʱ��
		
		DMA_DeInit(DMA1_Channel1);												//ʹ��DMA1��1��ͨ��
		
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)( &(ADC1->DR));  //��ADC��������
		
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;  //ADC�ɼ�ֵ�ڴ�ӳ��
		
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//����ΪDMA����ADC
		
		DMA_InitStructure.DMA_BufferSize = 40;													//���䵥λ���������ݴ�С

		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�ر������������ͨ��ֻ��ADC1
		
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//���ڴ��ַ����
		
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //halfword

		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;					//HALFWORD
		
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;															//ѭ��ģʽ
		
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;													//�����ȼ�

		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;																//�ر�M2M

		DMA_Init(DMA1_Channel1,&DMA_InitStructure);																	//ʹ��DMA

		DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, DISABLE); 														//�ر�DMA�ж�
		
		DMA_Cmd(DMA1_Channel1, ENABLE);																							//DMA��ʼ����
}

void ADC_INIT_CON(void)
{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE); 												//��ADCʱ��
		
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;													//����ģʽ
		
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;       													//��ͨ��ģʽ
		
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;													//����ɨ��ģʽ
		
		ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;						//�������
		
		ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;												//�ұ߶���
		
		ADC_InitStructure.ADC_NbrOfChannel=10;																				//ͨ����

		ADC_Init(ADC1, &ADC_InitStructure); 																				//��ʼ��ADC����
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);		//ͨ��0����������0������ID
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_239Cycles5);		//ͨ��1����������1������⹦��
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_2,3,ADC_SampleTime_239Cycles5);		//ͨ��2����������2�������¶�
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_3,4,ADC_SampleTime_239Cycles5);		//ͨ��3����������3�����������¶ȵ���
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_4,5,ADC_SampleTime_239Cycles5);   //ͨ��4����������4��
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_5,6,ADC_SampleTime_239Cycles5);		//ͨ��5����������5��
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_6,7,ADC_SampleTime_239Cycles5);		//ͨ��6����������6������������
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_7,8,ADC_SampleTime_239Cycles5);		//ͨ��7����������7������RF
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_8,9,ADC_SampleTime_239Cycles5);		//ͨ��8����������8������OPin.Power
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_9,10,ADC_SampleTime_239Cycles5);		//ͨ��9����������9������OPins.Power
		
		
}




void alignmentADC(void)
{
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_Cmd(ADC1, ENABLE);   
	
	ADC_ResetCalibration(ADC1);
	
	while(ADC_GetResetCalibrationStatus(ADC1)); 
	
	ADC_StartCalibration(ADC1); 
	
	while(ADC_GetCalibrationStatus(ADC1)); 
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); 
}
