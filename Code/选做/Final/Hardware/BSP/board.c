#include "board.h"

/**********************************************************
*** ZDT_X42_V2.0�����ջ���������
*** ��д���ߣ�ZHANGDATOU
*** ����֧�֣��Ŵ�ͷ�ջ��ŷ�
*** �Ա����̣�https://zhangdatou.taobao.com
*** CSDN���ͣ�http s://blog.csdn.net/zhangdatou666
*** qq����Ⱥ��262438510
**********************************************************/

/**
	*	@brief		����ʱ�ӳ�ʼ��
	*	@param		��
	*	@retval		��
	*/
extern uint8_t board_move_i;                      // ���յ�������
extern uint8_t board_move_mode,rfid_mode,blue_tooth_mode;                      // ���յ�������

void clock_init(void)
{
	// ʹ��GPIOA�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

/**
	*	@brief		gpio���ų�ʼ��
	*	@param		��
	*	@retval		��
	*/
void gpio_init(void)
{
	// ��ʼ��PA5(En) PA6(Stp) PA7(Dir)�˿�Ϊ�������ģʽ
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// ��ʼ��PA5(En) PA6(Stp) PA7(Dir)�˿�Ϊ�͵�ƽ���
	GPIO_ResetBits(GPIOB, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
}

/**
	*	@brief		���س�ʼ��
	*	@param		��
	*	@retval		��
	*/
void board_init(void)
{
	clock_init();
	gpio_init();
}

extern unsigned char board_turn_flag_cw;  // ˳ʱ���־λ
extern unsigned char board_turn_flag_ccw; // ˳ʱ��ת������

//����
void rotate90_degrees(void) {
    static uint16_t pulse_count = 0;

    // ����Ѿ�˳ʱ��ת��90�ȣ��Ͳ���˳ʱ��ת
    if (board_turn_flag_cw == 1) {
        return;  // �Ѿ�ִ�й�˳ʱ��ת����ֱ�ӷ���
    }

    // ���õ������Ϊ��ת��˳ʱ�룩
    GPIO_ResetBits(GPIOB, GPIO_Pin_7);  // �����ź�����͵�ƽ�������ת

    // ���һ������
    GPIO_SetBits(GPIOB, GPIO_Pin_6);    // �ߵ�ƽ
    delay_cnt(100);                     // �ߵ�ƽ����ʱ��
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);  // �͵�ƽ
    delay_cnt(100);                     // �͵�ƽ����ʱ��

    // ������1
    pulse_count++;

    // ÿ10���������һ��OLED��ʾ
    if(pulse_count % 10 == 0) {
        OLED_ShowNum(3, 10, pulse_count, 3);
    }

    // ����Ƿ����800������(90��)
    if(pulse_count >= 800) {
        pulse_count = 0;                 // ���ü�����
        Delay_ms(1000);                  // ��ʱ1��
        OLED_ShowString(3, 1, "Done");
        OLED_Clear();
        board_move_mode = 2;             // �л������״̬
        rfid_mode = 2;
        blue_tooth_mode = 2;

        // ����˳ʱ���־λΪ1����ʾ˳ʱ��ת�������
        board_turn_flag_cw = 1;

    }
}

// ����ת������
void rotate90_degrees_reverse(void) {
    static uint16_t pulse_count = 0;

    // ����Ѿ���ʱ��ת��90�ȣ��Ͳ�����ʱ��ת
    if (board_turn_flag_ccw == 1) {
        return;  // �Ѿ�ִ�й���ʱ��ת����ֱ�ӷ���
    }

    // ���õ������Ϊ��ת����ʱ�룩
    GPIO_SetBits(GPIOB, GPIO_Pin_7);  // �����ź�����ߵ�ƽ�������ת

    // ���һ������
    GPIO_SetBits(GPIOB, GPIO_Pin_6);    // �ߵ�ƽ
    delay_cnt(100);                     // �ߵ�ƽ����ʱ��
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);  // �͵�ƽ
    delay_cnt(100);                     // �͵�ƽ����ʱ��

    // ������1
    pulse_count++;

    // ÿ10���������һ��OLED��ʾ
    if(pulse_count % 10 == 0) {
        OLED_ShowNum(3, 10, pulse_count, 3);
    }

    // ����Ƿ����800������(90��)
    if(pulse_count >= 800) {
        pulse_count = 0;                 // ���ü�����
        Delay_ms(1000);                  // ��ʱ1��
        OLED_ShowString(3, 1, "Done");
        OLED_Clear();
        board_move_mode = 2;             // �л������״̬
        rfid_mode = 2;
        blue_tooth_mode = 2;

        // ������ʱ���־λΪ1����ʾ��ʱ��ת�������
        board_turn_flag_ccw = 1;
    }
}
uint8_t ii =0;

void close_door(void){
	GPIO_SetBits(GPIOB, GPIO_Pin_7);  	// �����ź�����͵�ƽ�������ת
	for(ii=0; ii < 800; ii++)             	// ���3200�������ź�
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_6);    // ����ߵ�ƽ 
		delay_cnt(1000+ii*20);           					// �ߵ�ƽ����ʱ��
		GPIO_ResetBits(GPIOB, GPIO_Pin_6);	// ����͵�ƽ 
		delay_cnt(1000+ii*20);           					// �͵�ƽ����ʱ��
		if(ii % 10 == 0) {
        OLED_ShowNum(3, 10, ii, 3);
    }
	}
	if(ii>=799){
        Delay_ms(1000);                  // ��ʱ1��
        OLED_ShowString(3, 1, "Done");
        OLED_Clear();
        board_move_mode = 2;             // �л������״̬
        rfid_mode = 2;
        blue_tooth_mode = 2;

        // ����˳ʱ���־λΪ1����ʾ˳ʱ��ת�������
        board_turn_flag_cw = 1;
	}
}