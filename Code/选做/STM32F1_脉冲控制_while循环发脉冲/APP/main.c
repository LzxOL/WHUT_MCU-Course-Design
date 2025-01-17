#include "board.h"
#include "delay.h"

/**********************************************************
*** ZDT_X42_V2.0�����ջ���������
*** ��д���ߣ�ZHANGDATOU
*** ����֧�֣��Ŵ�ͷ�ջ��ŷ�
*** �Ա����̣�https://zhangdatou.taobao.com
*** CSDN���ͣ�http s://blog.csdn.net/zhangdatou666
*** qq����Ⱥ��262438510
**********************************************************/

/**
	*	@brief		MAIN����
	*	@param		��
	*	@retval		��
	*/
int main(void)
{
	int32_t i = 0;

	// ��ʼ����������
	board_init();

	// �ϵ���ʱ2��ȴ�ZDT_X42_V2�ջ���ʼ�����
	delay_ms(2000);

	// ����˳ʱ��תһȦ -> ͣ1�� -> ��ʱ��תһȦ -> ͣ1�� -> ���ѭ��...
	while(1)
	{
		// 16ϸ���·���3200�����壬���Ƶ����תһȦ
		// ����Ƶ�ʣ����ת�٣�ͨ����ʱ����delay_cnt()�ı�
		// ��Сdelay_cnt()��ֵת�ٱ�죬�Ĵ�ת�ٱ���
		GPIO_ResetBits(GPIOB, GPIO_Pin_7);  	// �����ź�����͵�ƽ�������ת
		for(i=0; i < 800; i++)             	// ���3200�������ź�
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_6);    // ����ߵ�ƽ 
			delay_cnt(1000+i*20);           					// �ߵ�ƽ����ʱ��
			GPIO_ResetBits(GPIOB, GPIO_Pin_6);	// ����͵�ƽ 
			delay_cnt(1000+i*20);           					// �͵�ƽ����ʱ��
		}

		// ��ʱ1��
		delay_ms(1000);

		// 16ϸ���·���3200�����壬���Ƶ����תһȦ
		// ����Ƶ�ʣ����ת�٣�ͨ����ʱ����delay_cnt()�ı�
		// ��Сdelay_cnt()��ֵת�ٱ�죬�Ĵ�ת�ٱ���
//		GPIO_SetBits(GPIOB, GPIO_Pin_7);    	// �����ź�����ߵ�ƽ�������ת
////		GPIO_ResetBits(GPIOB, GPIO_Pin_7);    	// �����ź�����ߵ�ƽ�������ת

//		for(i=0; i < 800; i++)             	// ���3200�������ź�
//		{
//			GPIO_SetBits(GPIOB, GPIO_Pin_6);    // ����ߵ�ƽ 
//			delay_cnt(1000+i*10);           					// �ߵ�ƽ����ʱ��
//			GPIO_ResetBits(GPIOB, GPIO_Pin_6);	// ����͵�ƽ 
//			delay_cnt(1000+i*20);           					// �͵�ƽ����ʱ��
//		}

//		// ��ʱ1��
//		delay_ms(1000);
	}
}
