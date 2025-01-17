#include "stm32f10x.h"                  // �豸ͷ�ļ�
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Menu.h"
#include "RC522.h"
#include "Flash.h"
#include "timer.h"
#include "bluetooth.h"
#include "Servo.h"
#include "board.h"
#include <stdbool.h>        // ��������֧��

uint8_t inPasswordMode = 0; // �Ƿ��������ģʽ
uint8_t inPasswordMode_PD_Mode = 0;

uint8_t passwordPos = 0;  // ��������λ��
uint8_t password_client[4] = {0};
uint8_t correct_password[4] = {1, 1, 1, 1};  // ��ȷ���룬1, 2, 3, 4
uint8_t new_password[4]={0};
#define PASSWORD_LENGTH 4

uint8_t cardnumber, KeyNum, tempcard, select = 0, flag_scan = 0, flag_addcard = 0, flag_deletecard = 0, flag_clear = 0;
uint8_t pd_enter_flag = 0,pd_change_flag =0;
uint8_t key_last = 0, key_err = 0, stop_flag = 0;
/* ������ر��� */
uint8_t count = 0, clear_flag, reset_flag = 0;
/* ���� */
uint8_t bluetooth_password[4] = {0};  // ���ڴ洢��������������
uint8_t bt_password_pos = 0;          // ������������λ��
uint8_t bluetooth_connected = 0;       // ��������״̬
uint8_t RxData;                       // ���յ�������
uint8_t bluetooth_open_flag = 0;       // ��������״̬

#define BUFFER_SIZE 20                  // ������ջ�������С
#define PASSWORD_END_CHAR ' '           // ���������
#define MAX_ERROR_COUNT 3  // ���������
#define MAX_CLOSE_COUNT 30  // ���������

static uint8_t error_count = 0;  // �������������
uint8_t error_count_last_time = 0;  // �������������

char receivedPassword[BUFFER_SIZE];       // ���ڴ洢���յ�������
char last_receivedPassword[BUFFER_SIZE];  // ���ڴ洢��һ�ν��յ�������

volatile uint8_t passwordIndex = 0;       // ��������
volatile bool passwordReceived = false;   // ��־��ָʾ�Ƿ���յ���������


uint8_t board_move_flag = 0;;                 
uint8_t board_move_i = 0;                     
uint8_t board_move_mode = 0;;                 

uint8_t rfid_mode = 0;                    
uint8_t rfid_display_flag = 0;                      
uint8_t password_display_flag = 0;                    
uint8_t password_change_flag = 0,password_change_mode =0;                    

uint8_t blue_tooth_mode = 0;;                    
uint8_t isCorrect_change_pd ;
uint8_t first_change_key = 0 ;
uint8_t isCorrect_flag = 0;
// ��ս������뻺����
void ClearPasswordBuffer(void) {
    memset(receivedPassword, 0, sizeof(receivedPassword));
    passwordIndex = 0;
}
// ��ʼ��USART2���ж����ȼ���ʹ��
void Init_USART2_NVIC(void) {
    NVIC_InitTypeDef NVIC_InitStructure;

    // ����USART2�ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // �����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         // �����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
extern uint8_t UID[4], Temp[4];
extern uint8_t UI0[4]; // ��Ƭ0 ID����
extern uint8_t UI1[4]; // ��Ƭ1 ID����
extern uint8_t UI2[4]; // ��Ƭ2 ID����
extern uint8_t UI3[4]; // ��Ƭ3 ID����

void RFID_Check(void);
void Read_Card(void);
static uint8_t isFirstEnter = 1;
// �����־λ����ʼֵΪ 0����ʾδִ�й���Ӧ��ת��
unsigned char board_turn_flag_cw = 0;  // ˳ʱ���־λ
unsigned char board_turn_flag_ccw = 0; // ��ʱ���־λ
uint8_t pd_enter_after_change = 0;
void reset_flag_all(){
	inPasswordMode = 0;
	OLED_Clear();
	displayMainMenu();
	isFirstEnter = 1;
	rfid_display_flag = 0;
					flag_scan = 0;
				flag_addcard = 0;
				flag_deletecard = 0;
	//��־λ���
	board_move_mode = 0;             // �л������״̬
	rfid_mode = 0;
	blue_tooth_mode = 0;
	
	// ������ʱ���־λΪ1����ʾ��ʱ��ת�������
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

}



// ϵͳ��ʼ������
void system_init() {
    uint8_t stored_password[PASSWORD_LENGTH];
    LoadPasswordFromFlash(stored_password);  // ��Flash��ȡ����

    // ��������Ƿ���Ч
    uint8_t isPasswordValid = 1;
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
        if (stored_password[i] != correct_password[i]) {
            isPasswordValid = 0;  // ���벻ƥ��
            break;
        }
    }
isPasswordValid = 1;
    if (!isPasswordValid) {
        // ���������Ч��û�����룬д���ʼ����
        SavePasswordToFlash(correct_password);
        OLED_Clear();
        OLED_ShowString(1, 1, "Password set!");  // ��ʾ����������
        Delay_ms(1000);
    } else {
        // ���������Ч
			
		// ��ʼ����ɺ���ʾ"System init ok"
		OLED_Clear();
		OLED_ShowString(1, 1, "System init ok");  // ��ʾ��ʼ���ɹ�
		Delay_ms(500);
		OLED_Clear();
    }



}
int main(void)
{
    OLED_Init();       // OLED��ʼ��
    Key_Init();        // ������ʼ��
    RFID_Init();       // RFID��ʼ��
    Read_Card();       // ��ȡ��Ƭ��Ϣ
    Timer_Init();      // ��ʱ����ʼ��
	Buzzer_Init();
	board_init();
	UART2_Init();       // ��ʼ��USART2��������9600
    Init_USART2_NVIC();
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // ʹ��USART2�����ж�
	
	system_init();
	displayMainMenu(); // ��ʾ���˵�
//SavePasswordToFlash(correct_password);
	// �ϵ���ʱ2��ȴ�ZDT_X42_V2�ջ���ʼ�����
    while (1)
    {
        KeyNum = Key_Scan();  // ɨ�谴��
        key_err = KeyNum - key_last;
        key_last = KeyNum;
        if (error_count>=MAX_ERROR_COUNT) {// ����������������ʱ
            // ���ݰ���ִ�в�ͬ����
			if (isFirstEnter) {
				
				OLED_Clear();  // ����
				isFirstEnter = 0;
				inPasswordMode = 0;
				rfid_display_flag = 0;
				flag_scan = 0;
				flag_addcard = 0;
				flag_deletecard = 0;
				//��־λ���
				board_move_mode = 0;      // �л������״̬
				rfid_mode = 0;
				blue_tooth_mode = 0;
				
				// ������ʱ���־λΪ1����ʾ��ʱ��ת�������
				TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
			}			
			OLED_ShowString(1, 1, "error times");
			OLED_ShowString(2, 1, "try again ");
			error_count_last_time = MAX_CLOSE_COUNT - count;
			OLED_ShowNum(2, 11,error_count_last_time,2);
			OLED_ShowString(2,13, "s ");
			
			if(count >=MAX_CLOSE_COUNT) OLED_Clear();
			
        } 
		else {
            if (!inPasswordMode) {
                switch (KeyNum) {
                    case 1:
                        OLED_Clear();
                        inPasswordMode = 1; // �����������
						TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE); //�ر�ʱ��
                        break;
                    case 2:
                        OLED_Clear();
                        OLED_ShowString(1, 1, "RFID");
                        inPasswordMode = 2;// RFID����
						TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);

                        break;
                    case 3:
                        OLED_Clear();
                        inPasswordMode = 3;// ��������
						TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
                        break;
                }
            }
            else if (inPasswordMode == 1) {  // ��������
				
				switch(inPasswordMode_PD_Mode){
					case 0:
						if(!password_display_flag){
							password_display_flag = 1;
							OLED_ShowString(1, 1, "Keys Mode");
							OLED_ShowString(2, 1, "1.PD Enter");
							OLED_ShowString(3, 1, "2.Change PD");				
						}
						// ��������
						if (KeyNum == 1) {// ���밴�������ģʽ
							OLED_Clear();
							OLED_ShowString(1, 1, "Pls enter pd:");
							inPasswordMode_PD_Mode = 1;
						}
						if (KeyNum == 2) {// �����޸������ģʽ
							OLED_Clear();
							OLED_ShowString(1, 1, "Pls enter cur pd:");
							inPasswordMode_PD_Mode = 2;
						}
						
						if(KeyNum==4 && (pd_enter_flag != 1 && pd_change_flag !=1))	
						{
							OLED_Clear();
			
							inPasswordMode = 0;
							OLED_Clear();
							displayMainMenu();
							isFirstEnter = 1;
							password_display_flag = 0;
						}
					break;
					case 1:
						if (KeyNum >= 1 && KeyNum <= 4 && passwordPos < PASSWORD_LENGTH && !board_move_mode) {

							password_client[passwordPos] = KeyNum;
							passwordPos++;
							OLED_ShowNum(2, passwordPos + 1, KeyNum, 1);

							if (passwordPos >= PASSWORD_LENGTH) {
								uint8_t isCorrect = 1;  // ��������Ƿ���ȷ
								uint8_t flash_password[PASSWORD_LENGTH];	
								
								// �� Flash �м������뵽 correct_password
								LoadPasswordFromFlash(flash_password);
								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
									correct_password[i] = flash_password[i];  // ���� correct_password
								}
								
								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
										if (password_client[i] != correct_password[i]) {
											isCorrect = 0;  // �������
											break;
										}
									}
									Delay_ms(1000);
									OLED_Clear();
									OLED_ShowString(1, 1, "Checking...");
									Delay_ms(1000);
									OLED_Clear();
									if (isCorrect) {
										OLED_ShowString(1, 1, "Yes");
										Delay_ms(500);
										// ������ȷ��ִ����ز���
										board_move_mode = 1; // �����߼��жϽ׶�
										
									} else {
																				error_count ++; //��¼�������

										OLED_ShowString(1, 1, "No");
										uint8_t times_left = MAX_ERROR_COUNT-error_count;
										OLED_ShowNum(2,1,times_left,1);
										OLED_ShowString(2, 2, " times left");
										Delay_ms(500);
										// �������ִ����Ӧ��ʾ
										board_move_mode = 0;
										
										
										inPasswordMode = 0;
										passwordPos = 0;
										inPasswordMode_PD_Mode = 0;
										OLED_Clear();
										displayMainMenu();
										TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
										password_display_flag = 0;
									}
							}
							
						 }
						
						else if(board_move_mode == 1) {
							rotate90_degrees();// ����
						}
						else if(board_move_mode == 2) {
							// ����״̬
							inPasswordMode = 0;
							passwordPos = 0;
							inPasswordMode_PD_Mode = 0;
							board_move_mode = 0;
							OLED_Clear();
							displayMainMenu();
							TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
						}
					break;
					case 2:
						if (!password_display_flag) {
							password_display_flag = 1;
							OLED_ShowString(1, 1, "Change Password");
							OLED_ShowString(2, 1, "Pls enter cur pd:");  // ��ʾ�û����뵱ǰ����
						}

						// ���뵱ǰ����
						if (KeyNum >= 1 && KeyNum <= 4 && passwordPos < PASSWORD_LENGTH && !isCorrect_flag) {
							password_client[passwordPos] = KeyNum;  // �洢�û����������λ
							passwordPos++;
							OLED_ShowNum(2, passwordPos + 1, KeyNum, 1);  // ��ʾ���������λ
						
							// ����Ѿ������������ĵ�ǰ���룬��������У��
							if (passwordPos >= PASSWORD_LENGTH && !password_change_flag) {
								isCorrect_change_pd = 1;  // ����������ȷ�����бȶ�
								uint8_t stored_password[PASSWORD_LENGTH];
								LoadPasswordFromFlash(stored_password);  // ��Flash��ȡ����
								
								uint8_t flash_password[PASSWORD_LENGTH];	
								
								// �� Flash �м������뵽 correct_password
								LoadPasswordFromFlash(flash_password);
								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
									correct_password[i] = flash_password[i];  // ���� correct_password
								}
								
								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
									if (password_client[i] != correct_password[i]) {
										isCorrect_change_pd = 0;  // �������
										break;
									}
								}

								Delay_ms(1000);  // ��ʾ��ʱ�����û�һЩ����ʱ��
								OLED_Clear();
								OLED_ShowString(1, 1, "Checking...");  // ��ʾ������֤����
								Delay_ms(1000);
								OLED_Clear();

								if (isCorrect_change_pd) {
									// ��ǰ������ȷ�������û�����������
									passwordPos = 0;  // �������λ�ã�׼������������
									password_change_flag = 1;
									isCorrect_flag = 1;
									OLED_Clear();
									OLED_ShowString(1, 1, "Pls enter new pd");  // ��ʾ����������
									first_change_key = 1;
								} else {
									// ��ǰ����������¿�ʼ����
									OLED_ShowString(1, 1, "Incorrect password");
									Delay_ms(500);
									passwordPos = 0;  // �����������
									OLED_Clear();
									OLED_ShowString(1, 1, "Pls enter pd:");  // ��ʾ�������뵱ǰ����
								}
							}
						}

						// ���������ȷ�ĵ�ǰ���룬��������������
						if (isCorrect_change_pd && passwordPos < PASSWORD_LENGTH && password_change_flag == 1) {
							if (first_change_key == 1) {
								first_change_key = 0;
								KeyNum = 0;
								passwordPos = 0;
							}
							if (KeyNum >= 1 && KeyNum <= 4) {  // ȷ������δ������
								new_password[passwordPos] = KeyNum;  // �洢������ĵ�ǰλ
								passwordPos++;
								OLED_ShowNum(2, passwordPos + 1, KeyNum, 1);  // ��ʾ������λ
							}
						}

						// ���������������ʱ
						if (passwordPos >= PASSWORD_LENGTH && password_change_flag == 1) {
							// �������������룬����ȷ��
							password_change_flag = 2;  // ���������������ɣ�׼������ȷ��
						}

						// ���������������ʱ
						if (isCorrect_change_pd && passwordPos >= PASSWORD_LENGTH && password_change_flag == 2) {
							// �������������룬����ȷ��
							OLED_Clear();
							OLED_ShowString(1, 1, "New password set");  // ��ʾ���óɹ�����Ϣ

							// ����������
							for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
								correct_password[i] = new_password[i];  // ��������
							}

							// ��������д��Flash
							SavePasswordToFlash(correct_password);

							Delay_ms(1000);  // ��ʾ�޸ĳɹ�����ʱ1��
							OLED_Clear();
							displayMainMenu();  // �������˵�
							password_display_flag = 0;  // ���������ʾ��־
							passwordPos = 0;  // �����������λ��
							inPasswordMode_PD_Mode = 0;  // �˳������޸�ģʽ
							inPasswordMode = 0;
							password_change_flag = 0;
							isCorrect_change_pd = 0;

						}
						
//						if (!password_display_flag) {
//							password_display_flag = 1;
//							OLED_ShowString(1, 1, "Change Password");
//							OLED_ShowString(2, 1, "Pls enter cur pd:");  // ��ʾ�û����뵱ǰ����
//						}

//						// ���뵱ǰ����
//						if (KeyNum >= 1 && KeyNum <= 4 && passwordPos < PASSWORD_LENGTH && !isCorrect_flag) {
//							password_client[passwordPos] = KeyNum;  // �洢�û����������λ
//							passwordPos++;
//							OLED_ShowNum(2, passwordPos + 1, KeyNum, 1);  // ��ʾ���������λ
//							// ����Ѿ������������ĵ�ǰ���룬��������У��
//							if (passwordPos >= PASSWORD_LENGTH && !password_change_flag) {
//								isCorrect_change_pd = 1;  // ����������ȷ�����бȶ�
//								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
//									if (password_client[i] != correct_password[i]) {
//										isCorrect_change_pd = 0;  // �������
//										break;
//									}
//								}

//								Delay_ms(1000);  // ��ʾ��ʱ�����û�һЩ����ʱ��
//								OLED_Clear();
//								OLED_ShowString(1, 1, "Checking...");  // ��ʾ������֤����
//								Delay_ms(1000);
//								OLED_Clear();

//								if (isCorrect_change_pd) {
//									// ��ǰ������ȷ�������û�����������
//									passwordPos = 0;  // �������λ�ã�׼������������
//									password_change_flag = 1;
//									isCorrect_flag = 1;
//									OLED_Clear();
//									OLED_ShowString(1, 1, "Pls enter new pd");  // ��ʾ����������
//									first_change_key = 1;
//								} else {
//									// ��ǰ����������¿�ʼ����
//									OLED_ShowString(1, 1, "Incorrect password");
//									Delay_ms(500);
//									passwordPos = 0;  // �����������
//									OLED_Clear();
//									OLED_ShowString(1, 1, "Pls enter pd:");  // ��ʾ�������뵱ǰ����
//								}
//							}
//						}

//							// ���������ȷ�ĵ�ǰ���룬��������������
//							if (isCorrect_change_pd && passwordPos < PASSWORD_LENGTH && password_change_flag == 1) {
//								if(first_change_key == 1){
//									first_change_key = 0;
//									KeyNum = 0;
//									passwordPos = 0;
//								}
//								if (KeyNum >= 1 && KeyNum <= 4) {  // ȷ������δ������
//									new_password[passwordPos] = KeyNum;  // �洢������ĵ�ǰλ
//									passwordPos++;
//									OLED_ShowNum(2, passwordPos + 1, KeyNum, 1);  // ��ʾ������λ
//								}
//							}

//							// ���������������ʱ
//							if (passwordPos >= PASSWORD_LENGTH && password_change_flag == 1) {
//								// �������������룬����ȷ��
//								password_change_flag = 2;  // ���������������ɣ�׼������ȷ��
//							}
//														// ���������������ʱ
//							if (isCorrect_change_pd && passwordPos >= PASSWORD_LENGTH && password_change_flag == 2) {
//								// �������������룬����ȷ��
//								OLED_Clear();
//								OLED_ShowString(1, 1, "New password set");  // ��ʾ���óɹ�����Ϣ
//								// ����������
//								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
//									correct_password[i] = new_password[i];  // ��������
//								}

//								Delay_ms(1000);  // ��ʾ�޸ĳɹ�����ʱ1��
//								OLED_Clear();
//								displayMainMenu();  // �������˵�
//								password_display_flag = 0;  // ���������ʾ��־
//								passwordPos = 0;  // �����������λ��
//								inPasswordMode_PD_Mode = 0;  // �˳������޸�ģʽ
//								inPasswordMode = 0;
//								password_change_flag = 0;
//							}
//						
//						break;
					default:
						break;
				
				}
            }
            else if (inPasswordMode == 2) {  // RFIDģʽ
				
				if(!rfid_display_flag){
					OLED_ShowString(1, 1, "RFID Mode");
					OLED_ShowString(2, 1, "1.Scan");
					OLED_ShowString(3, 1, "2.Add card");
					OLED_ShowString(4, 1, "3.delete");
					rfid_display_flag = 1;
				}

				
                // ��������
                if (KeyNum == 1) {// ����ˢ��ģʽ
                    flag_scan = 1;
                    flag_addcard = 0;
                    flag_deletecard = 0;

                }
                if (KeyNum == 2) {// ����д��ģʽ
                    flag_scan = 0;
                    flag_addcard = 1;
                    flag_deletecard = 0;

                }
                if (KeyNum == 3) {// ����ɾ��ģʽ
                    flag_scan = 0;
                    flag_addcard = 0;
                    flag_deletecard = 1;

                }
				if(KeyNum==4)	//����4���˳�ɾ��ģʽ������Ѱ��ģʽ
				{
					OLED_Clear();
					
					flag_scan=0;
					flag_addcard=0;
					flag_deletecard=0;
					
					inPasswordMode = 0;
					OLED_Clear();
					displayMainMenu();
					isFirstEnter = 1;
					rfid_display_flag = 0;
				}
					
                if (flag_scan == 1) {
                    RFID_Check();
					if(rfid_mode == 1){
//						Buzzer2();				// ������������
						rotate90_degrees();
//						WaitCardOff();			// �ȴ���Ƭ�ƿ�
//						
//						inPasswordMode = 0;
//						OLED_Clear();
//						displayMainMenu();
					}
					else if(rfid_mode == 2){
						Delay_ms(1000);
						reset_flag_all();
					}

					if(KeyNum==4)	//����4���˳�ɾ��ģʽ������Ѱ��ģʽ
					{
						OLED_Clear();
						flag_scan=0;
						flag_addcard=0;
						flag_deletecard=0;
						inPasswordMode = 0;
						OLED_Clear();
						displayMainMenu();
						isFirstEnter = 1;
						rfid_display_flag = 0;
						
					}
					
                }

                while (flag_addcard == 1) {  // ��ӿ�Ƭ
					if (isFirstEnter) {
                        OLED_Clear();  // ����
                        isFirstEnter = 0;
                    }
					
					OLED_ShowString(1, 1, "   Add Card:    ");

                    KeyNum = Key_Scan();
                    if (KeyNum == 1) {  // ���ص�ɨ��ģʽ
                        flag_scan = 1;
                        flag_addcard = 0;
                        flag_deletecard = 0;
                    }
                    if (KeyNum == 3) {  // ���ص�ɾ����Ƭģʽ
                        flag_scan = 0;
                        flag_addcard = 0;
                        flag_deletecard = 1;
                    }
					if(KeyNum==4)	//����4���˳�ɾ��ģʽ������Ѱ��ģʽ
					{
						
						flag_scan=0;
						flag_addcard=0;
						flag_deletecard=0;
						
						OLED_Clear();

						inPasswordMode = 2;
						isFirstEnter = 1;
						rfid_display_flag = 0;
					}


                    if (PcdRequest(REQ_ALL, Temp) == MI_OK) {
                        if (PcdAnticoll(UID) == MI_OK) {
                            // ��鿨ƬID�Ƿ�Ϊ���п�Ƭ
                            if (UI0[0] == 0xFF && UI0[1] == 0xFF && UI0[2] == 0xFF && UI0[3] == 0xFF)
                                tempcard = 0;
                            else if (UI1[0] == 0xFF && UI1[1] == 0xFF && UI1[2] == 0xFF && UI1[3] == 0xFF)
                                tempcard = 1;
                            else if (UI2[0] == 0xFF && UI2[1] == 0xFF && UI2[2] == 0xFF && UI2[3] == 0xFF)
                                tempcard = 2;
                            else if (UI3[0] == 0xFF && UI3[1] == 0xFF && UI3[2] == 0xFF && UI3[3] == 0xFF)
                                tempcard = 3;
                            else
                                tempcard = 4;

                            // �жϿ�Ƭ�Ƿ��Ѵ���
                            if (UID[0] == UI0[0] && UID[1] == UI0[1] && UID[2] == UI0[2] && UID[3] == UI0[3])
                                tempcard = 5;
                            if (UID[0] == UI1[0] && UID[1] == UI1[1] && UID[2] == UI1[2] && UID[3] == UI1[3])
                                tempcard = 5;
                            if (UID[0] == UI2[0] && UID[1] == UI2[1] && UID[2] == UI2[2] && UID[3] == UI2[3])
                                tempcard = 5;
                            if (UID[0] == UI3[0] && UID[1] == UI3[1] && UID[2] == UI3[2] && UID[3] == UI3[3])
                                tempcard = 5;

                            switch (tempcard) {
                                case 0:
                                    UI0[0] = UID[0];
                                    UI0[1] = UID[1];
                                    UI0[2] = UID[2];
                                    UI0[3] = UID[3];
                                    FLASH_W(FLASH_ADDR1, UI0[0], UI0[1], UI0[2], UI0[3]);  // д������
                                    OLED_ShowString(1, 1, "  Add Card 1 OK ");
                                    Buzzer1();
                                    WaitCardOff();  // �ȴ���Ƭ�ƿ�
                                    OLED_Clear();
                                    break;
                                case 1:
                                    UI1[0] = UID[0];
                                    UI1[1] = UID[1];
                                    UI1[2] = UID[2];
                                    UI1[3] = UID[3];
                                    FLASH_W(FLASH_ADDR2, UI1[0], UI1[1], UI1[2], UI1[3]);
                                    OLED_ShowString(1, 1, "  Add Card 2 OK ");
                                    Buzzer1();
                                    WaitCardOff();
                                    OLED_Clear();
                                    break;
                                case 2:
                                    UI2[0] = UID[0];
                                    UI2[1] = UID[1];
                                    UI2[2] = UID[2];
                                    UI2[3] = UID[3];
                                    FLASH_W(FLASH_ADDR3, UI2[0], UI2[1], UI2[2], UI2[3]);
                                    OLED_ShowString(1, 1, "  Add Card 3 OK ");
                                    Buzzer1();
                                    WaitCardOff();
                                    OLED_Clear();
                                    break;
                                case 3:
                                    UI3[0] = UID[0];
                                    UI3[1] = UID[1];
                                    UI3[2] = UID[2];
                                    UI3[3] = UID[3];
                                    FLASH_W(FLASH_ADDR4, UI3[0], UI3[1], UI3[2], UI3[3]);
                                    OLED_ShowString(1, 1, "  Add Card 4 OK ");
                                    Buzzer1();
                                    WaitCardOff();
                                    OLED_Clear();
                                    break;
                                case 4:
                                    OLED_ShowString(1, 1, "Card max!!");
                                    Buzzer1();
                                    Delay_ms(500);
                                    OLED_Clear();
                                    break;
                                case 5:
                                    OLED_ShowString(1, 1, "Card exists!");
                                    Buzzer1();
                                    Delay_ms(500);
                                    OLED_Clear();
                                    break;
                            }
                        }
                    }
                }
				
				while(flag_deletecard==1)	//ɾ��ģʽ
				{
					if (isFirstEnter) {
                        OLED_Clear();  // ����
                        isFirstEnter = 0;
                    }
					OLED_ShowString(1,1,"  Delete Card:  ");
					KeyNum = Key_Scan();
					
					if(KeyNum==4)	//����4���˳�ɾ��ģʽ������Ѱ��ģʽ
					{
						flag_scan=0;
						flag_addcard=0;
						flag_deletecard=0;
						
						OLED_Clear();

						inPasswordMode = 2;
						isFirstEnter = 1;
						rfid_display_flag = 0;
					}
					
					
					if(KeyNum==1)	//��ɾ��ģʽ�°���1����ѡ��Ҫɾ���Ŀ�Ƭ����Ƭ��������
					{
						select++;
						if(select>=4 || select<1)select=0;
					}
					if(KeyNum==2)	//��ɾ��ģʽ�°���2����ѡ��Ҫɾ���Ŀ�Ƭ����Ƭ���м���
					{
						select--;
						if(select>=4 || select<1)select=0;
					}
					
					
					switch (select)
					{
						case 0:
						{
							OLED_ShowString(2,1," Delete Card 1 ?");
							if(KeyNum==3)	//��ɾ��ģʽ�°���3����ɾ����Ӧ�Ŀ�Ƭ
							{
								FLASH_Clear(FLASH_ADDR1);	
								UI0[0]=0xFF;
								UI0[1]=0xFF;
								UI0[2]=0xFF;
								UI0[3]=0xFF;
								OLED_ShowString(2,1,"Clear Card 1 OK ");
								Buzzer1();	//ɾ���ɹ����������һ��
								Delay_ms(1500);
							}
							else if(KeyNum==4){
								OLED_Clear();
								break;
							}
						}break;
						case 1:
						{
							OLED_ShowString(2,1," Delete Card 2 ?");
							if(KeyNum==3)
							{
								FLASH_Clear(FLASH_ADDR2);
								UI1[0]=0xFF;
								UI1[1]=0xFF;
								UI1[2]=0xFF;
								UI1[3]=0xFF;
								OLED_ShowString(2,1,"Clear Card 2 OK ");
								Buzzer1();
								Delay_ms(1500);
							}
							else if(KeyNum==4){
								OLED_Clear();
								break;
							}
						}break;
						case 2:
						{
							OLED_ShowString(2,1," Delete Card 3 ?");
							if(KeyNum==3)
							{
								FLASH_Clear(FLASH_ADDR3);
								UI2[0]=0xFF;
								UI2[1]=0xFF;
								UI2[2]=0xFF;
								UI2[3]=0xFF;
								OLED_ShowString(2,1,"Clear Card 3 OK ");
								Buzzer1();
								Delay_ms(1500);;
							}
							else if(KeyNum==4){
								OLED_Clear();
								break;
							}
						}break;
						case 3:
						{
							OLED_ShowString(2,1," Delete Card 4 ?");
							if(KeyNum==3)
							{
								FLASH_Clear(FLASH_ADDR4);
								UI3[0]=0xFF;
								UI3[1]=0xFF;
								UI3[2]=0xFF;
								UI3[3]=0xFF;
								OLED_ShowString(2,1,"Clear Card 4 OK ");
								Buzzer1();
								Delay_ms(1500);
							}
							else if(KeyNum==4){
								OLED_Clear();
								break;
							}
						}break;
						default:break;
					}
				}
            }
			else if (inPasswordMode == 3) {  // ����ģʽ
				if (isFirstEnter) {
                    OLED_Clear();  // ����
                    isFirstEnter = 0;
                    OLED_ShowString(1, 1, "Connecting...");
                }
                if (passwordReceived && !blue_tooth_mode) {
                    // �ȽϽ��յ��������Ƿ���ȷ
                    if (strcmp(receivedPassword, "1234") == 0) {
                        // ������ȷ
                        OLED_Clear();
                        OLED_ShowString(1, 1, "Yes");
                        // ��ʾ Opening door
                        OLED_ShowString(2, 1, "Opening door...");
						// �ر�USART2�����ж�
						USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); 
						blue_tooth_mode = 1;	
                  }
					//�������
					else {
						// �������
						OLED_Clear();
						OLED_ShowString(1, 1, "No");
						error_count ++;
						// ���� TIM2 �жϹر�
						Delay_ms(2000);
						OLED_Clear();
						OLED_ShowString(1, 1, "Pls try again!");
						uint8_t err_left = MAX_ERROR_COUNT-error_count;
						OLED_ShowNum(2, 1, err_left,1);
						OLED_ShowString(2, 2, " times left");
						TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
					}

                    // ����last_receivedPassword����ѡ����������
                    strncpy(last_receivedPassword, receivedPassword, BUFFER_SIZE);
                    last_receivedPassword[BUFFER_SIZE - 1] = '\0'; // ȷ���ַ�������

                    // ���ñ�־
                    passwordReceived = false;

                    // ������뻺������׼���´ν���
                    ClearPasswordBuffer();

			}
				else if(blue_tooth_mode == 1){
					rotate90_degrees();
				}
				else if(blue_tooth_mode == 2){
											
					Delay_ms(1500);
					// ��ѡ����ʾ�������
					OLED_ShowString(3, 1, "Door Opened");
					Delay_ms(2000);
					OLED_Clear();

					blue_tooth_mode = 3;
					
				}
				else if(blue_tooth_mode == 3){
					inPasswordMode = 0;
                    passwordReceived = false;
					blue_tooth_mode = 0;
					OLED_Clear();
					displayMainMenu();
					// ���¿��� TIM2 �ж�
					TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
					USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // ʹ��USART2�����ж�
					isFirstEnter=1;
				}

		}
        }
		

//		count = 3;
		if(clear_flag == 1 ){
			if(board_turn_flag_cw  == 1 && !board_turn_flag_ccw){//�����ʱ������û��
//				TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
//				USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); // ʹ��USART2�����ж�
//				board_turn_flag_cw = 1;//��־λ��1 �����Ѿ�����
				rotate90_degrees_reverse();//˳ʱ��
			}
			else OLED_Clear();
			
			if(KeyNum){
				clear_flag = 0;
				reset_flag = 1;
				//���ñ�־λ
				inPasswordMode = 0;
			}

		}
		else if(error_count <MAX_CLOSE_COUNT)OLED_ShowNum(4, 15, count, 2);
		
		if(count>=MAX_CLOSE_COUNT && error_count <MAX_ERROR_COUNT){ //����ʱ��� ���� ���°���
			count =0; 
			clear_flag = 1;
			if(board_turn_flag_cw  == 1 && !board_turn_flag_ccw && count>=MAX_CLOSE_COUNT){//�����ʱ������û��
//				TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
//				USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); // ʹ��USART2�����ж�
//				board_turn_flag_cw = 1;//��־λ��1 �����Ѿ�����
				rotate90_degrees_reverse();//����
		}
		}
		else if(count >=MAX_CLOSE_COUNT && error_count>=MAX_ERROR_COUNT){ // �����������5�� 60�������
			error_count = 0;
			count = 0;
			displayMainMenu();
		}
		else {
			if(KeyNum) count = 0; //������°�������
		}
    }
}


// ���RFID��Ƭ�ĺ�������ȡ��ƬID
void RFID_Check()									
{
	if(!rfid_mode){
		cardnumber = Rc522Test();	// ��ȡ��Ƭ���
		if (isFirstEnter) {
			OLED_Clear();  // ����
			isFirstEnter = 0;
		}	
		if(cardnumber == 0)			// �����Ƭ��Ч������0��������ʾ������Ϣ
		{
			OLED_ShowString(1,1,"   Error card   ");
			Buzzer_Alarm();			// ����������
			WaitCardOff();			// �ȴ���Ƭ�ƿ�
		}
		// �����Ƭ�����1��4����ʾ��Ч��Ƭ
		else if((cardnumber == 1 || cardnumber == 2 || cardnumber == 3 || cardnumber == 4))
		{
			OLED_Clear();
			OLED_ShowString(1,1,"The CardID is:  ");
			OLED_ShowNum(1,15,cardnumber,2);
			OLED_ShowString(3,1,"opening...");
			Delay_ms(500);
			rfid_mode = 1; // ���ű�־λ
		}
		else 		OLED_ShowString(3,3,"Scanning...");
	}
}

// ��Flash�ж�ȡ��ƬID�����ڴ洢��ƬID�������ַ��
void Read_Card()
{
    UI0[0] = FLASH_R(FLASH_ADDR1);
    UI0[1] = FLASH_R(FLASH_ADDR1 + 2);
    UI0[2] = FLASH_R(FLASH_ADDR1 + 4);
    UI0[3] = FLASH_R(FLASH_ADDR1 + 6);
	
    UI1[0] = FLASH_R(FLASH_ADDR2);
    UI1[1] = FLASH_R(FLASH_ADDR2 + 2);
    UI1[2] = FLASH_R(FLASH_ADDR2 + 4);
    UI1[3] = FLASH_R(FLASH_ADDR2 + 6);
	
    UI2[0] = FLASH_R(FLASH_ADDR3);
    UI2[1] = FLASH_R(FLASH_ADDR3 + 2);
    UI2[2] = FLASH_R(FLASH_ADDR3 + 4);
    UI2[3] = FLASH_R(FLASH_ADDR3 + 6);
	
    UI3[0] = FLASH_R(FLASH_ADDR4);
    UI3[1] = FLASH_R(FLASH_ADDR4 + 2);
    UI3[2] = FLASH_R(FLASH_ADDR4 + 4);
    UI3[3] = FLASH_R(FLASH_ADDR4 + 6);
}

// USART2�жϷ������
void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET && inPasswordMode==3) {
        char receivedChar = USART_ReceiveData(USART2);  // ��ȡ���յ�������

        // ����Ƿ���յ�������
        if (receivedChar == PASSWORD_END_CHAR) {
            receivedPassword[passwordIndex] = '\0';  // ����ַ���������
            passwordReceived = true;                 // ���ñ�־
            passwordIndex = 0;                       // ��������
        } else {
            if (passwordIndex < BUFFER_SIZE - 1) {   // ��黺�����Ƿ����㹻�ռ�
                receivedPassword[passwordIndex++] = receivedChar;  // �洢���յ����ַ�
            } else {
                // ��������������ջ�����
                ClearPasswordBuffer();
                // ��OLED����ʾ������Ϣ
                OLED_ShowString(4, 1, "Password Too Long!");
            }
        }
    }
}

void TIM2_IRQHandler(void)//
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET )
	{
		if(key_err==0&&stop_flag==0)//���û�м�⵽��������
		count++;//��ʱ����
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}


