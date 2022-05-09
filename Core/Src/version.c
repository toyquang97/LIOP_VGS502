
#include "typedef.h"
#include "bootloader.h"

//#define	__DEBUG


DWORD const identyty[ID_NUMS] =
{
	0x000000B5,
	0x00000000,
	0x00000000,
	0x00000000,
};

// ע��:�����汾�ĸ�ʽΪ "vX.XX",�ڲ����Ǽ�Ӳ��������ʹ��,�����������
BYTE const version_uea_G741[SW_SIZE] = "V1.0A";
BYTE const version_uea_G742[SW_SIZE] = "V1.4B";

BYTE const hardware_uea_G741[HW_SIZE] = "V1.3";	
BYTE const device_name_G741[DV_SIZE] = {
	'V', 'G', 'S', '5', '0', '2'
};

BYTE const hardware_uea_G742[HW_SIZE] = "V1.1";	
BYTE const device_name_G742[DV_SIZE] = {
	'G', '-', '7', '4', '2',
};

WORD const version_fver = FVER_G_741;

#ifdef __DEBUG
DWORD const version_chksum = DEBUG_CHKSUM;
#else
DWORD const version_chksum = 0xffffffff;
#endif

