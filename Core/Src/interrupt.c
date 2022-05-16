
#include "AllHeader.h"

/************************************************************************************************/
/* Low priority interrupt routine																*/
/************************************************************************************************/
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
	

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint8_t i, j;
	static uint8_t a = 0;
	uint8_t buf[2];
	SBYTE pos;
	static uint8_t timer_100ms = 0;
	static uint8_t count = 0;
	static uint8_t timer_cnt = 0;
	if (htim->Instance == htim2.Instance)
	{

		bTime.Time_500ms = 1;
		if (sdo_timer) /* check for SDO transfer time out		*/
			sdo_timer--;
		if (hsetime)
			hsetime--;
		if (!hsetime)	  /* HSE heartbeat time out 				*/
			hsecheck = 1; /* HSE check necessary					*/
		if (can_inittime)
			--can_inittime;
		if (heartbeat) /* decrement heartbeat timer			*/
			heartbeat--;
		if (flashtimer)
			flashtimer--;
		if (landingcalltimer) /* timer for misuse of landing calls	*/
			landingcalltimer--;
		if (display_timer)
			display_timer--;
		if (can_passive_time)
			--can_passive_time;
		com_can_work++;
		timer_cnt++;
		if ((timer_cnt % 2) == 0)
		{
			timer_1S5 = 1;
			bTime.Time_1s = 1;
		}
		if ((timer_cnt % 10) == 0)
		{
			bTime.Time_5s = 1;
		}
			
		bTime.dwin_500ms = 1;

		if (display[BUF_ARROW] == NO_ARROW)
		{
			if ((backlight_time_count++ > backlight_off_time) && (light_para_ok))
				u8Start_Dark = 1;
		}
		else
		{
			backlight_time_count = 0;
			u8Start_Dark = 0;
		}

		for (i = 0; i < MAX_IO; i++) /* search output parameter list			*/
		{
			if (outpar[i][IO_BASIC_FUNC] == ARRIVAL_INDICATION)
				if (outpar[i][IO_ACK]) /* arrival indication is on				*/
				{
					outpar[i][IO_ACK]--;	/* decrement time counter				*/
					if (!outpar[i][IO_ACK]) /* time out								*/
					{
						bit_reset(out, i); /* switch off output					*/
						outpar[i][IO_STATE] = 0;
					}
				}
		}
		if (arrowflash & 0x01)
		{
			if (scroll)
			{
				if (arrowflash & 0x80)
				{
					arrowflash &= ~0x80;
					display[BUF_ARROW] = NO_ARROW;
				}
				else
				{
					arrowflash |= 0x80;
					display[BUF_ARROW] = flashcontent;
				}
			}
			else
				display[BUF_ARROW] = flashcontent;
		}
	}
	if (htim->Instance == htim3.Instance)
	{
		if (!nmtstate) /* only during Boot up					*/
		{
			if (nmtwait) /* waiting time for first heartbeat		*/
				nmtwait--;
		}
		if (!input_detect)
		{
			input[2] = input[1]; /* shift input variables				*/
			input[1] = input[0];
			input[0] = (uint8_t)(GPIOB->IDR >> 11) & 0x0F;
			input[0] = (input[0] ^ 0x0f) & 0x0F; //
			// input[0] = PORTC & 0x0F;
			// input[0] = (input[0] ^ 0xF0) & 0x0F;  //
			for (i = 0; i < 8; i++)
			{
				j = 0x01 << i;
				if (((input[2] & j) == (input[1] & j)) &&
					((input[1] & j) == (input[0] & j)))
				{
					in &= ~j; /* set input valid						*/
					in |= (input[2] & j);
				}
			}
		}
		++input_detect;
		if (input_detect == INPUT_DETECT_INTERVAL)
			input_detect = 0;

		i = out ^ out_polarity; /* invert output if desired				*/
		HAL_GPIO_WritePin(UO_GPIO_Port, UO_Pin, i & 0x01);
		HAL_GPIO_WritePin(DO_GPIO_Port, DO_Pin, bit_select(i, 1));
		// LATBbits.LATB0 = bit_select(i, 1); /* set outputs							*/
		// LATBbits.LATB1 = i & 0x01;
		// LATBbits.LATB4 = bit_select(i, 3);
		// LATBbits.LATB5 = bit_select(i, 2);
	}
	if (htim->Instance == htim4.Instance)
	{
		++count;
		if (hardware_version == VGS_502)
		{
			if ((count % 50) == 0)
			{
				bTime.dwin_100ms = 1;
			}
			if ((count % 50) == 0)
			{
				display_scantimer = 1;
				timer_100ms = 1;
				bTime.Time_100ms = 1;
			}
			if (row < 4) // increment row number
				row++;
			else
			{
				row = 0;
				scrollpos = 0;
			}
			// Buffer for Ten
			if (display[BUF_TEN] < 13)
			{
				buf[0] = sign[38][row];
			}
			else if (display[BUF_TEN] > 12 && display[BUF_TEN] < 16)
			{
				buf[0] = sign[41][row];
			}
			else if (display[BUF_TEN] == 16)
			{
				buf[0] = sign[40][row];
			}
			else
				buf[0] = 0;

			// Buffer for Unit
			if (display[BUF_UNIT] < 13)
			{
				buf[0] |= sign[39][row];
			}
			else if (display[BUF_UNIT] > 12 && display[BUF_UNIT] < 16)
			{
				buf[0] |= sign[43][row];
			}
			else if (display[BUF_UNIT] == 16)
			{
				buf[0] |= sign[42][row];
			}
			else
				buf[0] |= 0;

			// process data
			buf[0] |= ((sign[display[BUF_UNIT]][row] >> 3) |  sign[display[BUF_ARROW]][row]); 		
			buf[1] = (sign[display[BUF_TEN]][row] | ((sign[display[BUF_UNIT]][row] << 5) & 0xE0));  /// 5bit of Ten and 3 bit of unit

			HC959_SEl = 0;
			SPI_SendOneByte(buf[0]);
			SPI_SendOneByte(buf[1]);
			i = (0x01 << row);
			GPIOB->ODR &= 0xfff0;
			HC959_SEl = 1;
			__NOP();
			__NOP();
			__NOP();
			__NOP();
			HC959_SEl = 0;
			__NOP();
			__NOP();
			__NOP();
			__NOP();
			GPIOB->ODR |= (i & 0x0F);

		}
		if (timer_100ms)
		{
			if (mLongKeyTime)
			{
				if (--mLongKeyTime == 0)
					bKey_Fg.LongKeyEn = true;
			}
			if (keytimer)
			{
				if (--keytimer == 0)
				{
					setid_mode = 0;
					arrowflash = arrowflash_old;
					keytimer = 0;
					nmtstate = PRE_OP;
				}
			}
			timer_100ms = 0;
		}
	}
}
#if 0
void interrupt low_priority InterruptHandlerLow(void)
{
	uint8_t i, j, arrow_mes;
	SBYTE pos;
	uint16_t ten, unit;
	uint8_t buf[2];
	static uint8_t timer_100ms = 0;
	static uint8_t count = 0;
	static uint8_t a = 0;
	static uint8_t timer_cnt = 0;

	if (INTCONbits.TMR0IF) /* check for Timer 0 overflow			*/
	{
		INTCONbits.TMR0IF = 0; /* clear interrupt flag					*/
		TMR0H = T0H_500ms;	   /* set timer value (must be set first!)	*/
		TMR0L = T0L_500ms;	   /* set timer value (500 ms)				*/
		bTime.Time_500ms = 1;

		if (sdo_timer) /* check for SDO transfer time out		*/
			sdo_timer--;
		if (hsetime)
			hsetime--;
		if (!hsetime)	  /* HSE heartbeat time out 				*/
			hsecheck = 1; /* HSE check necessary					*/
		if (can_inittime)
			--can_inittime;
		if (heartbeat) /* decrement heartbeat timer			*/
			heartbeat--;
		if (flashtimer)
			flashtimer--;
		if (landingcalltimer) /* timer for misuse of landing calls	*/
			landingcalltimer--;
		if (display_timer)
			display_timer--;
		if (can_passive_time)
			--can_passive_time;
		com_can_work++;
		timer_cnt++;
		if ((timer_cnt % 2) == 0)
			timer_1S5 = 1;
		if ((timer_cnt % 10) == 0)
			bTime.Time_5s = 1;

		if (display[BUF_ARROW] == NO_ARROW)
		{
			if ((backlight_time_count++ > backlight_off_time) && (light_para_ok))
				u8Start_Dark = 1;
		}
		else
		{
			backlight_time_count = 0;
			u8Start_Dark = 0;
		}

		for (i = 0; i < MAX_IO; i++) /* search output parameter list			*/
		{
			if (outpar[i][IO_BASIC_FUNC] == ARRIVAL_INDICATION)
				if (outpar[i][IO_ACK]) /* arrival indication is on				*/
				{
					outpar[i][IO_ACK]--;	/* decrement time counter				*/
					if (!outpar[i][IO_ACK]) /* time out								*/
					{
						bit_reset(out, i); /* switch off output					*/
						outpar[i][IO_STATE] = 0;
					}
				}
		}
		if (arrowflash & 0x01)
		{
			if (scroll)
			{
				if (arrowflash & 0x80)
				{
					arrowflash &= ~0x80;
					display[BUF_ARROW] = NO_ARROW;
				}
				else
				{
					arrowflash |= 0x80;
					display[BUF_ARROW] = flashcontent;
				}
			}
			else
				display[BUF_ARROW] = flashcontent;
		}
	}
	if (PIR1bits.TMR2IF) /*check for Timer 2 overflow			*/
	{
		PIR1bits.TMR2IF = 0; /* clear Timer 2 interrupt flag			*/
		if (!nmtstate)		 /* only during Boot up					*/
		{
			if (nmtwait) /* waiting time for first heartbeat		*/
				nmtwait--;
		}
		if (!input_detect)
		{
			input[2] = input[1]; /* shift input variables				*/
			input[1] = input[0];
			input[0] = PORTC & 0x0F;
			input[0] = (input[0] ^ 0x0F) & 0x0F;  //
			for (i = 0; i < 8; i++)
			{
				j = 0x01 << i;
				if (((input[2] & j) == (input[1] & j)) &&
					((input[1] & j) == (input[0] & j)))
				{
					in &= ~j; /* set input valid						*/
					in |= (input[2] & j);
				}
			}
		}
		++input_detect;
		if (input_detect == INPUT_DETECT_INTERVAL)
			input_detect = 0;

		i = out ^ out_polarity;			   /* invert output if desired				*/
		LATBbits.LATB0 = bit_select(i, 1); /* set outputs							*/
		LATBbits.LATB1 = i & 0x01;
		LATBbits.LATB4 = bit_select(i, 3);
		LATBbits.LATB5 = bit_select(i, 2);
	}

	if (PIR1bits.TMR1IF && PIE1bits.TMR1IE) /* check for Timer 1 overflow			*/
	{
		PIR1bits.TMR1IF = 0; /* clear Timer 1 interrupt flag			*/
		TMR1H = timer1_countH;
		TMR1L = timer1_countL;
		++count;
		if (hardware_version == G_741_LCD)
		{
			if ((count % 50) == 0)
			{
				display_scantimer = 1;
				timer_100ms = 1;
			}
			ten = tFloor_Tab[display[BUF_TEN]];
			unit = tFloor_Tab[display[BUF_UNIT]];

			if (!display_STN_mode)
			{ //���²���ͼ��ʾ
				if ((unit == Char1_RIGHT) && !ten)
					unit = Char1_LEFT;
			}
			else if (display_STN_mode == 1)
			{
				if (unit == Char1_RIGHT)
				{
					if (!ten)
						unit = Char1_LEFT;
					else if (ten == Char1_RIGHT)
					{
						unit = Char1_LEFT;
						ten = Char1_LEFT;
					}
				}
				else if (ten == Char1_RIGHT)
				{
					if (unit != Char7)
						ten = Char1_LEFT;
				}
			}
			/*
								if(!display_STN_mode)
									{//���²���ͼ��ʾ
										if((unit == Char1_RIGHT) && !ten)
											unit = Char1_LEFT;
										unit = Char1_RIGHT;
									}
								else if(display_STN_mode == 1)
									{
										if(unit == Char1_RIGHT)
											{
												if(!ten)
													unit = Char1_RIGHT;
												else if(ten == Char1_RIGHT)
													{
														unit = Char1_RIGHT;
														ten = Char1_RIGHT;
													}
											}
										else if(ten == Char1_RIGHT)
											{
												if(unit != Char7)
													ten = Char1_RIGHT;
											}
									}*/
			arrow_mes = tFloor_Tab[display[BUF_ARROW]];

			//�õ�״̬��Ϣ
			if (display[BUF_MESSAGE] & FULL)
				arrow_mes |= Char_FULL;
			if (display[BUF_MESSAGE] & PARK)
				arrow_mes |= Char_PARK;

			//��ʼ����
			LATCbits.LATC5 = 0;
			if ((a % 2) == 1)
			{
				ten ^= 0xFFFF;
				unit ^= 0xFFFF;
				arrow_mes ^= 0xFF;
			}
			SPI_SendOneByte((uint8_t)arrow_mes);
			SPI_SendOneByte((uint8_t)(unit >> 8));
			SPI_SendOneByte((uint8_t)unit);
			SPI_SendOneByte((uint8_t)(ten >> 8));
			SPI_SendOneByte((uint8_t)ten);

			LATCbits.LATC5 = 1;
			Nop();
			Nop();
			Nop();
			Nop();
			Nop();
			Nop();
			Nop();
			Nop();
			LATCbits.LATC5 = 0;
			if ((a % 2) == 1)
				LATDbits.LATD7 = 1;
			else
				LATDbits.LATD7 = 0;
			a++;

			if (backlight_func)
			{
				if (u8Start_Dark)
				{
					if (backlight_func == BACKLIGHT_DARK)
					{
						if (++u8PWM_Count <= DARK_COUNT)
							CLOSE_BL();
						else
						{
							u8PWM_Count = 0;
							OPEN_BL();
						}
					}
					else if (backlight_func == BACKLIGHT_OFF)
						CLOSE_BL();
				}
				else
					OPEN_BL();
			}
			else
				OPEN_BL();
		}
		if (hardware_version == G_742_LED)
		{
			if ((count % 50) == 0)
			{
				display_scantimer = 1;
				timer_100ms = 1;
			}
			if (row < 6) // increment row number
				row++;
			else
			{
				row = 0;
				if (arrowtype == 0)
				{ //����
					if (scrolltimer < 8)
						scrolltimer++;
					else
						scrolltimer = 0;
					if (!scrolltimer)
					{					 // 8ms ����һ��
						if (scroll == 1) // scroll display up
						{
							if (scrollpos < 8) // set scroll position
								scrollpos++;
							else
								scrollpos = -8;
						}
						else if (scroll == 2) // scroll display down
						{
							if (scrollpos > -8) // set scroll position
								scrollpos--;
							else
								scrollpos = 8;
						}
						else
							scrollpos = 0;
					}
				}
				else
					scrollpos = 0;
			}

			//��λ�Լ���ͷ4-5��
			buf[0] = (sign[display[BUF_UNIT]][row] << 3) | 0x07;
			pos = scrollpos + row;
			if ((pos >= 0) && (pos < 7))
				buf[0] &= (((sign[display[BUF_ARROW]][pos] & 0x18) >> 2) | 0xF8);
			else
				buf[0] &= 0xF8;

			//ʮλ�Լ���ͷ1-3��
			buf[1] = (sign[display[BUF_TEN]][row] << 3) | 0x07;
			if ((pos >= 0) && (pos < 7))
				buf[1] &= ((sign[display[BUF_ARROW]][pos] & 0x07) | 0xF8);
			else
				buf[1] &= 0xF8;

			LATCbits.LATC5 = 0;
			SPI_SendOneByte(buf[0]);
			SPI_SendOneByte(buf[1]);

			i = (0x01 << row);
			LATD = 0x00;

			LATCbits.LATC5 = 1;
			Nop();
			Nop();
			Nop();
			Nop();
			LATCbits.LATC5 = 0;

			//�������ź�,Ȼ������
			LATD = i & 0x7F;
		}
		if (timer_100ms)
		{
			if (mLongKeyTime)
			{
				if (--mLongKeyTime == 0)
					bKey_Fg.LongKeyEn = true;
			}
			if (keytimer)
			{
				if (--keytimer == 0)
				{
					setid_mode = 0;
					arrowflash = arrowflash_old;
					keytimer = 0;
					nmtstate = PRE_OP;
				}
			}
			timer_100ms = 0;
		}
	}
}
#endif
