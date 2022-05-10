
#include "typedef.h"
#include "bootloader.h"

//#define	__DEBUG


uint32_t const identyty[ID_NUMS] =
{
	0x000000B5,
	0x00000000,
	0x00000000,
	0x00000000,
};

// ע��:�����汾�ĸ�ʽΪ "vX.XX",�ڲ����Ǽ�Ӳ��������ʹ��,�����������
uint8_t const version_uea_G741[SW_SIZE] = "V1.0A";
uint8_t const version_uea_G742[SW_SIZE] = "V1.4B";

uint8_t const hardware_uea_G741[HW_SIZE] = "V1.3";	
uint8_t const device_name_G741[DV_SIZE] = {
	'V', 'G', 'S', '5', '0', '2'
};

uint8_t const hardware_uea_G742[HW_SIZE] = "V1.1";	
uint8_t const device_name_G742[DV_SIZE] = {
	'G', '-', '7', '4', '2',
};

uint16_t const version_fver = FVER_G_741;

#ifdef __DEBUG
uint32_t const version_chksum = DEBUG_CHKSUM;
#else
uint32_t const version_chksum = 0xffffffff;
#endif

