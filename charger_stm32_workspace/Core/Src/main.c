/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

struct CANMessage {
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;
	uint8_t data[8];
};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
HAL_StatusTypeDef CAN_Start(void);
HAL_StatusTypeDef CAN_Activate(void);
void CAN_SettingsInit(struct CANMessage *ptr);
HAL_StatusTypeDef CAN_Send(struct CANMessage *ptr);
void CAN_Send_Test(struct CANMessage *ptr);
HAL_StatusTypeDef CAN_Send_Simple(void);
/* USER CODE BEGIN PFP */
void setOutputPins(
		uint8_t BRUSA_PON_SIG_State,
		uint8_t HVIL_MCU_OUT_State,
		uint8_t FANS_SIG_State,
		uint8_t MCU_PCC_State,
		uint8_t MCU_MC_State,
		uint8_t HV_LED_State);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  	  int pre_charge_occured = 0;
  	  setOutputPins(0, 0, 0, 0, 0, 0);

  struct CANMessage message;
  CAN_SettingsInit(&message);
  CAN_Send_Test(&message);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      if (HAL_GPIO_ReadPin(HVIL_MCU_IN_GPIO_Port, HVIL_MCU_IN_Pin) == 1 && HAL_GPIO_ReadPin(BRUSA_EN_IN_GPIO_Port, BRUSA_EN_IN_Pin) == 1 && pre_charge_occured == 1) {
    	  // Charge state
    	  setOutputPins(1, 1, 1, 0, 1, 1);
      } else if (HAL_GPIO_ReadPin(HVIL_MCU_IN_GPIO_Port, HVIL_MCU_IN_Pin) == 1 && HAL_GPIO_ReadPin(BRUSA_EN_IN_GPIO_Port, BRUSA_EN_IN_Pin) == 0 && pre_charge_occured == 1) {
    	  // Idle state
    	  setOutputPins(0, 1, 1, 0, 1, 1);
      } else if (HAL_GPIO_ReadPin(HVIL_MCU_IN_GPIO_Port, HVIL_MCU_IN_Pin) == 1 && pre_charge_occured == 0) {
    	  // Pre-charge state
          setOutputPins(0, 1, 0, 1, 0, 0);
          for (int delay = 0; delay < 39; delay++)
          {
            HAL_GPIO_TogglePin(GPIOB, HV_LED_Pin);
            HAL_Delay(100);
          }
          setOutputPins(0, 1, 1, 0, 1, 1);
          pre_charge_occured = 1;
      } else if (HAL_GPIO_ReadPin(HVIL_MCU_IN_GPIO_Port, HVIL_MCU_IN_Pin) == 0) {
    	  // X state
    	  setOutputPins(0, 0, 0, 0, 0, 0);
    	  pre_charge_occured = 0;
      }
  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 16;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

HAL_StatusTypeDef CAN_Start() {
	return HAL_CAN_Start(&hcan1);
}

HAL_StatusTypeDef CAN_Activate() {
	return HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void CAN_SettingsInit(struct CANMessage *ptr) {
	CAN_Start();
	CAN_Activate();
	ptr->TxHeader.IDE = CAN_ID_STD;
	ptr->TxHeader.StdId = 0x00;
	ptr->TxHeader.RTR = CAN_RTR_DATA;
	ptr->TxHeader.DLC = 8;
}


HAL_StatusTypeDef CAN_Send(struct CANMessage *ptr) {
	return HAL_CAN_AddTxMessage(&hcan1, &ptr->TxHeader, ptr->data, &ptr->TxMailbox);

}

void CAN_Send_Test(struct CANMessage *ptr) {


	uint16_t CAN_ID = 0x100; /* I made up a random number for the CAN_ID. Should it be something specific?*/
	ptr->TxHeader.StdId = CAN_ID; /* sets CAN ID in TxHeader. TxHeader is configured in CAN_SettingsInit() */


	ptr->data[0] = 0x2A; /* This is the CAN message that will be sent*/
	ptr->data[1] = 0x12;
	ptr->data[2] = 0x5B;
	ptr->data[3] = 0x77;
	ptr->data[4] = 0x6C;
	ptr->data[5] = 0x89;
	ptr->data[6] = 0x1D;
	ptr->data[7] = 0x3F;

	CAN_Send(ptr);

}

/* Simpler function that handles all parts of sending the message in one function */
HAL_StatusTypeDef CAN_Send_Simple() {
	CAN_TxHeaderTypeDef TxHeaderSimple;
	TxHeaderSimple.DLC = 8;
	TxHeaderSimple.StdId = 0x15A;
	TxHeaderSimple.IDE = CAN_ID_STD;
	TxHeaderSimple.RTR = CAN_RTR_DATA;



	uint8_t data[8] = {0x2a, 0x12, 0x5B, 0x77, 0x6C, 0x89, 0x1D, 0x3F};

	uint32_t TxMailbox;

	return HAL_CAN_AddTxMessage(&hcan1, &TxHeaderSimple, data, &TxMailbox);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, MCU_PCC_Pin|MCU_MC_Pin|FANS_SIG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BRUSA_PON_SIG_Pin|HVIL_MCU_OUT_Pin|HV_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : MCU_PCC_Pin MCU_MC_Pin FANS_SIG_Pin */
  GPIO_InitStruct.Pin = MCU_PCC_Pin|MCU_MC_Pin|FANS_SIG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BRUSA_PON_SIG_Pin HVIL_MCU_OUT_Pin HV_LED_Pin */
  GPIO_InitStruct.Pin = BRUSA_PON_SIG_Pin|HVIL_MCU_OUT_Pin|HV_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BRUSA_EN_IN_Pin */
  GPIO_InitStruct.Pin = BRUSA_EN_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BRUSA_EN_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : HVIL_MCU_IN_Pin */
  GPIO_InitStruct.Pin = HVIL_MCU_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(HVIL_MCU_IN_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void setOutputPins(
		  uint8_t BRUSA_PON_SIG_State,
		  uint8_t HVIL_MCU_OUT_State,
		  uint8_t FANS_SIG_State,
		  uint8_t MCU_PCC_State,
		  uint8_t MCU_MC_State,
		  uint8_t HV_LED_State) {
    HAL_GPIO_WritePin(GPIOB, BRUSA_PON_SIG_Pin, BRUSA_PON_SIG_State);
    HAL_GPIO_WritePin(GPIOB, HVIL_MCU_OUT_Pin, HVIL_MCU_OUT_State);
    HAL_GPIO_WritePin(GPIOC, FANS_SIG_Pin, FANS_SIG_State);
    HAL_GPIO_WritePin(GPIOC, MCU_PCC_Pin, MCU_PCC_State);
    HAL_GPIO_WritePin(GPIOC, MCU_MC_Pin, MCU_MC_State);
    HAL_GPIO_WritePin(GPIOB, HV_LED_Pin, HV_LED_State);
}
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

#ifdef  USE_FULL_ASSERT
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
