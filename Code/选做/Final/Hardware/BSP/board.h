#ifndef __BOARD_H
#define __BOARD_H

#include "stm32f10x.h"

/**********************************************************
***	Emm_V5.0�����ջ���������
***	��д���ߣ�ZHANGDATOU
***	����֧�֣��Ŵ�ͷ�ջ��ŷ�
***	�Ա����̣�https://zhangdatou.taobao.com
***	CSDN���ͣ�http s://blog.csdn.net/zhangdatou666
***	qq����Ⱥ��262438510
**********************************************************/

void clock_init(void);
void gpio_init(void);
void board_init(void);
void rotate90_degrees(void);
void rotate90_degrees_reverse(void);
void close_door(void);
#endif
