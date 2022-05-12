/*
 * flash.h
 *
 *  Created on: Dec 8, 2021
 *      Author: User
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_
#define DATA_START_ADDRESS 		 	((uint32_t)0x0801FC00)	//Page 127
#include "AllHeader.h"
#ifdef _FLASH_C


void deleteBuffer(char* data);
void 	Flash_Lock(void);
void 	Flash_Unlock(void);
void 	Flash_Erase(uint32_t addr);
void 	Flash_Write_Int(uint32_t addr, int data);
uint16_t Flash_Read_Int(uint32_t addr);
void 	Flash_Write_Char(uint32_t addr, char* data);
void 	Flash_ReadChar(char* dataOut, uint32_t addr1, uint32_t addr2);
void 	Flash_ProgramPage(char* dataIn, uint32_t addr1, uint32_t addr2);

void Flash_Write_Bytes(uint32_t addr, uint8_t* data,uint16_t len);
void Flash_Read_Bytes(uint8_t* dataOut, uint32_t addr1, uint16_t len);
#else
extern void deleteBuffer(char* data);
extern void 	Flash_Lock(void);
extern void 	Flash_Unlock(void);
extern void 	Flash_Erase(uint32_t addr);
extern void 	Flash_Write_Int(uint32_t addr, int data);
extern uint16_t Flash_Read_Int(uint32_t addr);
extern void 	Flash_Write_Char(uint32_t addr, char* data);
extern void 	Flash_ReadChar(char* dataOut, uint32_t addr1, uint32_t addr2);
extern void 	Flash_ProgramPage(char* dataIn, uint32_t addr1, uint32_t addr2);

extern void Flash_Write_Bytes(uint32_t addr, uint8_t* data,uint16_t len);
extern void Flash_Read_Bytes(uint8_t* dataOut, uint32_t addr1, uint16_t len);
#endif
#endif /* INC_FLASH_H_ */
