#include <REGX52.H>
#include <INTRINS.H>
#include "delay.h"

extern unsigned char code ShowNum[];
/*
���������Ӧ��ϵ��
  0: 0xC0
  1: 0xF9
  2: 0xA4
  3: 0xB0
  4: 0x99
  5: 0x92
  6: 0x82
  7: 0xF8
  8: 0x80
  9: 0x90
a-f:0x88,0x83,0xc6,0xa1,0x86,0x8e
*/

extern unsigned int temp[3];
void InitUART(void) {
    PCON &= 0x7F;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	TMOD &= 0x0F;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xFD;			//�趨��ʱ��ֵ
	TH1 = 0xFD;			//�趨��ʱ����װֵ		
	TR1 = 1;			//������ʱ��1
	ET1 = 0;        	//��ֹ��ʱ��1�ж�
	EA=1;				//�������ж� 
	ES=1;				//���Ŵ����ж�
}

void weichoice(unsigned int i, unsigned int j) {
    P3_7 = 0;
    P3_6 = 0;
    if (i > 7) i = i % 8;
    P2 = _crol_(0x01, i);
    P3_7 = 1;
    P3_7 = 0;
    P2 = ShowNum[j];
    P3_6 = 1;
    P3_6 = 0;
}

void tempcount(unsigned int k) {
    // ȷ�� k �� 0 - 99 ��Χ�ڣ���ְ١�ʮ����λ
    temp[0] = k / 100;  // ��λ
    temp[1] = (k / 10) % 10; // ʮλ
    temp[2] = k % 10;  // ��λ
}

void Delay(unsigned int i) {
    unsigned int j;
    for (; i > 0; i--) {
        for (j = 0; j < 125; j++) {}
    }
}

//void main(void) {
//    InitUART();
//    SendStr("Hello");

//    ES = 1; //�����п��ж�
//    while (1) {
//        unsigned int a;
//        for (a = 0; a < 3; a++) {
//            weichoice(a, temp[a]);
//            Delay(2);
//        }
//    }
//}

void SendByte(unsigned char dat) {
    SBUF = dat;
    while (TI == 0);
    TI = 0;
}

void SendStr(unsigned char *s) {
    while (*s != '\0') { // �����ַ���
        SendByte(*s);
        s++;
    }
}

//void UART_SER(void) interrupt 4 {
//    unsigned char Temp;
//    unsigned char temp0;

//    if (RI) {
//        RI = 0;
//        Temp = SBUF; // ���յ�������
//        SBUF = Temp; // ���ڻ���
//        temp0 = Temp; // ��ʱ�洢���յ����ֽ�
//        tempcount(temp0); // ���� temp ����
//    }
//    if (TI) {
//        TI = 0;
//    }
//}
