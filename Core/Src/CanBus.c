
#define _CAN_C_

#include "CanBus.h"

/************************************************************************************************/
/* High priority interrupt routine																*/
/************************************************************************************************/
extern CAN_HandleTypeDef hcan;
extern uint8_t Callstatus[8];
extern uint8_t Callstatus_old[8];
extern uint8_t Arrow_state;

void Init_Can(void)
{

	uint8_t i, j;

	rc = 0; // clear all CAN variables
	ri = 0;
	ro = 0;
	tc = 0;
	ti = 0;
	to = 0;
	for (i = 0; i < RX_SIZE; i++) // clear rx buffer
	{
		for (j = 0; j < 10; j++)
			rx[i][j] = 0;
	}
	for (i = 0; i < TX_SIZE; i++) // clear tx buffer
	{
		for (j = 0; j < 10; j++)
			tx[i][j] = 0;
	}

	CAN_FilterTypeDef sFilterConfig;
	/*------------filter bank 1----------*/
	sFilterConfig.FilterBank = 1;
	sFilterConfig.FilterIdHigh = PDO_OUT << 8;
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh = 0xF000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;
	sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO1;
	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO1;
	sFilterConfig.FilterBank = 2;
	sFilterConfig.FilterIdHigh = ((RSDO | (node_id >> 3)) << 8) + (node_id << 5);
	// sFilterConfig.FilterIdHigh = RSDO << 8;
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh = 0xF000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;
	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
		/* Filter configuration Error */
		Error_Handler();
	}
	sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO1;
	sFilterConfig.FilterBank = 3;
	sFilterConfig.FilterIdHigh = NMT << 8;
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh = 0xF000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;
	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
		/* Filter configuration Error */
		Error_Handler();
	}

	sFilterConfig.FilterBank = 4;
	sFilterConfig.FilterIdHigh = (MPDO << 8) + (EMS_ID << 5);
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh = 0xF000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;
	sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO1;

	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO1;
	sFilterConfig.FilterBank = 5;
	sFilterConfig.FilterIdHigh = LSS << 8;
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh = 0xF000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
		/* Filter configuration Error */
		Error_Handler();
	}

	/* -----------------Filter bank 0---------------*/
	sFilterConfig.FilterBank = 10;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = ((HEARTBEAT << 3) + HSE_ID) << 5;
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh = ((HEARTBEAT << 3) + HSE_ID) << 5;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO0;
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);

	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY);
	HAL_CAN_Start(&hcan);

#if 0
	uint8_t i, j;

	rc = 0;       							  //clear all CAN variables
	ri = 0;
	ro = 0;
	tc = 0;
	ti = 0;
	to = 0;
	for (i = 0; i < RX_SIZE; i++)	//clear rx buffer
		{
			for (j = 0; j < 10; j++)
				rx[i][j] = 0;
		}
	for (i = 0; i < TX_SIZE; i++)	//clear tx buffer
		{
			for (j = 0; j < 10; j++)
				tx[i][j] = 0;
		}
	CAN_FilterTypeDef  sFilterConfig;
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh =((HEARTBEAT<<3)+HSE_ID)<<5;
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh =((HEARTBEAT<<3)+HSE_ID)<<5;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;

	if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
	/* Filter configuration Error */
		Error_Handler();
	}
	sFilterConfig.FilterBank = 1;
	sFilterConfig.FilterIdHigh =PDO_OUT<<8;
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh =0xF000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1;

	if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
	/* Filter configuration Error */
		Error_Handler();
	}
	sFilterConfig.FilterBank = 2;
	sFilterConfig.FilterIdHigh = ((RSDO | (node_id >> 3)) << 8) + (node_id <<5);
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh =0xF000;
	sFilterConfig.FilterMaskIdLow = 0x0000;

	if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
	/* Filter configuration Error */
		Error_Handler();
	}
	sFilterConfig.FilterBank = 3;
	sFilterConfig.FilterIdHigh = NMT <<8;
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh =0xF000;
	sFilterConfig.FilterMaskIdLow = 0x0000;

	if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
	/* Filter configuration Error */
		Error_Handler();
	}
	sFilterConfig.FilterBank = 4;
	sFilterConfig.FilterIdHigh = (MPDO << 8) +(EMS_ID << 5);
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh =0xF000;
	sFilterConfig.FilterMaskIdLow = 0x0000;

	if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
	/* Filter configuration Error */
		Error_Handler();
	}

	sFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO1;
	sFilterConfig.FilterBank = 5;
	sFilterConfig.FilterIdHigh = LSS << 8;
	sFilterConfig.FilterIdLow = 0x00;
	sFilterConfig.FilterMaskIdHigh = 0xF000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
		/* Filter configuration Error */
		Error_Handler();
	}


	RXB0CON 	= 0x20;		//ֻ���մ��б�׼��ʶ������Ч����
	RXB1CON 	= 0x20;		//ֻ���մ��б�׼��ʶ������Ч����

	//���չ�����0��1����� HSE ����
	RXF0SIDL	= HSE_ID << 5;			//Set acceptance filter 0 for buffer 0
	RXF0SIDH	= HEARTBEAT;			//buffer 0 only for Heartbeat HSE
	RXF1SIDL	= HSE_ID << 5;			//Set acceptance filter 1 for buffer 1
	RXF1SIDH	= HEARTBEAT;			//buffer 1 only for Heartbeat HSE
	RXM0SIDL		= 0x00;
	RXM0SIDH		= 0xFE;					//Set acceptance mask for buffer 0

	//���չ�����2����� LSS ���� PDO_OUT
	RXF2SIDL			= 0;					// Set acceptance filter 2 for buffer 1
	RXF2SIDH		= PDO_OUT;		// buffer 1 for virtual output messages

	//���չ�����3����� RSDO
	RXF3SIDL			= node_id << 5;	// Set acceptance filter 3 for buffer 1
	RXF3SIDH			= RSDO | (node_id >> 3);		// buffer 1 for receive SDO's

	//���չ�����4����� NMT
	RXF4SIDL			= 0x00;							// Set acceptance filter 4 for buffer 1
	RXF4SIDH			= NMT;							// buffer 1 for network management

	//���չ�����5����� TIME
	RXF5SIDL		= EMS_ID << 5;				// Set acceptance filter 5 for buffer 1
	RXF5SIDH		= MPDO;		//buffer 1 for time stamp message
	RXM1SIDL		= 0x00;		//Set acceptance mask for buffer 1
	RXM1SIDH		= 0xF0;		//use only function code for filtering

	CANCON  			= CAN_MODE_NORMAL1;				// set CAN normal mode
	while ((CANSTAT & CAN_MODE_BITS) != CAN_MODE_NORMAL1);	//wait till normal mode is set
 	TXB1SIDH 			= HEARTBEAT + (node_id >> 3);	// write ID bit 10 ... 3 for HEARTBEAT
	TXB1SIDL 			= node_id << 5;		// write ID bit  2 ... 0 for HEARTBEAT
	TXB1DLC	 			= 1;							// write data lenght code
	TXB1D0   			= nmtstate;				// write data uint8_t for HEARTBEAT
	PIR3 				= 0;								// clear all interrupt flags
	PIE3 				= 0x27;							// enable Error,TXB0 and RXB0 interrupt
	IPR3 				= 0xFF;							// all CAN interrupts high priority

	INTCONbits.GIEH		= 1;					// low priority interrupts enable
#else
#warning add filter and enbale interrupt

	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
	{
	}
	HAL_CAN_Start(&hcan);
#endif
}

uint8_t Can1RxData[8] = {0};
CAN_RxHeaderTypeDef Can1RxHeader;
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &Can1RxHeader, Can1RxData);

	if ((rc == RX_SIZE) && (!can_passive_time)) /* software buffer data overrun 		*/
	{
		errorregister |= ER_COMMUNICATION; /* set error bits 					*/
		can_passive_time = ERROR_TIME;
	}
	else
	{
		rx[ri][0] = (Can1RxHeader.StdId >> 3) & 0xF0; // read function code
		rx[ri][1] = Can1RxHeader.StdId & 0x7F;		  //(RXB1SIDL >> 5) + ((RXB1SIDH & 0x0F) << 3);	// node ID
		if ((rx[ri][0] != RSDO) || (rx[ri][1] == node_id))
		{
			memcpy((void *)&rx[ri][2], Can1RxData, Can1RxHeader.DLC);
			if (ri == (RX_SIZE - 1)) // increment RX message write pointer
				ri = 0;
			else
				ri++;
			rc++; // increment message counter
		}
	}
}

uint8_t CanRxData[8] = {0};
CAN_RxHeaderTypeDef CanRxHeader;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CanRxHeader, CanRxData);

	if (((CanRxHeader.StdId >> 3) & 0xF0) == HEARTBEAT)
	{
		if (((CanRxHeader.StdId & 0x7F) - 1) < MAX_LIFT) // Heartbeat from HSE
			hsetime = HSETIME;							 // reset HSE supervisor time
		else
		{
			hsetime = HSETIME - 10;
		}
	}
	hse_heartbeat = 1;
	// rx[ri][0] = (Can1RxHeader.StdId >> 3) & 0xF0; // read function code
	// rx[ri][1] = Can1RxHeader.StdId & 0x7F;		  //(RXB1SIDL >> 5) + ((RXB1SIDH & 0x0F) << 3);	// node ID
	// if (rx[ri][0] == HEARTBEAT)
	// {
	// 	hse_no = (CanRxHeader.StdId & 0x3F) - 1; /* read node id (HSE number)			*/
	// 	if (hse_no < MAX_LIFT)					 /* Heartbeat from HSE 				*/
	// 		hsetime = HSETIME;					 /* reset HSE supervisor time			*/
	// 	hse_heartbeat = 1;
	// }
	// else if ((rx[ri][0] == MPDO) && (rx[ri][1] == EMS_ID))
	// {
	// 	memcpy((void *)&rx[ri][2], CanRxData, CanRxHeader.DLC);
	// 	if (ri == (RX_SIZE - 1)) /* increment RX message write pointer */
	// 		ri = 0;
	// 	else
	// 		ri++;
	// 	rc++; /* increment message counter			*/
	// }
}

uint32_t pTxMailbox;

uint16_t mallbox[3] = {0};
uint16_t txmallbox = 0;
uint16_t txmallbox_request = 0;
void CAN_transmit_Interrupt(void)
{
	if (tc) // more messages to send
	{
		CAN_TxHeaderTypeDef CanTxHeader;
		uint8_t CanTxData[8] = {0};
		CanTxHeader.StdId = ((tx[to][0] & 0xF0) << 3) + tx[to][1];
		CanTxHeader.DLC = tx[to][0] & 0x0F; // read data lenght code
		CanTxHeader.IDE = CAN_ID_STD;
		CanTxHeader.RTR = CAN_RTR_DATA;
		memcpy(CanTxData, (void *)(tx[to] + 2), 8);
		if (HAL_CAN_AddTxMessage(&hcan, &CanTxHeader, CanTxData, &pTxMailbox) == HAL_OK)
		{
			txmallbox++;
		}
		else
		{
			Error_Handler();
		}
		tc--;					 // decrement TX message counter
		if (to == (TX_SIZE - 1)) // increment TX message read pointer
			to = 0;
		else
			to++;
	}
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	mallbox[0]++;
	CAN_transmit_Interrupt();
}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	mallbox[1]++;
	CAN_transmit_Interrupt();
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	mallbox[2]++;
	CAN_transmit_Interrupt();
}

// abort SDO transfer
void abort_sdo(uint32_t errorcode)
{
	while (tc == TX_SIZE)
		; /* wait for TX buffer free				*/

	tx[ti][0] = TSDO + 8;  /* write function code + data length	*/
	tx[ti][1] = node_id;   /* write node id of UEA					*/
	tx[ti][2] = ABORT_REQ; /* write command specifier				*/
	tx[ti][3] = sdo_index; /* Index of last RX SDO					*/
	tx[ti][4] = sdo_index >> 8;
	tx[ti][5] = sdo_subindex; /* Sub-index last RX SDO				*/
	tx[ti][6] = errorcode;	  /* errorcode (reason for abort request	*/
	tx[ti][7] = errorcode >> 8;
	tx[ti][8] = errorcode >> 16;
	tx[ti][9] = errorcode >> 24;
	can_transmit(); /* transmit message						*/
}

// SDO response
void sdo_response(uint8_t command, uint16_t index, uint8_t subindex, uint32_t value)
{
	while (tc == TX_SIZE)
		; /* wait for TX buffer free				*/

	tx[ti][0] = TSDO + 8; /* write function code + data length	*/
	tx[ti][1] = node_id;  /* write node id of UEA					*/
	tx[ti][2] = command;  /* write command specifier				*/
	tx[ti][3] = index;	  /* write index 							*/
	tx[ti][4] = index >> 8;
	tx[ti][5] = subindex; /* write sub-index						*/
	tx[ti][6] = value;	  /* write value							*/
	tx[ti][7] = value >> 8;
	tx[ti][8] = value >> 16;
	tx[ti][9] = value >> 24;
	can_transmit(); /* transmit message						*/
}

// SDO segment
void sdo_segment(uint8_t command, uint8_t size, uint8_t *value)
{
	uint8_t i;
	while (tc == TX_SIZE)
		;				  /* wait for TX buffer free				*/
	tx[ti][0] = TSDO + 8; /* write function code + data length	*/
	tx[ti][1] = node_id;  /* write node id of UEA					*/
	tx[ti][2] = command;  /* write command specifier				*/
	for (i = 0; i < size; i++)
		tx[ti][3 + i] = *value++; /* write value 							*/
	for (i = size; i < 7; i++)
		tx[ti][3 + i] = 0; /* set unused data bytes to 0			*/
	can_transmit();		   /* transmit message						*/
}

// LSS response
void lss_response(uint8_t command, uint8_t value)
{
	uint8_t i;
	while (tc == TX_SIZE)
		;					/* wait for TX buffer free				*/
	tx[ti][0] = LSS + 8;	/* write function code + data length	*/
	tx[ti][1] = LSS_RES_ID; /* write node id part of identifier		*/
	tx[ti][2] = command;	/* write command specifier				*/
	tx[ti][3] = value;		/* write index 							*/
	for (i = 4; i <= 9; i++)
		tx[ti][i] = 0; /* set unused data bytes to 0			*/
	can_transmit();	   /* transmit message						*/
}

// transmit emergency message
void transmit_error(void)
{
	uint8_t i;
	while (tc == TX_SIZE)
		;						/* wait for TX buffer free				*/
	tx[ti][0] = EMERGENCY + 8;	/* write function code + data length	*/
	tx[ti][1] = node_id;		/* write node id of UEA					*/
	tx[ti][2] = errorcode;		/* write error code						*/
	tx[ti][3] = errorcode >> 8; /* write error code						*/
	tx[ti][4] = errorregister;	/* write error register					*/
	for (i = 5; i <= 9; i++)
		tx[ti][i] = 0; /* manufacture specific part not used	*/
	can_transmit();	   /* transmit message						*/
}

// ransmit special inputs and calls
void transmit_in(uint8_t *input)
{
	uint8_t i;
	while (tc == TX_SIZE)
		;							  /* wait for TX buffer free				*/
	tx[ti][0] = PDO_IN + MAX_IO_TYPE; /* write function code + data length	*/
	tx[ti][1] = node_id;			  /* write node id of UEA					*/
	for (i = 0; i < MAX_IO_TYPE; i++)
		tx[ti][2 + i] = *input++; /* write input function					*/
	can_transmit();				  /* transmit message						*/
}
/************************************************************************************************/
/* transmit load measurement state																*/
/************************************************************************************************/
void transmit_load(void)
{
	uint8_t i;
	uint8_t state;
	state = 0;
	for (i = 0; i < MAX_IO; i++)
	{
		if (inpar[i][IO_BASIC_FUNC] == LOAD_IN) /* load measurement input				*/
			if (inpar[i][IO_STATE])				/* input is on							*/
				state |= inpar[i][IO_SUB_FUNC]; /* set load measurement state			*/
	}
	while (tc == TX_SIZE)
		;				  /* wait for TX buffer free				*/
	tx[ti][0] = FC_3 + 1; /* write function code + data length	*/
	tx[ti][1] = LOAD_ID;  /* write second part of ID				*/
	tx[ti][2] = state;	  /* write load measurment state			*/
	can_transmit();		  /* transmit message						*/
}
uint8_t check_for_call(uint8_t value)
{
	switch (value)
	{
	case (HALL_CALL):			/* standard hall call					*/
	case (HALL_CALL_SPECIAL):	/* special hall call					*/
	case (HALL_CALL_ADVANCED):	/* advanced hall call					*/
	case (HALL_CALL_EMERGENCY): /* emergency hall call					*/
	case (CAR_CALL):
		return (1); /* car call								*/
	default:
		return (0); /* all other values						*/
	}
}
void sent_heartbeat(void)
{
	while (tc == TX_SIZE)
		; /* wait for TX buffer free				*/

	tx[ti][0] = HEARTBEAT + 1; /* write function code + data length	*/
	tx[ti][1] = node_id;	   /* write node id of UEA					*/
	tx[ti][2] = nmtstate;	   /* write command specifier				*/
	can_transmit();			   /* transmit message						*/
							   // 	TXB1SIDH 			= HEARTBEAT + (node_id >> 3);	// write ID bit 10 ... 3 for HEARTBEAT
							   //	TXB1SIDL 			= node_id << 5;		// write ID bit  2 ... 0 for HEARTBEAT
							   //	TXB1DLC	 			= 1;							// write data lenght code
							   //	TXB1D0   			= nmtstate;				// write data uint8_t for HEARTBEAT
}

void read_rx(void)
{
	uint8_t type;
	uint16_t index;
	uint8_t subindex;
	uint8_t i;
	uint8_t size;
	uint8_t sub;
	uint32_t value;
	uint8_t buffer[8];
	static uint8_t pos = 0;
	//	static uint8_t cte = 0;

	switch (rx[ro][0]) /* message function code				*/
	{
	case (PDO_OUT):			  /* receive PDO virtual output			*/
		index = rx[ro][2];	  /* read function code					*/
		subindex = rx[ro][3]; // FLOOR
		if (index)			  /* function code > 0					*/
		{
			if (nmtstate == OPERATIONAL) /* only in operational state			*/
			{
				for (i = 0; i < MAX_IO_TYPE; i++) /* read output function					*/
					virt_out[i] = rx[ro][i + 2];  /* write to virtual output mapping	*/
				set_output(virt_out);			  /* set physical outputs					*/
			}
		}
		break;

	case (RSDO):						  /* receive SDO message					*/
		type = rx[ro][2];				  /* read SDO type						*/
		switch (type & COMMAND_SPECIFIER) /* check command specifier of SDO		*/
		{
		case (INIT_WRITE_REQ):				 /* init write or expedited write		*/
			index = *(uint16_t *)&rx[ro][3]; /* read object index					*/
			subindex = rx[ro][5];			 /* read object subindex					*/
			value = search_dict(index, subindex, type, &pos);
			if (value)			  /* wrong access to object dictionary	*/
				abort_sdo(value); /* abort SDO transfer					*/
			else				  /* write possible						*/
			{
				if (type & EXPEDITED_BIT) /* expedited transfer					*/
				{
					value = write_dict(pos, subindex, *(uint32_t *)&rx[ro][6]);
					if (value)			  /* value out of range					*/
						abort_sdo(value); /* abort SDO transfer					*/
					else				  /* write ready							*/
					{
						sdo_response(INIT_WRITE_RESP, index, subindex, *(uint32_t *)&rx[ro][6]);
						if (index == ARROW_MODE)
						{
							if (arrowtype == ARROW_SCROLL)
							{
								if (hardware_version == VGS_502)
									arrowflash = 0x01;
								else
									arrowflash = 0x00;
							}
							else if (arrowtype == ARROW_FLASH)
								arrowflash = 0x01;
							else
								arrowflash = 0x00;
							arrowflash_old = arrowflash;
						}
					}
				}
				else /* normal transfer						*/
				{
					sdo_response(INIT_WRITE_RESP, index, subindex, 0);
					sdo_index = index; /* save object index and subindex		*/
					sdo_subindex = subindex;
					sdo_timer = SDO_TIMER; /* start SDO time out timer				*/
				}
			}
			break;

		case (WRITE_SEGM_REQ): /* write segment						*/
			if (!sdo_index)	   /* no init write request before			*/
				abort_sdo(SDO_UNSUPPORTED);
			else if (type & TOGGLE_BIT) /* toggle bit must be 0 for 1. segment	*/
				abort_sdo(SDO_TOGGLEBIT);
			else if (!(type & LAST_SEGM_BIT)) /* more segments to write				*/
				abort_sdo(SDO_L_TO_HIGH);
			else
			{
				switch (sdo_index)
				{
				case (VIRTUAL_OUTPUT): /* virtual output mapping object		*/
					for (i = 0; i < MAX_IO_TYPE; i++)
						virt_out[i] = rx[ro][3 + i];
					set_output(virt_out);
					break;

				case (INPUT_GROUP):			/* input group							*/
					for (i = 0; i < 5; i++) /* dont write input state (Byte 5)		*/
					{
						if (inpar[sdo_subindex - 1][i] != rx[ro][3 + i])
						{
							inpar[sdo_subindex - 1][i] = rx[ro][3 + i];
						}
					}
					break;

				case (OUTPUT_GROUP): /* output group							*/
					for (i = 0; i < 5; i++)
					{
						if (outpar[sdo_subindex - 1][i] != rx[ro][3 + i])
						{
							outpar[sdo_subindex - 1][i] = rx[ro][3 + i];
						}
					}
					set_io_config(); /* check if output is push output		*/
					break;
				}
				sdo_segment(WRITE_SEGM_RESP, 0, 0);
			}
			sdo_index = 0; /* reset marker from init request		*/
			sdo_subindex = 0;
			sdo_timer = 0;
			break;

		case (INIT_READ_REQ):				 /* init read or expedited read			*/
			index = *(uint16_t *)&rx[ro][3]; /* read object index					*/
			subindex = rx[ro][5];			 /* read object subindex					*/
			value = search_dict(index, subindex, type, &pos);
			if (value)			  /* wrong access to object dictionary	*/
				abort_sdo(value); /* abort SDO transfer					*/
			else				  /* read possible						*/
			{
				size = dict[pos].size;
				sub = dict[pos].sub;
				if (size > 4) /* normal transfer						*/
				{
					if ((!subindex) && sub) /* number of entries					*/
						sdo_response(INIT_READ_RESP | EXPEDITED_BIT, index, subindex, sub);
					else
					{
						sdo_response(INIT_READ_RESP, index, subindex, size);
						sdo_index = index; /* save object index and subindex		*/
						sdo_subindex = subindex;
						sdo_timer = SDO_TIMER;
					}
				}
				else /* expedited transfer					*/
				{
					if ((!subindex) && sub)
					{			  /* number of entries					*/
						size = 1; /* size is 1 byte						*/
						value = sub;
					}	 /* calculate number of entries			*/
					else /* read value of object					*/
						value = read_dict(pos, subindex);
					sdo_response(INIT_READ_RESP | EXPEDITED_BIT, index, subindex, value);
				}
			}
			break;

		case (READ_SEGM_REQ): /* read segment							*/
			if (!sdo_index)	  /* no init read request before			*/
				abort_sdo(SDO_UNSUPPORTED);
			else if (type & TOGGLE_BIT) /* toggle bit must be 0 for 1. segment	*/
				abort_sdo(SDO_TOGGLEBIT);
			else
			{
				switch (sdo_index)
				{
				case (DEVICE_NAME): /* device name							*/
					for (i = 0; i < DV_SIZE; i++)
					{
						if (hardware_version == VGS_502)
							buffer[i] = device_name_G741[i];
						else
							buffer[i] = device_name_G742[i];
					}
					sdo_segment(READ_SEGM_RESP | LAST_SEGM_BIT, 6, buffer);
					break;

				case (HW_VERSION): /* hardware version						*/
					for (i = 0; i < 6; i++)
					{
						if (hardware_version == VGS_502)
							buffer[i] = hardware_uea_G741[i];
						else
							buffer[i] = hardware_uea_G742[i];
					}
					sdo_segment(READ_SEGM_RESP | LAST_SEGM_BIT, 5, buffer);
					break;

				case (SW_VERSION): /* software version						*/
					for (i = 0; i < 6; i++)
					{
						if (hardware_version == VGS_502)
							buffer[i] = version_uea_G741[i];
						else
							buffer[i] = version_uea_G742[i];
					}
					sdo_segment(READ_SEGM_RESP | LAST_SEGM_BIT, 5, buffer);
					break;

				case (VIRTUAL_INPUT): /* virtual input mapping object			*/
					sdo_segment(READ_SEGM_RESP | LAST_SEGM_BIT, MAX_IO_TYPE, virt_in);
					break;

				case (VIRTUAL_OUTPUT): /* virtual output mapping object		*/
					sdo_segment(READ_SEGM_RESP | LAST_SEGM_BIT, MAX_IO_TYPE, virt_out);
					break;

				case (INPUT_GROUP): /* input group							*/
					sdo_segment(READ_SEGM_RESP | LAST_SEGM_BIT, MAX_IO_TYPE, inpar[sdo_subindex - 1]);
					break;

				case (OUTPUT_GROUP): /* output group							*/
					sdo_segment(READ_SEGM_RESP | LAST_SEGM_BIT, MAX_IO_TYPE, outpar[sdo_subindex - 1]);
					break;
				}
			}
			sdo_index = 0; /* reset marker from init request		*/
			sdo_subindex = 0;
			sdo_timer = 0;
			break;

		case (ABORT_REQ):  /* abort request						*/
			sdo_index = 0; /* reset marker from init request		*/
			sdo_subindex = 0;
			sdo_timer = 0;
			break;

		default:
			abort_sdo(SDO_NOT_VALID); /* unknown SDO command specifier		*/
		}
		break;

	case (NMT):						/* network management					*/
		i = rx[ro][3];				/* read node id							*/
		if ((!i) || (i == node_id)) /* for this node or for all nodes		*/
		{
			switch (rx[ro][2]) /* command specifier					*/
			{
			case (START_NODE):			/* start node							*/
				nmtstate = OPERATIONAL; /* enter operational state				*/
				break;

			case (RESET_NODE): /* reset node							*/
				merker = 0;
#if 0
				CANCON = CAN_MODE_CONFIG; /* set CAN configuration mode			*/
				Reset();				  /* force a software reset				*/
#else
				HAL_NVIC_SystemReset();
#endif
				break;

			case (TEST_MODE): /* start test mode						*/
				nmtstate = TEST_MODE;
				display_timer = 6;
				break;

			case VERSION_NODE:
				if (hardware_version == VGS_502)
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
				heartbeat = HEARTBEATTIME; //��ʾ�汾�� 3S
				break;

			case (STOP_NODE):	/* reset node							*/
			case (ENTER_PREOP): /* reset node							*/
			case (RESET_COMM):	/* reset node							*/
			default:
				break; /* do nothing (not implemented)			*/
			}
		}
		break;
	case (LSS): /* LSS message for initialization		*/
		if (LSS_REQ_ID == rx[ro][1])
		{
			type = rx[ro][2]; /* read LSS service type				*/
			switch (type)
			{
			case (SET_NODE_ID):
				setid_mode = 2;
				setid_mode_old = 0;
				node_id = rx[ro][3];
				id_buff[BUF_ARROW] = rx[ro][6];
				id_buff[BUF_TEN] = rx[ro][4];
				id_buff[BUF_UNIT] = rx[ro][5];
				if (hardware_version == VGS_502)
					id_buff[BUF_ARROW] = NO_ARROW;
				else if (id_buff[BUF_ARROW] <= 0x20)
					id_buff[BUF_ARROW] = NO_ARROW;
				if (id_buff[BUF_TEN] <= 0x20)
					id_buff[BUF_TEN] = NO_FLOOR + '0';
				if (id_buff[BUF_UNIT] <= 0x20)
					id_buff[BUF_UNIT] = NO_FLOOR + '0';
				break;

			case (DISP_NODE_ID):
				setid_mode = 3;
				disp_id = 0;
				id_buff[BUF_ARROW] = NO_ARROW;
				if (node_id >= ESE_ID && (node_id < ESE_ID + MAX_ESE))
				{
					id_buff[BUF_TEN] = ((node_id - ESE_ID + 1) / 10) + '0';
					id_buff[BUF_UNIT] = ((node_id - ESE_ID + 1) % 10) + '0';
					for (uint8_t i = 0; i < 36; i++)
					{
						if (id_buff[BUF_TEN] == tDisp_FloorAscii[i])
						{
							showNodeId[BUF_TEN] = i;
						}
						if (id_buff[BUF_UNIT] == tDisp_FloorAscii[i])
						{
							showNodeId[BUF_UNIT] = i;
						}
					}
				}
				else
				{
					id_buff[BUF_TEN] = A_BETR_TEN + '0';
					id_buff[BUF_UNIT] = A_BETR_UNIT + '0';
				}
				break;

			case (SET_NODE_ID2):
				setid_mode = 4;
				setid_mode_old = 0;
				break;

			case (ABORT_NODE_ID):
				merker = ID_MERKER;
				if (setid_mode == 2)
				{
					nmtstate = PRE_OP;
					node_id = preset_node_id;
				}
				setid_mode = 0;
				setid_mode_old = 0;
				break;
			}
		}
		break;
		//����Ϣ��ʱֻ������ͣ�����״̬��Ϣ		2017-11-15
	case (MPDO): // time stamp message
		sub = rx[ro][1];
		type = rx[ro][2];
		if ((sub == EMS_ID) && (type == 0))
		{
			for (i = 0; i < 8; i++)
				recei_monitor[i] = rx[ro][2 + i];
			if (recei_monitor[7] & 0x80)
				display[BUF_MESSAGE] |= FULL;
			else
				display[BUF_MESSAGE] &= ~FULL;
			if (recei_monitor[7] & 0x01)
				display[BUF_MESSAGE] |= PARK;
			else
				display[BUF_MESSAGE] &= ~PARK;
		}
		break;
		//����Ϣ��ʱֻ������ͣ�����״̬��Ϣ		2017-11-15
	}
	if (HAL_CAN_DeactivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
	{
	}
	// INTCONbits.GIEH		= 0;		// low priority interrupts disable
	if (ro == (RX_SIZE - 1)) // increment RX message read pointer
		ro = 0;
	else
		ro++;
	rc--; // decrement RX counter
	i = ri;
	size = rc;
	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
	{
	}
	// INTCONbits.GIEH		= 1;		// low priority interrupts enable
	if (i < ro)
		i += RX_SIZE;
	if ((i - ro) != (size % RX_SIZE))
	{
		merker = RC_MERKER; // set rc counter error marker
	}
}

uint32_t timeeeee;
uint8_t CanTxData_t[8] = {0};
void can_transmit(void)
{
	txmallbox_request++;
	if ((!hse_heartbeat) || (merker == BS_MERKER)) // û��MCU����������busoff�������κ���Ϣ
		return;

	if (ti == (TX_SIZE - 1)) /* increment TX message write pointer	*/
		ti = 0;
	else
		ti++;
	if (HAL_CAN_DeactivateNotification(&hcan, CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
	{
	}
	CAN_TxHeaderTypeDef CanTxHeader_t;

	uint32_t Txmallbox;
	// INTCONbits.GIEH	= 0;								/* global interrupts disable			*/
	CanTxHeader_t.StdId = ((tx[to][0] & 0xF0) << 3) + tx[to][1];
	CanTxHeader_t.DLC = tx[to][0] & 0x0F; // read data lenght code
	CanTxHeader_t.IDE = CAN_ID_STD;
	CanTxHeader_t.RTR = CAN_RTR_DATA;

	memcpy(CanTxData_t, (void *)(tx[to] + 2), 8);
	if (HAL_CAN_AddTxMessage(&hcan, &CanTxHeader_t, CanTxData_t, &Txmallbox) == HAL_OK)
	{
		txmallbox++;
		if (to == (TX_SIZE - 1)) /* increment TX message read pointer	*/
			to = 0;
		else
			to++;
	}
	else
	{
		tc++;
	}
	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
	{
	}
}

/*=============================  �����Ƿ��ͳ���   ================================*/
// transmit a message if TX register is free or write message to buffer

uint8_t fire_alarm = 0;
uint8_t fire_evacuation = 0;
uint8_t fire_state = 0;
// set special outputs SPECIAL_FUNC
void set_output(uint8_t *virt)
{
	uint8_t i;
	uint8_t iotype;
	uint8_t buf[3] = {0, 0, 0};
	uint8_t sub = 0;
	uint16_t index = 0;

	iotype = virt[IO_BASIC_FUNC];
	if (check_for_call(iotype)) /* car call, hall call or priority call	*/
	{
		if (iotype == HALL_CALL) /* read floor number					*/
			i = virt[IO_FLOOR];
		else
			i = virt[IO_SUB_FUNC];
		if (i == 0xFF) /* clear all calls						*/
		{
			for (i = 0; i < MAX_IO; i++) /* search output parameter list			*/
			{
				if ((outpar[i][IO_BASIC_FUNC] == iotype) && (outpar[i][IO_ENABLE] == ENABLE))
				{
					outpar[i][IO_ACK] &= ~virt[IO_LIFT];
					if (!outpar[i][IO_ACK]) /* all acknowledgements cancelled		*/
					{
						bit_reset(out, i); /* clear output							*/
						outpar[i][IO_STATE] = 0;
					}
				}
			}
		}
		else /* set or reset single call				*/
		{
			for (i = 0; i < MAX_IO; i++) /* search output parameter list			*/
			{
				if (virt[IO_BASIC_FUNC] == outpar[i][IO_BASIC_FUNC])
					if (virt[IO_SUB_FUNC] == outpar[i][IO_SUB_FUNC])
						if (virt[IO_FLOOR] == outpar[i][IO_FLOOR])
							if (virt[IO_LIFT] & outpar[i][IO_LIFT])
								if (!((~virt[IO_DOOR]) & (outpar[i][IO_DOOR] & 0x0F)))
									if (outpar[i][IO_ENABLE] == ENABLE)
									{							   /* virtual output matches with physical	*/
										if (virt[IO_STATE] & 0x01) /* set acknowledgement					*/
										{
											bit_set(out, i); /* set physical output					*/
											outpar[i][IO_ACK] |= virt[IO_LIFT];
											outpar[i][IO_STATE] = 1;
										}
										else /* reset acknowledgement				*/
										{
											outpar[i][IO_ACK] = 0;
											bit_reset(out, i); /* clear physical output				*/
											outpar[i][IO_STATE] = 0;
										}
									}
			}
		}
	}
	else if (iotype == POSITION_INDICATOR)
	{
		if ((virt[IO_LIFT] == disp_lift) && (!setid_mode)) /* display message is for this lift 	*/
		{
			// PIE1bits.TMR1IE = 0;			/* disable Timer 2 interrupt			*/
			buf[BUF_TEN] = virt[IO_DOOR];	/* 1. digit; not CANopen compatible		*/
			buf[BUF_UNIT] = virt[IO_STATE]; /* 2. digit; not CANopen compatible		*/
			for (i = 0; i < STANDER_FLOOR_NUM + THREE_FLOOR_NUM + 10; i++)
			{
				if (buf[BUF_TEN] == tDisp_FloorAscii[i])
				{
					display[BUF_TEN] = i;
					break;
				}
				else if ((buf[BUF_TEN] == 0) || (buf[BUF_TEN] == 0x20))
				{
					display[BUF_TEN] = NO_FLOOR;
					break;
				}
				else if (tDisp_FloorAscii[i] == 0)
				{
					break;
				}
			}
			for (i = 0; i < STANDER_FLOOR_NUM + THREE_FLOOR_NUM + 10; i++)
			{
				if (buf[BUF_UNIT] == tDisp_FloorAscii[i])
				{
					display[BUF_UNIT] = i;
					break;
				}
				else if ((tDisp_FloorAscii[i] == 0) || (buf[BUF_UNIT] == 0) || (buf[BUF_UNIT] == 0x20))
				{
					display[BUF_UNIT] = NO_FLOOR;
					break;
				}
			}
			if (display[BUF_UNIT] == NO_FLOOR) // lift out of work
			{
				display[BUF_TEN] = A_BETR_TEN;
				display[BUF_UNIT] = A_BETR_UNIT;
			}
			// PIE1bits.TMR1IE = 1; /* enable Timer 2 interrupt				*/
		}
	}
	else if ((iotype == DIRECTION_IND) && (virt[IO_LIFT] == disp_lift) && (!setid_mode))
	{
		// PIE1bits.TMR1IE = 0;
		if (!virt[IO_STATE] & 0x01)
			display[BUF_ARROW] = 0;
		else
			display[BUF_ARROW] = (virt[IO_SUB_FUNC] & 0x03);
		if (display[BUF_ARROW] > 2)
			display[BUF_ARROW] = 0;
		if (display[BUF_ARROW] == 0)
			scroll = 0;
		else
			scroll = (virt[IO_SUB_FUNC] >> 4) & 0x03;
		display[BUF_ARROW] += NO_ARROW; /* set to ASCII 62 - 64 				*/
		flashcontent = display[BUF_ARROW];
		// PIE1bits.TMR1IE = 1;
	}
	else if ((iotype == HALL_LANTERN) || (iotype == LIGHT_FUNC))
	{
		sub = virt[IO_SUB_FUNC];
		if (iotype == HALL_LANTERN)
		{
			for (i = 0; i < MAX_IO; i++) /* search output parameter list			*/
			{
				if ((virt[IO_BASIC_FUNC] == outpar[i][IO_BASIC_FUNC]) &&
					(virt[IO_LIFT] == outpar[i][IO_LIFT]) &&
					(outpar[i][IO_ENABLE] == ENABLE))
				{
					if (((virt[IO_SUB_FUNC] & outpar[i][IO_SUB_FUNC] & 0x03) || (!outpar[i][IO_SUB_FUNC])) &&
						((virt[IO_FLOOR] == outpar[i][IO_FLOOR]) || (virt[IO_FLOOR] == 0xFF) || (!outpar[i][IO_FLOOR])) &&
						((virt[IO_DOOR] & outpar[i][IO_DOOR]) || (!outpar[i][IO_DOOR])))
					{
						if (!direction_ind_mode)
						{
							if (virt[IO_STATE] & 0x01)
							{
								bit_set(out, i); /* set physical output					*/
								outpar[i][IO_STATE] = 1;
							}
							else
							{
								bit_reset(out, i); /* reset physical output				*/
								outpar[i][IO_STATE] = 0;
							}
						}
						else
						{
							if (virt[IO_SUB_FUNC] >> 4) // ���������У�����Ʋ�����
							{
								bit_reset(out, i);
								outpar[i][IO_STATE] = 0;
							}
							else
							{
								if (virt[IO_STATE] & 0x01)
								{
									bit_set(out, i); /* set physical output					*/
									outpar[i][IO_STATE] = 1;
								}
								else
								{
									bit_reset(out, i); /* reset physical output				*/
									outpar[i][IO_STATE] = 0;
								}
							}
						}
					}
					else /* reset all not matching indications	*/
					{
						bit_reset(out, i); /* reset physical output				*/
						outpar[i][IO_STATE] = 0;
					}
				}
			}
		}
		else if ((iotype == LIGHT_FUNC) &&
				 (sub & (HALL_LANTERN_UP | HALL_LANTERN_DN | DIRECTION_IND_UP | DIRECTION_IND_DN)))
		{
			for (i = 0; i < MAX_IO; i++) /* search output parameter list			*/
			{
				if ((virt[IO_BASIC_FUNC] == outpar[i][IO_BASIC_FUNC]) &&
					(virt[IO_LIFT] == outpar[i][IO_LIFT]) &&
					(outpar[i][IO_ENABLE] == ENABLE))
				{
					if ((virt[IO_SUB_FUNC] & outpar[i][IO_SUB_FUNC] & 0x0F) &&
						((virt[IO_FLOOR] == outpar[i][IO_FLOOR]) || (virt[IO_FLOOR] == 0xFF)) &&
						(virt[IO_DOOR] & outpar[i][IO_DOOR]))
					{
						if (!direction_ind_mode)
						{
							if (virt[IO_STATE] & 0x01)
							{
								bit_set(out, i); /* set physical output					*/
								outpar[i][IO_STATE] = 1;
							}
							else
							{
								bit_reset(out, i); /* reset physical output				*/
								outpar[i][IO_STATE] = 0;
							}
						}
						else
						{
							if (virt[IO_SUB_FUNC] >> 4) // ���������У�����Ʋ�����
							{
								bit_reset(out, i);
								outpar[i][IO_STATE] = 0;
							}
							else
							{
								if (virt[IO_STATE] & 0x01)
								{
									bit_set(out, i); /* set physical output					*/
									outpar[i][IO_STATE] = 1;
								}
								else
								{
									bit_reset(out, i); /* reset physical output				*/
									outpar[i][IO_STATE] = 0;
								}
							}
						}
					}
					else /* reset all not matching indications	*/
					{
						bit_reset(out, i); /* reset physical output				*/
						outpar[i][IO_STATE] = 0;
					}
				}
			}
		}
	}
	else if (iotype == ARRIVAL_INDICATION)
	{
		for (i = 0; i < MAX_IO; i++) /* search output parameter list			*/
		{
			if (virt[IO_BASIC_FUNC] == outpar[i][IO_BASIC_FUNC])
				if ((virt[IO_SUB_FUNC] & outpar[i][IO_SUB_FUNC] & 0x03) || (!outpar[i][IO_SUB_FUNC]))
					if ((virt[IO_FLOOR] == outpar[i][IO_FLOOR]) || (virt[IO_FLOOR] == 0xFF) || (!outpar[i][IO_FLOOR]))
						if (virt[IO_LIFT] == outpar[i][IO_LIFT])
							if ((virt[IO_DOOR] & outpar[i][IO_DOOR]) || (!outpar[i][IO_DOOR]))
								if (outpar[i][IO_ENABLE] == ENABLE)
								{
									if (virt[IO_STATE] & 0x01)
									{
										bit_set(out, i);		/* set physical output					*/
										outpar[i][IO_ACK] = 20; /* set on timer 20 * 500 ms = 10 s		*/
										outpar[i][IO_STATE] = 1;
									}
								}
		}
	}
	else if ((iotype == SPECIAL_FUNC) || (iotype == FIRE_FUNCTION) || (iotype == CALL_TYPE))
	{ //���⹦��
		sub = virt[IO_SUB_FUNC];
		switch (sub)
		{
		case FULL_LOAD_STATE:
			if (virt[IO_STATE])
			{
				display[BUF_MESSAGE] |= FULL;
				OPEN_FULL();
			}
			else
			{
				display[BUF_MESSAGE] &= ~FULL;
				CLOSE_FULL();
			}
			break;

		case REMOTE_OFF:
		case REMOTE_OFF_STATE:
			if (virt[IO_STATE])
				display[BUF_MESSAGE] |= PARK;
			else
				display[BUF_MESSAGE] &= ~PARK;
			break;

		case FIRE_ALARM:
		case FIRE_EVACUATION:
		case FIRE_STATE:
			switch (sub)
			{
			case FIRE_ALARM:
				fire_alarm = virt[IO_STATE];
				break;
			case FIRE_EVACUATION:
				fire_evacuation = virt[IO_STATE];
				break;
			case FIRE_STATE:
				fire_state = virt[IO_STATE];
				break;
			}
			break;

		case OUT_OF_ORDER:
			if (virt[IO_STATE])
				OPEN_OUT_OF_ORDER();
			else
				CLOSE_OUT_OF_ORDER();
			break;

		case DOOR_STOP:
			if (virt[IO_STATE])
				OPEN_INUSE();
			else
				CLOSE_INUSE();
			break;
		}
	}
	else if (iotype == OPERATION_DATA)
	{
		index = ((uint16_t)virt[IO_SUB_FUNC] << 8) | virt[IO_FLOOR];
		if (index == BACKLIGHT_MODE)
		{
			backlight_func = (virt[IO_STATE] >> 4) & 0x03;
			backlight_off_time = ((uint16_t)(virt[IO_STATE] & 0x0F)) * NO_SIGNAL_TIMEOUT;
			light_para_ok = 1;
			display_STN_mode = (virt[IO_STATE] >> 6) & 0x03;
		}
	}
	else
	{
		for (i = 0; i < MAX_IO; i++) /* search output parameter list			*/
		{
			if (virt[IO_BASIC_FUNC] == outpar[i][IO_BASIC_FUNC])
				if ((virt[IO_SUB_FUNC] == outpar[i][IO_SUB_FUNC]) || (!virt[IO_SUB_FUNC]))
					if (virt[IO_FLOOR] == outpar[i][IO_FLOOR])
						if (virt[IO_LIFT] == outpar[i][IO_LIFT])
							if ((virt[IO_DOOR] & outpar[i][IO_DOOR]) || (!outpar[i][IO_DOOR]))
								if (outpar[i][IO_ENABLE] == ENABLE)
								{
									if (virt[IO_STATE] & 0x01)
									{
										bit_set(out, i); /* set physical output					*/
										outpar[i][IO_STATE] = 1;
									}
									else
									{
										bit_reset(out, i); /* reset physical output				*/
										outpar[i][IO_STATE] = 0;
									}
								}
		}
	}
}

void set_io_config(void)
{
	uint8_t i;
	outpush = 0;
	for (i = 0; i < MAX_IO; i++)
	{
		switch (outpar[i][IO_BASIC_FUNC])
		{
		case (SPECIAL_FUNC):
			switch (outpar[i][IO_SUB_FUNC])
			{
			case (FAN_1):
			case (HALLCALL_BYPASS):
			case (DOOR_OPEN):
			case (DOOR_CLOSE):
			case (DOOR_STOP):
				bit_set(outpush, i);
				break;
			default:
				bit_reset(outpush, i);
				break;
			}
			break;

		case (CAR_CALL):
		case (HALL_CALL):
		case (HALL_CALL_SPECIAL):
		case (HALL_CALL_ADVANCED):
		case (HALL_CALL_EMERGENCY):
			bit_set(outpush, i);
			break;

		default:
			bit_reset(outpush, i);
			break;
		}
	}
}
