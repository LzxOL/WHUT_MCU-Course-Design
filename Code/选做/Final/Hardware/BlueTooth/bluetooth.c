#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include "LED.h"
#include "OLED.h"
#define BUFFER_SIZE 20                  // ������ջ�������С



// ȫ�ֱ���
volatile uint8_t Serial_RxData = 0;  // �洢���յ�������
volatile uint8_t Serial_RxFlag = 0;  // ���ձ�־

// UART2 ��ʼ������
// ���ڳ�ʼ������
void UART2_Init(void) {
    // ����GPIOA��USART2ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // ����USART2 TX (PA2) �� RX (PA3)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  // TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;  // RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ����USART2
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;  // ���ò�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 8λ����λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  // 1��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;  // ����żУ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // ��ʹ��Ӳ��������
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // �շ�ģʽ
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);  // ����USART2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  // ʹ�ܽ����ж�

    // �����ж�
    NVIC_EnableIRQ(USART2_IRQn);  // ����USART2�ж�
}

// ����һ���ֽ�
void USART_SendByte(uint8_t Byte)
{
    USART_SendData(USART2, Byte);
    while (USART_GetFlagStatus(USART2, USART_IT_TXE) == RESET);  // �ȴ��������
}

// �����ֽ�����
void USART_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++)
    {
        USART_SendByte(Array[i]);
    }
}

// �����ַ���
void USART_SendString(char *String)
{
    uint16_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        USART_SendByte(String[i]);
    }
}

// ���� X �� Y �η�
uint32_t USART_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

// ��������
void USART_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        USART_SendByte(Number / USART_Pow(10, Length - i - 1) % 10 + '0');
    }
}

// �ض��� printf �� USART2
int fputc(int ch, FILE *f)
{
    USART_SendByte(ch);
    return ch;
}



// ��ȡ���ձ�־
uint8_t Serial_GetRxFlag(void)
{
    return Serial_RxFlag;
}

// ��ȡ���յ�������
uint8_t Serial_GetRxData(void)
{
    Serial_RxFlag = 0;  // ������ձ�־
    return Serial_RxData;
}
