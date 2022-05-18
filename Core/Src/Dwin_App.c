/*
 * Dwin_App.c
 *
 *  Created on: Dec 31, 2021
 *      Author: User
 */

#define _DWIN_C_

#include "Dwin_App.h"

extern uint8_t Callstatus[8];
extern uint8_t Callstatus_old[8];

extern UART_HandleTypeDef huart1;

void dwinUpdateImageFloor(uint16_t spAddressTen, uint16_t spAddressUint)
{
	uint8_t bufferTen[2] = {0};
	uint8_t bufferUint[2] = {0};
	static uint8_t bufferDataOld[2] = {0};
	bufferTen[1] = display[BUF_TEN];
	bufferUint[1] = display[BUF_UNIT];

	if (bufferDataOld[0] != bufferTen[1] || bufferDataOld[1] != bufferUint[1])
	{
		bufferDataOld[0] = bufferTen[1];
		bufferDataOld[1] = bufferUint[1];
		dwinWriteData(spAddressTen, bufferTen, 2);
		dwinWriteData(spAddressUint, bufferUint, 2);
	}
}
////////////////////////////////////////////

void changeLocationTEXT(uint16_t spAddress, uint16_t xAxis, uint16_t yAxis)
{
	uint8_t buffer[4];
	buffer[0] = (uint16_t)xAxis >> 8;
	buffer[1] = (uint8_t)xAxis;
	buffer[2] = (uint16_t)yAxis >> 8;
	buffer[3] = (uint8_t)yAxis;
	dwinWriteData(spAddress, buffer, 4);
}

void messageInCase(messageMode_e mode)
{
	uint8_t bufferData[2] = {0};
	bufferData[0] = 0;
	bufferData[1] = mode;
	dwinWriteData(VP_ROLLTEXT, bufferData, 2);
}

void showMessageWarning(_IconShow_e index)
{
	uint8_t bufferData[2] = {0};
	bufferData[0] = 0;
	bufferData[1] = index;
	// dwinWriteData(MESSAGE_WARNING, bufferData, 2);
	if (index == fullloadIcon)
	{
		bufferData[0] = 0;
		bufferData[1] = index - 1;
	}
	if (index == inspectionIcon)
	{
		bufferData[0] = 0;
		bufferData[1] = index - 1;
	}
	else if (index == outOfServiceIcon)
	{
		bufferData[0] = 0;
		bufferData[1] = index - 1;
	}
	else if (index == emergencyIcon)
	{
		bufferData[0] = 0;
		bufferData[1] = index - 2;
	}
	else if (index == vipRunIcon)
	{
		bufferData[0] = 0;
		bufferData[1] = index - 2;
	}
	else if (index == parkingIcon)
	{
		bufferData[0] = 0;
		bufferData[1] = index - 2;
	}
	else if (index == attendantIcon)
	{
		bufferData[0] = 0;
		bufferData[1] = index - 2;
	}

	dwinWriteData(ICON_WARNING, bufferData, 2);
}

///////////////////////////////////
void Dwin_Change_Current_FloorName(char *flName)
{
	uint8_t Buf[20] = {0};

	uint16_t tmp;
	Buf[0] = 0;
	Buf[1] = floorDisplay[BUF_TEN];
	dwinWriteData(FLOORNAME_VP_TEN, Buf, 2);
	Buf[2] = 0;
	Buf[3] = floorDisplay[BUF_UNIT];
	dwinWriteData(FLOORNAME_VP_UNIT, Buf + 2, 2);

	if ((Buf[1] == 0x20) || (Buf[1] == 0x00))
	{
		tmp = DEAFAULT_FLOOR_NAME_X_UINT - 85;
		changeLocationTEXT(FLOORNAME_SP_UNIT + DWIN_POSITION, tmp, DEAFAULT_FLOOR_NAME_Y_UNIT);
	}
	else
	{
		changeLocationTEXT(FLOORNAME_SP_TEN + DWIN_POSITION, DEAFAULT_FLOOR_NAME_X_TEN, DEAFAULT_FLOOR_NAME_Y_TEN);
		changeLocationTEXT(FLOORNAME_SP_UNIT + DWIN_POSITION, DEAFAULT_FLOOR_NAME_X_UINT, DEAFAULT_FLOOR_NAME_Y_UNIT);
	}
}
void initImage(void)
{
	uint8_t Buf[20] = {0};
	Buf[0] = 0x5A;
	Buf[1] = 0x01;
	Buf[2] = 0x00;
	Buf[3] = 109;
	dwinWriteData(0X0084, Buf, 4);
}

void Dwin_switch_to_next_page(void)
{
	static int PageCnt = 0;
	uint8_t Buf[20] = {0};
	Buf[0] = 0x5A;
	Buf[1] = 0x01;
	Buf[2] = 0x00;
	Buf[3] = PageCnt;
	dwinWriteData(0X0084, Buf, 4);
	PageCnt++;
	if (PageCnt == MAX_PAGE)
	{
		PageCnt = 0;
	}
}

void Dwin_update_time(void)
{
	uint8_t Buf[20] = {0};

	Buf[0] = 0;
	Buf[1] = aBCAN_ReceiveBuf_Clock[2];
	if (Buf[1] >= 12)
	{
		Buf[1] -= 12;
	}
	if (aBCAN_ReceiveBuf_Clock[1] <= 10)
	{
		Buf[1] *=  5;
	}
	else
		Buf[1] = Buf[1] * 5 + (unsigned char)((aBCAN_ReceiveBuf_Clock[1] / 10) - 1);

	dwinWriteData(TIME_VP, Buf, 2); // set hour

	Buf[0] = 0;
	Buf[1] = aBCAN_ReceiveBuf_Clock[1];

	dwinWriteData(TIME_VP + 1, Buf, 2); // set minute

	// Buf[10] = 0;
	Buf[0] = 0;
	Buf[1] = aBCAN_ReceiveBuf_Clock[3];

	dwinWriteData(0x1280, Buf, 2); // set date

	Buf[0] = 0;
	Buf[1] = (aBCAN_ReceiveBuf_Clock[4] >> 3);
	dwinWriteData(0x1320, Buf, 2); // set months
}

void DWIN_Arrow_Process(void)
{
	static uint8_t Arrow_state_old = 1;

	uint8_t Buf[100] = {0};
	if (Arrow_state_old == floorDisplay[BUF_ARROW])
		return;
	Arrow_state_old = floorDisplay[BUF_ARROW];
	Buf[0] = 00;
	Buf[2] = 00;
	Buf[4] = 00;
	switch (Arrow_state_old)
	{
	case 0X00: // hide

		Buf[1] = ARROW_STOP;
		Buf[3] = ARROW_STOP;
		Buf[5] = ARROW_STOP;
		break;
	case 0x80:
		Buf[1] = ARROW_UP_START;
		Buf[3] = ARROW_UP_START;
		Buf[5] = ARROW_UP_START;
		break;
	case 0x40:
		Buf[1] = ARROW_DOWN_START;
		Buf[3] = ARROW_DOWN_START;
		Buf[5] = ARROW_DOWN_START;
		break;
	case 0xA0:
		Buf[1] = ARROW_UP_STOP;
		Buf[3] = ARROW_UP_START;
		Buf[5] = ARROW_UP_END;

		break;
	case 0x60:
		Buf[1] = ARROW_DOWN_STOP;
		Buf[3] = ARROW_DOWN_START;
		Buf[5] = ARROW_DOWN_END;
		break;
	default:
		break;
	}
	Dwin_Write_VP(ARROW_ICON_SP + 6, (uint8_t *)&Buf, 3);
}

static uint16_t display_message_old = 1;
void DWIN_Message_Process(void)
{
	static uint32_t Message_Time = 0;

	uint8_t Buf[100] = {0};
	if (display_message_old == display_message)
		return;


	if (display_message_old != display_message)
	{
		display_message_old = display_message;
		if (display_message_old)
		{
			changeLocationTEXT(ICON_WARNING_SP + DWIN_POSITION, DEAFAULT_ICON_X + 155, DEAFAULT_ICON_Y); // center
			if (display_message_old & ERROR_REMOTEOFF)
			{
				showMessageWarning(parkingIcon);
			}
			if (display_message_old & ERROR_ATTENDANCE)
			{
				showMessageWarning(attendantIcon);
			}
			if (display_message_old & ERROR_VIPRUN)
			{
				showMessageWarning(vipRunIcon);
			}
			if (display_message_old & ERROR_FULLLOAD)
			{
				showMessageWarning(fullloadIcon);
			}

			if (display_message_old & ERROR_OVERLOAD)
			{
				showMessageWarning(overloadIcon);
			}

			if (display_message_old & ERROR_OUTOFORDER)
			{
				showMessageWarning(outOfServiceIcon);
			}

			if (display_message_old & ERROR_INSPECTION)
			{
				showMessageWarning(inspectionIcon);
			}

			if (display_message_old & ERROR_EMERGENCY)
			{
				showMessageWarning(emergencyIcon);
			}

			if (display_message_old & ERROR_FIRECASE)
			{
				showMessageWarning(fireIcon);
			}
		}
		else
		{
			changeLocationTEXT(ICON_WARNING_SP + DWIN_POSITION, DEAFAULT_ICON_X, DEAFAULT_ICON_Y); // logo
			showMessageWarning(normalLogo);
		}
	}
}
#if 0
	if (User_Message.Timeshow)
	{
		if (User_Message.IsChange)
		{
			User_Message.IsChange = 0;
			Message_Time = HAL_GetTick() + User_Message.Timeshow;
			sprintf((char *)Textmessage, User_Message.Data); // show message
			Keytimout = 0;
		}

		if ((HAL_GetTick() > Message_Time) && (User_Message.Timeshow != (0 - 1)))
		{
			User_Message.Timeshow = 0;
			if (display_message_old)
			{
				sprintf(Textmessage, TextErrmessage);
			}
			else
			{
				sprintf(Textmessage, "PLEASE ENTER DESTINATION FLOOR");
			}
			Keytimout = 0;
		}
		// return;
	}

	if ((virt_key[0] != virt_key_old[0]) || (virt_key[1] != virt_key_old[1]))
	{
		virt_key_old[0] = virt_key[0];
		virt_key_old[1] = virt_key[1];
		if (User_Message.Timeshow)
		{
			Keytimout = HAL_GetTick() + 300;
		}
		else
		{
			Keytimout = HAL_GetTick() + 1500;
		}
		for (int cnt = 0; cnt < 30; cnt++)
		{
			Buf[cnt] = 0;
		}
		Dwin_Write_VP(0x1500, Buf, 15); // hide message
		Buf[0] = 00;
		Buf[1] = virt_key_old[0];
		Buf[2] = 00;
		Buf[3] = virt_key_old[1];
		Dwin_Write_VP(0x1100, Buf, 2);
	}
	if (HAL_GetTick() > Keytimout)
	{
		sprintf((char *)Buf, Textmessage); // show message
		Dwin_Write_VP(0x1500, Buf, 15);	   // write buf to VP
		virt_key_cnt = 0;
		Buf[0] = 00;
		Buf[1] = 0x20;
		Buf[2] = 00;
		Buf[3] = 0x20;
		virt_key_old[0] = virt_key[0] = 0x20;
		virt_key_old[1] = virt_key[1] = 0x20;
		Dwin_Write_VP(0x1100, Buf, 2); // clear floor call
		Keytimout = 0 - 1;
	}
#endif
