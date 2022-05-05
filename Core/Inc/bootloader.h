
#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#include "AllHeader.h"

#define APP_UP_ENTRY		0xc0				//app��bootloader��ڵ�ַ
#define APP_START_ADDR		0x700				//app��ʼ��ַ
#define	APP_END_ADDR		0x4000				//app������ַ

#define NEW_INT_HIGH		0x708				//app�� �����ȼ��ж�ӳ���ַ
#define NEW_INT_LOW			0x718				//app�� �����ȼ��ж�ӳ���ַ

#define IDENTITY_ADDR		0x7FD8
#define ID_NUMS				4
#define SW_VERSION_ADDR		0x7FE8
#define SW_SIZE				6
#define GW_VERSION_ADDR		0x7FEE
#define HW_SIZE				6
#define DEVICE_NAME_ADDR	0x7FF4
#define DV_SIZE				6
#define FVER_ADDR			0x7FFA
#define FVER_SIZE			2
#define CHECKSUM_ADDR		0x7FFC

#define	DEBUG_CHKSUM		0xAA55A55A

#define	FVER_G_741			0xe741				// G-586

#endif
