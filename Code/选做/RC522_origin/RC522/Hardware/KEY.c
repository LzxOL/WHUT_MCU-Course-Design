#include "stm32f10x.h"                  // Device header
#include "Delay.h"

// ��ʼ������ GPIO
void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// ��ⵥ������״̬
uint8_t Key_Scan(void)
{
    uint8_t keynum = 0; // ��ʼ���������

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 1)
    {
        Delay_ms(20); // ������
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 1)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 1); // �ȴ������ɿ�
            keynum = 1;
        }
    }
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1);
            keynum = 2;
        }
    }
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 1)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 1)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 1);
            keynum = 3;
        }
    }
    else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 1)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 1)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 1);
            keynum = 4;
        }
    }

    return keynum;
}
