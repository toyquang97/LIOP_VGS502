/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
//#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define _MAIN_C_
#include "main.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define USER_DEBUG 0
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_IWDG_Init(void);
/* USER CODE BEGIN PFP */
void ClrWdt(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t hihi = 0;
void ClrWdt(void)
{
#if !USER_DEBUG
  HAL_IWDG_Refresh(&hiwdg);

#endif
}

void check_hse(uint8_t mode)
{
  uint8_t help;
  uint8_t i;
  help = 0;

  if (!hsetime) /* 5s no heartbeat from HSE				*/
  {
    help = 1;                             /* set marker							*/
    hsetime = HSETIME;                    /* restart HSE heartbeat timer			*/
    if ((!setid_mode) && (!mSwitch_code)) /* display is for this lift				*/
    {
      // PIE1bits.TMR1IE = 0;           /* disable Timer 2 interrupt			*/
      // HAL_TIM_Base_Stop_IT(&htim4);
      display[BUF_ARROW] = NO_ARROW; /* display no arrow						*/
      display[BUF_TEN] = CHAR_N;     /* display sign for out of work			*/
      display[BUF_UNIT] = CHAR_C;
      // PIE1bits.TMR1IE = 1; /* enable Timer 2 interrupt				*/
      // HAL_TIM_Base_Start_IT(&htim4);

      if (hardware_version == VGS_502)
        arrowflash = 0x01;
      else
        arrowflash = 0x00;
      flashcontent = display[BUF_ARROW];
    }
    hse_heartbeat = 0;
  }
  if (help && mode) /* one or more HSE not available		*/
  {
    for (i = 0; i < MAX_IO; i++) /* check all call indications			*/
    {
      if (check_for_call(outpar[i][IO_BASIC_FUNC]))
      { /* output is call acknowledgement		*/
        outpar[i][IO_ACK] &= ~help;
        if (!outpar[i][IO_ACK]) /* all acknowledgements cancelled		*/
        {
          bit_reset(out, i); /* clear output							*/
          outpar[i][IO_STATE] = 0;
        }
      }
      else /* all other output functions			*/
      {
        if (outpar[i][IO_LIFT] & help) /* output for this lift					*/
        {
          if ((outpar[i][IO_BASIC_FUNC] == SPECIAL_FUNC) &&
              (outpar[i][IO_SUB_FUNC] == OUT_OF_ORDER))
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
}

void init_userpara(void)
{
  uint8_t i;

  display[BUF_ARROW] = NO_ARROW;
  display[BUF_TEN] = 14;
  display[BUF_UNIT] = 15;
  display[BUF_MESSAGE] = 0;

  heartbeat = HEARTBEATTIME;
  display_timer = 0;
  keytimer = 0;
  hsetime = HSETIME; /* start timer with different times 	*/
  nmtwait = 0;
  hsecheck = 0; /* no HSE check now 					*/
  setid_mode = 0;
  setid_mode_old = 0; // set liop can ID mode

  arrowtype = 0;
  if (hardware_version == VGS_502)
    arrowflash = 0x01;
  else
    arrowflash = 0x00;
  flashcontent = NO_ARROW;

  disp_lift = 1;
  device_type = MULTIPLE_DEVICES;
  virtual_device[0] = INPUT_PANEL_UNIT >> 16;
  virtual_device[1] = OUTPUT_PANEL_UNIT >> 16;

  inpush = 0;
  outpush = 0;
  backlight_mode = 0;

  for (i = 0; i < MAX_IO; i++)
  {
    inpar[i][IO_STATE] = 0;
    outpar[i][IO_STATE] = 0;
    inpar[i][IO_BASIC_FUNC] = 0;
    outpar[i][IO_BASIC_FUNC] = 0;
    outpar[i][IO_ACK] = 0;
  }
  for (i = 0; i < MAX_IO_TYPE; i++)
  {
    virt_in[i] = 0;
    virt_out[i] = 0;
  }
  in_polarity = 0;
  out_polarity = 0;

  out = 0;
  // in = PORTC & 0x0F;
  in = (uint8_t)(GPIOB->IDR >> 11) & 0x0F;
  input[0] = in;
  input[1] = in;
  input[2] = in;
  inold = 0;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t i, j;
  uint8_t instate;
  uint8_t help;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  ClrWdt(); // reset watchdog timer
  HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(R3_GPIO_Port, R3_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(R4_GPIO_Port, R4_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(RESET_LED1_GPIO_Port, RESET_LED1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(RESET_LED2_GPIO_Port, RESET_LED2_Pin, GPIO_PIN_SET);
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim4);
  /*-------------gtech ---------------*/
  if (merker == RC_MERKER)
  {
    errorregister |= ER_COMMUNICATION;
  }
  if (merker == BS_MERKER)
  {
    errorregister |= ER_COMMUNICATION;
  }
  merker = WD_MERKER;
  ClrWdt();
  hardware_version = VGS_502;

  // node_id = read_eeprom(EE_NODEID_ADDR);
  //node_id = 0x10;
	Flash_Read_Bytes(&node_id, DATA_START_ADDRESS, 1);

  disp_lift = LIFT1;
  if ((node_id > 127) || (!node_id))
    node_id = ESE_ID;
  ClrWdt();
  nmtstate = BOOT_UP;
  nmtwait = node_id;
  preset_node_id = node_id; //�洢֮ǰ��ID
  nmtwait = (nmtwait * 20 + 1000) / 3 + 600;
  if (node_id == ESE_ID)
    nmtwait = 1000 / 3 + 600;

  init_userpara();

  ClrWdt();

  while (nmtwait)
    ClrWdt();
  Init_Can();
  HAL_Delay(5);
  heartbeat = HEARTBEATTIME;

  nmtstate = PRE_OP;
  while (nmtstate == PRE_OP)
  { //�ȴ������������ָ��
    ClrWdt();
    if (rc)      // Message in receive buffer
      read_rx(); // read and handle message
    if (sdo_index && !sdo_timer)
    { // SDO segment transfer time out
      sdo_index = 0;
      abort_sdo(SDO_TIMEOUT); // send SDO abort request
    }

    if (hsecheck)
    { // HSE heartbeat check necessary
      hsecheck = false;
      check_hse(0); // check if a HSE is not available
    }
    if ((!heartbeat) && (hse_heartbeat) && (!can_inittime)) // time to send heartbeat message
    {
      heartbeat = HEARTBEATTIME;
      sent_heartbeat();
    }

    if (errorcode)
    {                    // error occured
      transmit_error();  // send emergency message
      errorregister = 0; // reset error
      errorcode = 0;
    }
    if ((merker == BS_MERKER) && (!can_inittime))
    {
      Init_Can();
      errorregister |= ER_COMMUNICATION; // set error bits
      errorcode = E_BUS_OFF_A;           // set error code
      merker = 0;
    }
    if (nmtstate == TEST_MODE)
    {
      // test_mode();
    }
  }

  i = 0;
  j = 0;

  hsetime = HSETIME; /* start timer with different times		*/
  hsecheck = 0;      /* no HSE check now						*/
  ClrWdt();          /* reset watchdog timer					*/
  inold = 0;
  set_io_config();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    ClrWdt(); /* reset watchdog timer					*/
    if (rc)
    {
      read_rx();
    }
    // #if 0
    if (display_scantimer)
    {
      KeyScan();
      KeyProg(mKeyValue);
      if (setid_mode)
        SetNodeId();
      else if ((nmtstate == TEST_MODE) || ((mSwitch_code == 1) && (com_can_work > 10))) /* enter test mode						*/
        TestMode();
      display_scantimer = 0;
    }
    // if (sdo_index && !sdo_timer) /* SDO segment transfer time out		*/
    // {
    //   sdo_index = 0;
    //   abort_sdo(SDO_TIMEOUT); /* send SDO abort request				*/
    // }
    if ((!heartbeat) && (hse_heartbeat) && (!can_inittime) && (!setid_mode)) /* time to send heartbeat message		*/
    {
      heartbeat = HEARTBEATTIME; /* restart heartbeat timer				*/
      sent_heartbeat();
    }
    // if (errorcode) /* error occured						*/
    // {
    //   transmit_error();  /* send emergency message				*/
    //   errorregister = 0; /* reset error							*/
    //   errorcode = 0;
    // }
    if (hsecheck) /* HSE heartbeat check necessary		*/
    {
      hsecheck = 0;
      check_hse(1); /* check if a HSE is not available		*/
    }
    // if (((merker == BS_MERKER) && (!can_inittime)) || (com_can_work > 40))
    // {
    //   merker = 0;
    //   com_can_work = 0;
    //   errorregister |= ER_COMMUNICATION;
    //   errorcode = E_BUS_OFF_B;
    // }
    if (nmtstate == PRE_OP)
    {
      instate = in ^ in_polarity;                    /* read input state; invert if desired	*/
      for (i = 0; i < MAX_IO; i++)                   /* check all inputs						*/
        inpar[i][IO_STATE] = bit_select(instate, i); /* set input state						*/
    }
    if (nmtstate == OPERATIONAL)
    {
      instate = in ^ in_polarity; /* read input state; invert if desired	*/
      if (instate != inold)       /* input state changed					*/
      {
        for (i = 0; i < MAX_IO; i++) /* check all inputs 					*/
        {
          help = bit_select(instate, i);
          if (help != bit_select(inold, i)) /* input has changed					*/
          {
            inpar[i][IO_STATE] = help;   /* set input state						*/
            if (inpar[i][IO_BASIC_FUNC]) /* input has a function 				*/
            {
              for (j = 0; j < MAX_IO_TYPE; j++) /* write input to virtual input object	*/
                virt_in[j] = inpar[i][j];

              switch (inpar[i][IO_BASIC_FUNC])
              {
              case (CAR_CALL): /* standard car call					*/
                if (help)
                  transmit_in(virt_in);
                break;
              case (HALL_CALL): /* standard hall call 				*/
                if (!setid_mode)
                { /* landing call misuse					*/
                  if ((bTime.Time_100ms) || (inpar[i][IO_FLOOR] != landingcallfloor))
                  //if ((!landingcalltimer) || (inpar[i][IO_FLOOR] != landingcallfloor))
                  {
                    transmit_in(virt_in);
                  }
									//landingcalltimer = 1; 
                  bTime.Time_100ms = 0; 
                  landingcallfloor = inpar[i][IO_FLOOR];
                }
                break;

              default:
                transmit_in(virt_in);
                break;
              }
            }
          }
        }
        inold = instate;
      }
    }
    //#endif
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief CAN Initialization Function
 * @param None
 * @retval None
 */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 45;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */
}

/**
 * @brief IWDG Initialization Function
 * @param None
 * @retval None
 */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7199;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7199;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 29;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 7199;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 19;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, HC595_SEL_Pin | RESET_LED1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED1_Pin | LED2_Pin | LED3_Pin | SCK_Pin | LED4_Pin | SDO_Pin | RESET_LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, R1_Pin | R2_Pin | R3_Pin | R4_Pin | DO_Pin | UO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : HC595_SEL_Pin LED1_Pin LED2_Pin LED3_Pin
                           SCK_Pin LED4_Pin SDO_Pin RESET_LED1_Pin
                           RESET_LED2_Pin */
  GPIO_InitStruct.Pin = HC595_SEL_Pin | LED1_Pin | LED2_Pin | LED3_Pin | SCK_Pin | LED4_Pin | SDO_Pin | RESET_LED1_Pin | RESET_LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : R1_Pin R2_Pin R3_Pin R4_Pin
                           DO_Pin UO_Pin */
  GPIO_InitStruct.Pin = R1_Pin | R2_Pin | R3_Pin | R4_Pin | DO_Pin | UO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : UK_IN_Pin DK_IN_Pin FIRE_IN_Pin LOCK_IN_Pin
                           NODE_ID_Pin */
  GPIO_InitStruct.Pin = UK_IN_Pin | DK_IN_Pin | FIRE_IN_Pin | LOCK_IN_Pin | NODE_ID_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
