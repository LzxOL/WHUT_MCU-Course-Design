#include "reg52.h"
#include "adc.h"
#include <stdio.h>
#include "delay.h"

extern unsigned char AD1;

extern unsigned char dis_adc[10];
extern unsigned int i,j;
unsigned char date[8];//ADC0832


void DigDisplay()
{
    int i;
    for(i=0; i<8; i++)
    {
        switch(i)
        {
            case(0):
                P3 = 0xff;  // ����
                P2 = 0x01;  // ����WX0
                P3 = 0x7f;  // ����
                P2 = date[i] & 0x7f;
                delay_ms(2);  // ������ʱ��������˸�ٶ�
                break;

            case(1):
                P3 = 0xff;  // ����
                P2 = 0x02;  // ����WX1
                P3 = 0x7f;  // ����
                P2 = date[i];
                delay_ms(2);  // ������ʱ��������˸�ٶ�
                break;

            case(2):
                P3 = 0xff;  // ����
                P2 = 0x04;  // ����WX2
                P3 = 0x7f;  // ����
                P2 = date[i];
                delay_ms(2);  // ������ʱ��������˸�ٶ�
                break;

            case(3):
                P3 = 0xff;  // ����
                P2 = 0x08;  // ����WX3
                P3 = 0x7f;  // ����
                P2 = date[i];
                delay_ms(2);  // ������ʱ��������˸�ٶ�
                break;
        }
                delay_ms(1);  // ������ʱ��������˸�ٶ�
    }
}

unsigned char Get_AD_Result()
{
    uchar i;
    uchar data1=0,data2=0;
    CS=0;
    
  //��һ���½��ص���ǰ��DI����1����ʼ����λ����ʼת��
    CLK=0;DIO=1;    
    CLK=1;
    
 //�ڶ����½��ص���ǰ����D=1/0��ѡ�񵥶�/��֣�SGL/DIF��ģʽ�еĵ�������ģʽ
    CLK=0;DIO=1;   
    CLK=1;           
 
 //�������½��ص���ǰ����D=0/1��ѡ��CH0/CH1������ѡ��ͨ��ch0  
    CLK=0;DIO=1;    
    CLK=1;DIO=0;   
    
 //���ĸ��½��ص���ǰ,DI =1 
    CLK=0;DIO=1; 
    
//4-11����8���½��� DO���ת���źţ���ȡ���ݣ�MSB-->LSB�� 
    for(i=0;i<8;i++)
    {
        CLK=1;
        CLK=0;
       data1=(data1<<1)|(uchar)DIO;
    }
    CS=1;
    return data1;
}

//void main(void)
//{
//	
//	uint vt1;	
//	uint while_times = 0;
//	
//	delay_ms(300);
//	ET0  = 1;
//    TR0  = 1;
//    ET1  = 1;
//    TR1  = 1;
//    EA   = 1;     
//    CLK  = 0;      
//    CS   = 1; 
//	 
//	while(1)
//	{	
//	   AD1=Get_AD_Result();
//	   vt1=	(5000/256.0)*AD1;
//		date[0] =dis_adc[vt1/1000];
//		date[1] =dis_adc[vt1/100%10]; 
//		date[2] =dis_adc[vt1%100/10];
//		date[3] =dis_adc[vt1%10];
//		DigDisplay(); 	
//	}
//}