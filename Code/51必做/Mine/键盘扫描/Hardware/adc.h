#ifndef _ADC_H_
#define _ADC_H_//д��Դ�ļ�Ӣ��ΪСд����ҲҪ�ô�д����
#define uchar unsigned char	
#define uint  unsigned int
sbit CS   = P3^5;
sbit CLK  = P3^6;
sbit DIO = P3^7;
void DigDisplay();
unsigned char Get_AD_Result();
#endif
