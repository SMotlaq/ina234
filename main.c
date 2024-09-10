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
#include "i2c.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ina234.h"

#define SAMPLES_PER_BATCH 500
#define TIME_CALC			0

// Print setting -------------------
#define DEBUG_ENABLE  1
#define USB_DEBUG     1
#define DEBUG_UART    (&huart1)
// ---------------------------------

#if DEBUG_ENABLE
  #include "stdarg.h"
  #include "string.h"
  #include "stdlib.h"

  #if USB_DEBUG
    #include "usbd_cdc_if.h"
  #endif
#endif

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

/* USER CODE BEGIN PV */
INA234 ina234;

#if TIME_CALC
	uint32_t ptime = 0;
	double conv_time = 0;
	double sampling_rate = 0.0;
#endif

uint8_t raw[2];
uint8_t TxBuffer[SAMPLES_PER_BATCH*2];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void DEBUG(const char* _str, ...);
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
  MX_I2C1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
	
	HAL_Delay(2000);
	
	//INA234_SoftResetAll(&ina234);
	//HAL_Delay(2000);
	
	if(STATUS_OK == INA234_init(&ina234, 0x48, &hi2c1, 1, RANGE_20_48mV, NADC_1, CTIME_140us, CTIME_140us, MODE_CONTINUOUS_SHUNT) &&
		 STATUS_OK == INA234_alert_init(&ina234, ALERT_SHUNT_OVER_LIMIT, ALERT_ACTIVE_LOW, ALERT_TRANSPARENT, ALERT_CONV_DISABLE, 2.5)){
		
		//*/
			DEBUG("Manufacturer ID is 0x%04X \r\n", INA234_getManID(&ina234));
			DEBUG("      Device ID is 0x%04X \r\n", INA234_getDevID(&ina234));
		//*/
		
		while(1){
			
			/*/ Read seperately ----------------------------
				INA234_getShuntVoltage(&ina234);
				INA234_getBusVoltage(&ina234);
				INA234_getPower(&ina234);
				INA234_getCurrent(&ina234);
				DEBUG("Shunt Voltage: %.3fmV \t Bus Voltage: %.2fV \t Current: %.2fA \t Power: %.2fW\r\n", ina234.ShuntVoltage, ina234.BusVoltage, ina234.Current, ina234.Power);
				HAL_Delay(200);
			//*/
			
			/*/ Fast read ---------------------------------
				#if TIME_CALC
					ptime = HAL_GetTick();
				#endif
				for(int32_t i=0; i<SAMPLES_PER_BATCH; i++){
					HAL_I2C_Mem_Read(&hi2c1, 0x48<<1, SHUNT_VOLTAGE_REGISTER, I2C_MEMADD_SIZE_8BIT, raw, 2, 100);
					TxBuffer[i * 2 + 0] = raw[0];
					TxBuffer[i * 2 + 1] = raw[1];
				}
				CDC_Transmit_FS(TxBuffer, SAMPLES_PER_BATCH*2);
				#if TIME_CALC
					conv_time = (HAL_GetTick() - ptime) / ((double)SAMPLES_PER_BATCH);
					sampling_rate = 1/conv_time;
				#endif
			//*/
			
			//*/ Read all -----------------------------------
				INA234_readAll(&ina234);
				DEBUG("Shunt Voltage: %.3fmV \t Bus Voltage: %.2fV \t Current: %.2fA \t Power: %.2fW\r\n", ina234.ShuntVoltage, ina234.BusVoltage, ina234.Current, ina234.Power);
				HAL_Delay(200);
			//*/
			
		}
	}
	else{
		DEBUG("----- INA234 init failed -----\r\n");
	}
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void DEBUG(const char* _str, ...){
  #if DEBUG_ENABLE
    va_list args;
    va_start(args, _str);
    char buffer[150];
    memset(buffer, 0, 150);
    int buffer_size = vsprintf(buffer, _str, args);
    #if USB_DEBUG
      CDC_Transmit_FS((uint8_t*) buffer, buffer_size);
    #else
      HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buffer, buffer_size, 5000);
    #endif
  #endif
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
