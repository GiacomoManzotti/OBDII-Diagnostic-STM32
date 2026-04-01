/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include <string.h>	//used for strlen();
#include <stdio.h>	//used for sprintf();
#include <stdlib.h> //used for strtol();
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

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_CAN1_Init(void);
/* USER CODE BEGIN PFP */
void obd_submenu_and_handle (void); //Main menu with PID selection
void CAN_Send_RPM_Request (void);	//Send CAN frame regarding RPM used only for testing
void CAN_Send_Request(void);		//Send CAN frame
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t generic_pid;
typedef enum
{
    OBD_IDLE = 0,
    OBD_RPM,
    OBD_SPEED,
    OBD_OTEMP,
    OBD_LOAD,
    OBD_HEX
} OBD_State_t;

volatile OBD_State_t obd_state = OBD_IDLE;
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
  MX_USART1_UART_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  HAL_CAN_Start(&hcan1);
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); //To activate interrupt from CAN

  uint8_t msg_avvio[] = "Avvio diagnostica\r\n";
  HAL_UART_Transmit(&huart1, msg_avvio, sizeof(msg_avvio)-1, HAL_MAX_DELAY);

  //CAN_RxHeaderTypeDef rxHeader;
  //uint8_t rxData[8];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  obd_submenu_and_handle(); //Main menu with PID selection
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  CAN_Send_Request(); //Send CAN frame

	  HAL_Delay(500);

	  //polling attempt, not used since interrupt works correctly
	  /*while (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0)
	  {
		  HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxHeader, rxData);

		  char msg[64];
		  sprintf(msg, "ID:%03lX DATA: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
				  rxHeader.StdId,
				  rxData[0], rxData[1], rxData[2], rxData[3],
				  rxData[4], rxData[5], rxData[6], rxData[7]);

		  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);
	  }*/

	  //HAL_Delay(400);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  hcan1.Init.Prescaler = 6;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
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
  //filter settings
  CAN_FilterTypeDef filter = {0};
  filter.FilterBank = 0;
  filter.SlaveStartFilterBank = 14;      // banco CAN2 (non usato)
  filter.FilterMode = CAN_FILTERMODE_IDMASK;
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.FilterIdHigh = 0x0000;			// All packets accepted
  filter.FilterIdLow = 0x0000;			// All packets accepted
  filter.FilterMaskIdHigh = 0x0000;		// All packets accepted
  filter.FilterMaskIdLow = 0x0000;		// All packets accepted
  filter.FilterFIFOAssignment = CAN_RX_FIFO0;
  filter.FilterActivation = ENABLE;

  if (HAL_CAN_ConfigFilter(&hcan1, &filter) != HAL_OK) Error_Handler();

  // Avvia CAN1
  if (HAL_CAN_Start(&hcan1) != HAL_OK) Error_Handler();

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void uart_print(const char *s) {	// Helper function to send data over UART
    HAL_UART_Transmit(&huart1, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
}
void obd_submenu_and_handle(void)
{
    uart_print("Sottomenu OBD: scegli il PID da leggere:\r\n");
    uart_print("1 -> RPM (0x0C)\r\n");
    uart_print("2 -> Engine Load (0x04)\r\n");
    uart_print("3 -> Speed (0x0D)\r\n");
    uart_print("4 -> Oil Temp (0x5C)\r\n");
    uart_print("5 -> Inserisci PID esadecimale (00-FF)\r\n");
    uart_print("Digita il numero e premi invio (o invia il singolo char)...\r\n");

    uint8_t rx;
    // wait for one char (long timeout)
    while (1) {
        if (HAL_UART_Receive(&huart1, &rx, 1, 10000000) == HAL_OK) {
            // converti
            if (rx == '1') {
                uart_print("Richiesta: RPM\r\n");
                //CAN_Send_RPM_Request();
                obd_state = OBD_RPM;
                break;
            } else if (rx == '2') {
                uart_print("Richiesta: Engine Load\r\n");
                obd_state = OBD_LOAD;
                break;
            } else if (rx == '3') {
                uart_print("Richiesta: Speed\r\n");
                obd_state = OBD_SPEED;
                break;
            } else if (rx == '4') {
                uart_print("Richiesta: Oil Temp\r\n");
                obd_state = OBD_OTEMP;
                break;
            } else if (rx == '5') {
            	//noted PIDs for test: 0x11 butterfly position; 0x05 coolant; 0x0f intake temp; 0x0d speed
                uart_print("Inserisci PID esadecimale (00-FF) e premi invio:\r\n");

                #define UART_BUF_SIZE 3  // max two char + terminator
                char uart_buf[UART_BUF_SIZE];
                uint8_t idx = 0;
                uint8_t c;

                // save what is received from uart until enter
                while (1) {
                    if (HAL_UART_Receive(&huart1, &c, 1, HAL_MAX_DELAY) == HAL_OK) {
                        if (c == '\r' || c == '\n') {
                            uart_buf[idx] = 0;  // string terminator
                            break;
                        }
                        else if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
                            if (idx < UART_BUF_SIZE - 1) {
                                uart_buf[idx++] = c;       // add to buffer
                                HAL_UART_Transmit(&huart1, &c, 1, HAL_MAX_DELAY); // echo
						}
					} else { // error for wrong input
						uart_print("\r\nCarattere non valido, inserisci solo 0-9 o A-F\r\n");
					}
				}
			}

			// string converted to uint8_t
			uint8_t pid = (uint8_t)strtol(uart_buf, NULL, 16);

			char msg[64];
			sprintf(msg, "\r\nRichiesta PID esadecimale: 0x%02X\r\n", pid);
			uart_print(msg);

			obd_state = OBD_HEX;
			generic_pid = pid;
			break;
            } else { // Error wrong quick input
                uart_print("Scelta non valida, riprova\r\n");
            }
        } else { // Error timeout
            uart_print("Timeout attesa input UART\r\n");
            break;
        }
    }
}
void CAN_Send_Request(void)
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8] = {0};
    uint32_t txMailbox;

    txHeader.StdId = 0x7DF;   // Some cars may prefer: 0x7E0
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;

    txData[0] = 0x02; // length
    txData[1] = 0x01; // mode: show currant data
    switch (obd_state)
	{
		case OBD_RPM:
		{
			txData[2] = 0x0C; // PID RPM
			break;
		}

		case OBD_SPEED:
		{
			txData[2] = 0x0D; // PID SPEED
			break;
		}

		case OBD_OTEMP:
		{
			txData[2] = 0x5C; // PID OIL TEMP
			break;
		}

		case OBD_LOAD:
		{
			txData[2] = 0x04; // PID ENGINE LOAD
			break;
		}

		case OBD_HEX:
		default:
		{
			txData[2] = generic_pid;
            uart_print("OBD_HEX selected");
			break;
		}
	}

    HAL_CAN_AddTxMessage(&hcan1, &txHeader, txData, &txMailbox);	//send can message
}
void CAN_Send_RPM_Request(void) //asks only for rpm
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8] = {0};
    uint32_t txMailbox;

    txHeader.StdId = 0x7DF;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;

    txData[0] = 0x02; // length
    txData[1] = 0x01; // mode: show current data
    txData[2] = 0x0C; // PID RPM

    HAL_CAN_AddTxMessage(&hcan1, &txHeader, txData, &txMailbox);
}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) //function called by the CAN interrupt
{
	CAN_RxHeaderTypeDef rxHeader;
	uint8_t rxData[8];

	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData);

	if (rxHeader.StdId >= 0x7E8 && rxHeader.StdId <= 0x7EF)	//check answerer's ID
	{
		char msg[64];

		switch (obd_state)
		{
			case OBD_RPM:
			{
				uint16_t rpm = ((rxData[3] << 8) | rxData[4]) / 4;	//equation to parse RPMs data
				sprintf(msg, "RPM: %d\r\n", rpm);
				break;
			}
			case OBD_SPEED:
			{
				uint8_t speed = rxData[3];							//equation to parse speed data
				sprintf(msg, "Speed: %d km/h\r\n", speed);
				break;
			}
			case OBD_OTEMP:
			{
				int temp = rxData[3] - 40;							//equation to parse Oil Temp data
				sprintf(msg, "Oil temp: %d C\r\n", temp);
				break;
			}
			case OBD_LOAD:
			{
				int load = (rxData[3] * 100) / 255;					//equation to parse Load data
				sprintf(msg, "Load: %d %%\r\n", load);
				break;
			}
			case OBD_HEX:
			default:
			{
				sprintf(msg,										//printing raw data received to be parsed outside
						"RAW: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
						rxData[0], rxData[1], rxData[2], rxData[3],
						rxData[4], rxData[5], rxData[6], rxData[7]);
				break;
			}
		}
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);
	}
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
