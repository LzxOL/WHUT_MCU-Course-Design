#ifndef __FLASH_H__
#define __FLASH_H__

#define FLASH_ADDR1 0x08007000
#define FLASH_ADDR2 0x08007400
#define FLASH_ADDR3 0x08007800
#define FLASH_ADDR4 0x08007c00
#define FLASH_ADDR5 0x08008000  
	void FLASH_W(u32 add,u8 dat1,u8 dat2,u8 dat3,u8 dat4);
	u16 FLASH_R(u32 add);
	void FLASH_Clear(u32 add);
void SavePasswordToFlash(uint8_t* password);

// ��Flash��ȡ����
void LoadPasswordFromFlash(uint8_t* password);

// ���޸�����󣬵��ñ��������뵽Flash
void UpdatePasswordInFlash(uint8_t* new_password);

#endif

