
#define _DISPLAY_C_
#include "Display.h"
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
uint8_t SPI_SendOneByte(uint8_t dat)
{

	SCK = 0;
	__NOP();
	for (int i = 7; i >= 0; i--)
	{
		PIN_A7 = (dat >> i) & 0x01;
		__NOP();
		SCK = 1;
		__NOP();
		__NOP();
		SCK = 0;
		__NOP();
		__NOP();
		__NOP();
	}

	return dat;
}

/************************************************************************************************/
/* 	Set node ID												                        			*/
/************************************************************************************************/

void SetNodeId(void)
{
	static uint8_t buffer[3] = {0, 0, 0};
	uint8_t bufferShowSetNodeId[4] = {0};
	if ((disp_id != node_id) || (setid_mode_old != setid_mode))
	{
		if ((setid_mode == 1) || (setid_mode == 4))
		{ //�ֹ�����
			display[BUF_ARROW] = NO_ARROW;
			display[BUF_MESSAGE] = 0;
			if (node_id >= ESE_ID && node_id < ESE_ID + MAX_ESE)
			{
				// display[BUF_TEN] = (((node_id - ESE_ID + 1) / 10) % 10);
				// display[BUF_UNIT] = ((node_id - ESE_ID + 1) % 10);
				bufferShowSetNodeId[BUF_TEN] = (((node_id - ESE_ID + 1) / 10) % 10) + 48;
				bufferShowSetNodeId[BUF_UNIT] = ((node_id - ESE_ID + 1) % 10) + 48;
				for (uint8_t i = 0; i < 36; i++)
				{
					if (bufferShowSetNodeId[BUF_TEN] == tDisp_FloorAscii[i])
					{
						display[BUF_TEN] = i;
					}
					if (bufferShowSetNodeId[BUF_UNIT] == tDisp_FloorAscii[i])
					{
						display[BUF_UNIT] = i;
					}
				}
				if ((node_id - ESE_ID + 1) >= 100)
				{
					if (hardware_version == G_741_LCD)
						display[BUF_ARROW] = DIRECTION_UP;
					else
						display[BUF_ARROW] = '1' - '0';
				}
			}
			else
			{
				display[BUF_TEN] = 33;	// x
				display[BUF_UNIT] = 33; // x
			}
		}
		else
		{
			display[BUF_ARROW] = id_buff[BUF_ARROW];
			// display[BUF_TEN] = id_buff[BUF_TEN] - '0';
			// display[BUF_UNIT] = id_buff[BUF_UNIT] - '0';
			display[BUF_TEN] = showNodeId[BUF_TEN];
			display[BUF_UNIT] = showNodeId[BUF_UNIT];
			display[BUF_MESSAGE] = 0;
		}
		disp_id = node_id;
		setid_mode_old = setid_mode;
		buffer[BUF_ARROW] = display[BUF_ARROW];
		buffer[BUF_TEN] = display[BUF_TEN];
		buffer[BUF_UNIT] = display[BUF_UNIT];
	}

	if (!flashtimer)
	{
		// INTCONbits.TMR0IE = 0;
		// HAL_TIM_Base_Stop_IT(&htim2);

		flashtimer = 3;
		if ((display[BUF_ARROW] != NO_ARROW) || (display[BUF_TEN] != NO_ARROW) || (display[BUF_UNIT] != NO_ARROW))
		{
			display[BUF_ARROW] = NO_ARROW;
			display[BUF_TEN] = NO_ARROW;
			display[BUF_UNIT] = NO_ARROW;
			display[BUF_MESSAGE] = 0;
		}
		else
		{
			display[BUF_ARROW] = buffer[BUF_ARROW];
			display[BUF_TEN] = buffer[BUF_TEN];
			display[BUF_UNIT] = buffer[BUF_UNIT];
			display[BUF_MESSAGE] = 0;
		}
		// INTCONbits.TMR0IE = 1;
		// HAL_TIM_Base_Start_IT(&htim2);
	}
}

/************************************************************************************************/
/* 	PCB test mode											                        			*/
/************************************************************************************************/
void TestMode(void)
{
	uint8_t i;
	static uint8_t oldin = 0;

	if (timer_1S5)
	{
		if (HAL_GPIO_ReadPin(NODE_ID_GPIO_Port, NODE_ID_Pin))
			;
		{ //���԰汾��
			if (hardware_version == G_741_LCD)
			{
				display[BUF_TEN] = version_uea_G741[1] - '0';
				display[BUF_UNIT] = version_uea_G741[3] - '0';
			}
			else
			{
				display[BUF_TEN] = version_uea_G742[1] - '0';
				display[BUF_UNIT] = version_uea_G742[3] - '0';
			}
			display[BUF_ARROW] = NO_ARROW;
			scroll = 0;
			display_timer = 10;
		}
		if (!display_timer)
			Display_test();
		if (in != oldin)
		{
			if ((in & 0x0f) != 0x0f)
			{
				for (i = 0; i < 4; ++i)
				{
					if (in & (1 << i))
					{
						// PIE1bits.TMR1IE = 0;
						// HAL_TIM_Base_Stop_IT(&htim4);
						scroll = 0;
						display_timer = 10;
						out = 1 << i;
						switch (i)
						{
						case 0:
							display[BUF_ARROW] = DIRECTION_UP;
							display[BUF_TEN] = 'U' - '7';
							display[BUF_UNIT] = 'P' - '7';
							break;

						case 1:
							display[BUF_ARROW] = DIRECTION_DN;
							display[BUF_TEN] = 'D' - '7';
							display[BUF_UNIT] = 'N' - '7';
							break;

						case 2:
							display[BUF_ARROW] = NO_ARROW;
							display[BUF_TEN] = 'F' - '7';
							display[BUF_UNIT] = 'F' - '7';
							out = 8;
							break;

						case 3:
							display[BUF_ARROW] = NO_ARROW;
							display[BUF_TEN] = 'L' - '7';
							display[BUF_UNIT] = 'L' - '7';
							out = 4;
							break;
						}
						// PIE1bits.TMR1IE	= 1;
						// HAL_TIM_Base_Start_IT(&htim4);
					}
					else
					{
						if (i < 2)
							bit_reset(out, i);
						else if (i == 2)
							bit_reset(out, 3);
						else
							bit_reset(out, 2);
					}
				}
			}
			oldin = in;
		}
		timer_1S5 = 0;
	}
}

void Display_test(void)
{
	uint8_t number = 0;
	uint8_t buf[4];

	if (hardware_version == G_741_LCD)
		number = 37;
	else
		number = 13;
	// HAL_TIM_Base_Stop_IT(&htim4);
	// PIE1bits.TMR1IE = 0;
	if (testno < number)
	{
		if (hardware_version == G_741_LCD)
		{
			buf[BUF_ARROW] = NO_ARROW;
			buf[BUF_TEN] = NO_FLOOR;
			buf[BUF_UNIT] = NO_FLOOR;
			buf[BUF_MESSAGE] = 0;

			if (testno < 32)
				buf[(testno / 16) + 1] = NUM_TEST_DATA + testno;
			else if (testno < 34)
				buf[BUF_ARROW] = NUM_TEST_DATA + testno - 32;
			else if (testno < 36)
				buf[BUF_MESSAGE] = tFloor_Tab[88 + testno - 34];
			if (testno == number - 1)
			{ //���һ������ʾȫ��
				buf[BUF_ARROW] = NUM_TEST_DATA + testno;
				buf[BUF_TEN] = NUM_TEST_DATA + testno + 1;
				buf[BUF_UNIT] = NUM_TEST_DATA + testno + 1;
				buf[BUF_MESSAGE] = Char_FULL + Char_PARK;
			}
			display[BUF_TEN] = buf[BUF_TEN];
			display[BUF_UNIT] = buf[BUF_UNIT];
			display[BUF_ARROW] = buf[BUF_ARROW];
			display[BUF_MESSAGE] = buf[BUF_MESSAGE];
		}
		else
		{
			// display[BUF_ARROW] = testdisplay[testno][2];
			// display[BUF_TEN] = testdisplay[testno][0];
			// display[BUF_UNIT] = testdisplay[testno][1];
		}
		if (testno == number - 1)
			display_timer = 10;
	}
	else if (testno < number + 64)
	{
		if (testno % 3)
		{
			display[BUF_ARROW] = DIRECTION_UP;
			scroll = 1;
		}
		else
		{
			display[BUF_ARROW] = NO_ARROW;
			scroll = 0;
		}

		display[BUF_TEN] = ((testno - number) / 10);
		display[BUF_UNIT] = ((testno - number) % 10);
		if (display[BUF_TEN] == 0)
			display[BUF_TEN] = NO_FLOOR;
	}
	else if (testno < number + 64 + 64)
	{
		if (testno % 3)
		{
			display[BUF_ARROW] = DIRECTION_DN;
			scroll = 2;
		}
		else
		{
			display[BUF_ARROW] = NO_ARROW;
			scroll = 0;
		}

		display[BUF_TEN] = ((64 - (testno - number - 64)) / 10);
		display[BUF_UNIT] = ((64 - (testno - number - 64)) % 10);
		if (display[BUF_TEN] == 0)
			display[BUF_TEN] = NO_FLOOR;
	}
	else
		testno = number - 1;
	testno++;
	flashcontent = display[BUF_ARROW];
	// PIE1bits.TMR1IE = 1;
	// HAL_TIM_Base_Start_IT(&htim4);
}

void KeyScan(void)
{
	uint8_t i, j = 0x00;
	uint8_t key_code, value = 0;

	i = (uint8_t)((GPIOB->IDR >> 11) & 0x13);
	key_code = (i >> 2) | (i & 0x03);
	if (!key_code)
	{ //�޼�����
		mKeyValue_Back = NO_KEY;
		bKey_Fg.KeyRepeat = bKey_Fg.KeyRepeatEn = bKey_Fg.LongKeyEn = bKey_Fg.KeyAnswer = 0;
		mLongKeyTime = 0x00;
	}
	else
	{ //�жϾ���İ���
		value = key_code;
		for (i = 0; i < 3; i++)
		{
			if (value & BIT_0)
				++j;
			value >>= 1;
		}
		if (j > 3)
		{ /// 3�ؼ���Ч
			bKey_Fg.KeyTrue = false;
			return;
		}
		for (i = 0; i < 6; i++)
		{
			if (key_code == NO_KEY)
				return;
			if (key_code == tKeyCode[i])
				break;
		}
		//ȥ����
		if (mKeyValue != i + 1)
		{
			mKeyValue = i + 1;
			return;
		}

		if (mKeyValue != mKeyValue_Back)
		{
			mKeyValue_Back = mKeyValue;
			if (mKeyValue_Back == KEYVALUE_SYSSET)
				mLongKeyTime = LONGKEYTIME_SET;
			else
				mLongKeyTime = LONGKEYTIME_SET;

			bKey_Fg.KeyTrue = 1;
			bKey_Fg.KeyRepeat = bKey_Fg.KeyRepeatEn = 0;
		}
		if (mKeyValue == KEYVALUE_SYSSET)
		{
			if (bKey_Fg.LongKeyEn)
			{
				bKey_Fg.LKeyScanEnd = false;
				bKey_Fg.KeyTrue = true;
			}
		}
	}
}

void KeyProg(const uint8_t value)
{

	if (!bKey_Fg.KeyTrue)
		return;
	switch (value)
	{
	case KEYVALUE_SYSSET:
		if (bKey_Fg.LongKeyEn)
		{
			bKey_Fg.LongKeyEn = 0;
			if (!setid_mode)
			{
				if (!mSwitch_code)
				{ // ID setting mode
					mSwitch_code = 0;
					setid_mode = 1;
					setid_mode_old = 0;
					scroll = 0;
					arrowflash = 0;
				}
			}
			else if (!mSwitch_code)
			{ // Test mode
				mSwitch_code = 0;
				setid_mode = 0;
				setid_mode_old = 0;
				arrowflash = arrowflash_old;
			}
		}
		break;

	case KEYVALUE_UP:
		if (setid_mode)
		{
			flashtimer = 3;
			if ((setid_mode == 1) || (setid_mode == 4))
			{
				keytimer = ID_SET_TIME;
				if (node_id >= ESE_ID && node_id < ESE_ID + MAX_ESE)
				{
					++node_id;
					if (node_id >= ESE_ID + MAX_ESE)
						node_id = ESE_ID;
				}
				else
					node_id = ESE_ID;
				preset_node_id = node_id;
				// write_eeprom (EE_NODEID_ADDR, node_id);
				Flash_Write_Bytes(DATA_START_ADDRESS, (uint8_t *)&node_id, sizeof(node_id));
			}
			else if (setid_mode == 2)
			{
				keytimer = 0;
				preset_node_id = node_id;
				// write_eeprom (EE_NODEID_ADDR, node_id);
				Flash_Write_Bytes(DATA_START_ADDRESS, (uint8_t *)&node_id, sizeof(node_id));
				lss_response(SET_NODE_ID, node_id);
			}
		}
		break;

	case KEYVALUE_DOWN:
		if (setid_mode)
		{
			flashtimer = 3;
			if ((setid_mode == 1) || (setid_mode == 4))
			{
				keytimer = ID_SET_TIME;
				if (node_id >= ESE_ID && node_id < ESE_ID + MAX_ESE)
				{
					if (node_id == ESE_ID)
						node_id = ESE_ID + MAX_ESE - 1;
					else
						--node_id;
				}
				else
					node_id = ESE_ID;
				preset_node_id = node_id;
				Flash_Write_Bytes(DATA_START_ADDRESS, (uint8_t *)&node_id, sizeof(node_id));
				// write_eeprom (EE_NODEID_ADDR, node_id);
			}
			else if (setid_mode == 2)
			{
				keytimer = 0;
				preset_node_id = node_id;
				Flash_Write_Bytes(DATA_START_ADDRESS, (uint8_t *)&node_id, sizeof(node_id));
				// write_eeprom (EE_NODEID_ADDR, node_id);
				lss_response(SET_NODE_ID, node_id);
			}
		}
		break;
	}
	bKey_Fg.KeyTrue = 0;
}
