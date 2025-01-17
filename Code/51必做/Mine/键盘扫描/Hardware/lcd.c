#include "reg52.h"
#include "delay.h"
#include "lcd.h"

sbit RS=P1^0;      // lcd��RS�ӿڽӵ���Ƭ����P1�ĵ�0��
sbit RW=P1^1;		// lcd��RW�ӿڽӵ���Ƭ����P1�ĵ�1��
sbit E=P1^2;		// lcd��E�ӿڽӵ���Ƭ����P1�ĵ�2��

//  ��lcd��ʾʱ�õ����ַ��������һ����ʱ����������Ҳ���Ի���-��
extern unsigned char str1[];
extern unsigned char str2[];
extern unsigned int i,j;
//  lcdд���ݺ��� дdat����ĸ
void xdat(unsigned char dat)
{
	RS = 1;
	RW = 0;
	E = 0;
	P0 = dat;
	delay_ms(5);
	E = 1;
	E = 0;
}
 
//  lcdд�����  д��������ĸxml
void xml(unsigned char ml){
	RS = 0;
	RW = 0;
	E = 0;
	P0 = ml;
	delay_ms(5);
	E = 1;
	E = 0;
}
 
//  lcd��ʼ������
void lcd_init(){
	xml(0x38);//��λ���ݽӿڣ�������ʾ��5*7����
	xml(0x0c);//��ʾ�������أ���˸��
	xml(0x06);//���ݶ�д�����󣬹���Զ���һ�����治��
	xml(0x01);//��긴λ������
}
 
// ������
void delay_lcd(){
	xml(0x80);        // ��lcd�ĵ�һ����࿪ʼд�� 1000 0000
	for(i=0;i<=8;i++)
	{
		xdat(str1[i]);
		delay_ms(5);
	}
	for(j=15;j>=0;j--)
	{
		xml(0xc0+j);  // ��lcd�ĵڶ�����࿪ʼд��
		for(i=0;i<=14;i++)
		{
			xdat(str2[i]);
			delay_ms(5);
		}
		delay_ms(500);
	}
}