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
