#include "key.h"
#include "reg52.h"
#include "delay.h"

// ���Ŷ���
sbit R1 = P1^5;    // ������̵�3����
sbit R2 = P1^4;
sbit R3 = P1^3;

// �������������ʾ�ַ�1-9

// ��ʼ����ʾ����
void init_display(void) {
    P3 = 0xff;    // ����
    P2 = 0x01;    // ����WX0
    P3 = 0x7f;    // ����
    P2 = 0xff;
}

// �������ɨ�躯�������ذ��µļ�ֵ��1-9�������û�а�������0
uchar scan_keyboard(void) {
    uchar temp;
    uchar i;
    uchar key = 0;
    
    P1 = 0xfb;    // ��ɨ���ֵ1111 1011
    
    for(i = 0; i <= 2; i++) {
        if(R1 == 0) {
            delay_ms(20);    // ����
            key = i + 1;  // ��һ�а���ֵ��1,2,3
        }
        if(R2 == 0) {
            delay_ms(20);
            key = i + 4;  // �ڶ��а���ֵ��4,5,6
        }
        if(R3 == 0) {
            delay_ms(20);
            key = i + 7;  // �����а���ֵ��7,8,9
        }
        
        delay_ms(100);
        temp = P1;
        temp = temp | 0xf0;
        temp = temp >> 1;
        temp = temp | 0xf0;
        P1 = temp;
    }
    
    return key;
}
