/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>
#include <stdio.h>

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
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
volatile uint32_t g_msTicks = 0;

volatile uint16_t g_adcValue = 0;
volatile uint8_t g_adcPct = 0;
volatile uint8_t g_adcValid = 0;

static uint32_t adcLastSampleMs = 0;
static volatile uint8_t adcBusy = 0;

volatile uint32_t g_adcSampleCount = 0;
volatile uint32_t g_adcLastInterval = 0;
volatile uint32_t g_adcLastTime = 0;

static uint32_t ledLastToggleMs = 0;
static uint32_t uartLastTelemetryMs = 0U;

volatile uint8_t g_buttonRaw = 0;

/* Control del boton USER B1 - PC13 */
static uint8_t buttonRawState = GPIO_PIN_SET;
static uint8_t buttonStableState = GPIO_PIN_SET;
static uint32_t buttonLastChangeMs = 0;

volatile uint32_t g_buttonPressCount = 0;

/* Modos de funcionamiento */
#define MODE_RUN   0U
#define MODE_PAUSE 1U

volatile uint8_t g_systemMode = MODE_RUN;

/* Control de alarma */
#define ALARM_THRESHOLD_DEFAULT 70U
#define ALARM_HYSTERESIS         5U

volatile uint8_t g_alarmThreshold = ALARM_THRESHOLD_DEFAULT;
volatile uint8_t g_alarmActive = 0U;

static uint8_t alarmForceReevaluate = 1U;

/* Mensaje de bienvenida UART */
static uint8_t uartWelcomeMsg[] =
    "\r\nSEGUNDO PARCIAL - STM32\r\n"
    "UART USART2 INICIADA\r\n";

/* Recepcion UART por interrupciones */
#define UART_MAX_LINE_LENGTH 32U
#define UART_RX_BUFFER_SIZE (UART_MAX_LINE_LENGTH + 1U)

static uint8_t uartRxByte;

#define UART_QUEUE_SIZE 8U

#define UART_EVENT_COMMAND 1U
#define UART_EVENT_ERROR   2U

typedef struct
{
    uint8_t type;
    char text[UART_RX_BUFFER_SIZE];
} UART_Event_t;

static uint8_t uartRxIndex = 0U;
static char uartRxBuffer[UART_RX_BUFFER_SIZE];

/* Cola FIFO de eventos UART */
static UART_Event_t uartQueue[UART_QUEUE_SIZE];

static volatile uint8_t uartQueueHead = 0U;
static volatile uint8_t uartQueueTail = 0U;
static volatile uint8_t uartQueueCount = 0U;

/* Cola de transmision UART */
#define UART_TX_QUEUE_SIZE 8U
#define UART_TX_MESSAGE_SIZE 96U

static char uartTxQueue[UART_TX_QUEUE_SIZE][UART_TX_MESSAGE_SIZE];

static volatile uint8_t uartTxHead = 0U;
static volatile uint8_t uartTxTail = 0U;
static volatile uint8_t uartTxCount = 0U;
static volatile uint8_t uartTxBusy = 0U;

static volatile uint32_t uartTxOverflowCount = 0U;

/* Indica que la linea actual debe descartarse hasta LF */
static volatile uint8_t uartDiscardLine = 0U;

/* Estadisticas de recepcion */
static volatile uint32_t uartLinesReceived = 0U;
static volatile uint32_t uartRxOverflowCount = 0U;



static const char uartErrorMsg[] = "ERR\r\n";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
static void Button_Update(uint32_t now);

static void Alarm_Update(void);

static void UART_SendResponse(const char *message);

static void UART_SendStatus(void);

static uint8_t UART_ParseThreshold(const char *command, uint8_t *threshold);
static void UART_QueuePush(uint8_t type, const char *text);

static uint8_t UART_TxEnqueue(const char *message);
static void UART_TxStartNext(void);

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
  MX_ADC1_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  if (HAL_TIM_Base_Start_IT(&htim6) != HAL_OK)
  {
      Error_Handler();
  }

  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
  {
      Error_Handler();
  }

  /* Enviar mensaje de bienvenida por USART2 */
  HAL_UART_Transmit(
      &huart2,
      uartWelcomeMsg,
      sizeof(uartWelcomeMsg) - 1U,
      100U
  );

  /* Iniciar recepcion UART por interrupciones */
  if (HAL_UART_Receive_IT(&huart2, &uartRxByte, 1U) != HAL_OK)
  {
      Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      /* USER CODE END WHILE */

      /* USER CODE BEGIN 3 */

      uint32_t now = g_msTicks;
      Button_Update(now);
      Alarm_Update();
      g_buttonRaw = (uint8_t)HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

      /* ADC cada 100 ms */
      if ((uint32_t)(now - adcLastSampleMs) >= 100U)
      {
          adcLastSampleMs += 100U;

          if (adcBusy == 0U)
          {
              adcBusy = 1U;

              if (HAL_ADC_Start_IT(&hadc1) != HAL_OK)
              {
                  adcBusy = 0U;
              }
          }
      }

      /* LED cada 500 ms */
      if ((uint32_t)(now - ledLastToggleMs) >= 500U)
      {
          ledLastToggleMs += 500U;

          HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
      }

      /* Telemetria automatica cada 1000 ms */
      if ((uint32_t)(now - uartLastTelemetryMs) >= 1000U)
      {
          uartLastTelemetryMs += 1000U;
          UART_SendStatus();
      }

      /* Procesar una linea UART recibida */
      /* Procesar el siguiente evento UART de la cola FIFO */
      if (uartQueueCount > 0U)
      {
          UART_Event_t eventToProcess;
          uint8_t hasEvent = 0U;

          /* Extraer el evento de forma segura */
          __disable_irq();

          if (uartQueueCount > 0U)
          {
              eventToProcess = uartQueue[uartQueueTail];

              uartQueueTail =
                  (uint8_t)((uartQueueTail + 1U) % UART_QUEUE_SIZE);

              uartQueueCount--;
              hasEvent = 1U;
          }

          __enable_irq();

          if (hasEvent != 0U)
          {
              if (eventToProcess.type == UART_EVENT_ERROR)
              {
                  UART_SendResponse(uartErrorMsg);
              }
              else if (eventToProcess.type == UART_EVENT_COMMAND)
              {
                  const char *commandToProcess = eventToProcess.text;

                  if (strcmp(commandToProcess, "STATUS") == 0)
                  {
                      UART_SendStatus();
                  }
                  else if (strcmp(commandToProcess, "STOP") == 0)
                  {
                      g_systemMode = MODE_PAUSE;
                      Alarm_Update();

                      UART_SendResponse("OK PAUSE\r\n");
                  }
                  else if (strcmp(commandToProcess, "START") == 0)
                  {
                      if (g_systemMode == MODE_PAUSE)
                      {
                          g_systemMode = MODE_RUN;
                          Alarm_Update();
                      }

                      UART_SendResponse("OK RUN\r\n");
                  }
                  else if (strncmp(commandToProcess, "THR ", 4U) == 0)
                  {
                      uint8_t newThreshold;

                      if (UART_ParseThreshold(
                              commandToProcess,
                              &newThreshold) != 0U)
                      {
                          char response[24];

                          g_alarmThreshold = newThreshold;

                          if (g_systemMode == MODE_RUN)
                          {
                              alarmForceReevaluate = 1U;
                              Alarm_Update();
                          }

                          snprintf(
                              response,
                              sizeof(response),
                              "OK THR=%u\r\n",
                              (unsigned int)newThreshold
                          );

                          UART_SendResponse(response);
                      }
                      else
                      {
                          UART_SendResponse(uartErrorMsg);
                      }
                  }
                  else
                  {
                      UART_SendResponse(uartErrorMsg);
                  }
              }
          }
      }
      /* Iniciar el siguiente mensaje pendiente de la cola TX */
      UART_TxStartNext();
      /* USER CODE END 3 */
  }
      /* USER CODE END WHILE */
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC1;
  PeriphClkInit.Adc1ClockSelection = RCC_ADC1PLLCLK_DIV1;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_61CYCLES_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 79;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 99;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_alarm_Pin|LED_activity_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BTN_User_Pin */
  GPIO_InitStruct.Pin = BTN_User_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_User_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_alarm_Pin LED_activity_Pin */
  GPIO_InitStruct.Pin = LED_alarm_Pin|LED_activity_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        g_msTicks++;
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        uint32_t value = HAL_ADC_GetValue(hadc);

        g_adcValue = (uint16_t)value;
        g_adcPct = (uint8_t)((100UL * value + 2047UL) / 4095UL);
        g_adcValid = 1;
        uint32_t now = g_msTicks;

        if (g_adcSampleCount > 0U)
        {
            g_adcLastInterval = now - g_adcLastTime;
        }

        g_adcLastTime = now;
        g_adcSampleCount++;
        adcBusy = 0;
    }
}

static void Button_Update(uint32_t now)
{
    uint8_t raw = (uint8_t)HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

    /* Detectar cualquier cambio instantaneo */
    if (raw != buttonRawState)
    {
        buttonRawState = raw;
        buttonLastChangeMs = now;
    }

    /* Confirmar que el estado lleva 30 ms estable */
    if ((uint32_t)(now - buttonLastChangeMs) >= 30U)
    {
        if (buttonStableState != buttonRawState)
        {
            buttonStableState = buttonRawState;

            /* Pulsacion valida: PC13 pasa a nivel bajo */
            if (buttonStableState == GPIO_PIN_RESET)
            {
                g_buttonPressCount++;

                if (g_systemMode == MODE_RUN)
                {
                    g_systemMode = MODE_PAUSE;
                }
                else
                {
                    g_systemMode = MODE_RUN;
                }
            }
        }
    }
}

static void Alarm_Update(void)
{
    /* En PAUSE la alarma siempre permanece apagada */
    if (g_systemMode == MODE_PAUSE)
    {
        g_alarmActive = 0U;
        alarmForceReevaluate = 1U;
    }
    else if (g_adcValid == 0U)
    {
        /* Sin lectura valida, mantener alarma apagada */
        g_alarmActive = 0U;
    }
    else if (alarmForceReevaluate != 0U)
    {
        /* Entrada a RUN: evaluar sin estado anterior */
        g_alarmActive =
            (g_adcPct >= g_alarmThreshold) ? 1U : 0U;

        alarmForceReevaluate = 0U;
    }
    else if (g_alarmActive == 0U)
    {
        /* Encendido al alcanzar el umbral */
        if (g_adcPct >= g_alarmThreshold)
        {
            g_alarmActive = 1U;
        }
    }
    else
    {
        /* Apagado con histeresis de 5 puntos */
        uint8_t lowerThreshold =
            (g_alarmThreshold >= ALARM_HYSTERESIS)
            ? (g_alarmThreshold - ALARM_HYSTERESIS)
            : 0U;

        if (g_adcPct < lowerThreshold)
        {
            g_alarmActive = 0U;
        }
    }

    /* Actualizar salida fisica PB12 */
    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_12,
        (g_alarmActive != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );
}

static uint8_t UART_TxEnqueue(const char *message)
{
    size_t length = strlen(message);

    /* Verificar que el mensaje cabe en el buffer */
    if (length == 0U || length >= UART_TX_MESSAGE_SIZE)
    {
        uartTxOverflowCount++;
        return 0U;
    }

    /* Proteger la cola frente a la interrupcion UART */
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    /* Verificar que existe espacio en la cola */
    if (uartTxCount >= UART_TX_QUEUE_SIZE)
    {
        uartTxOverflowCount++;
        __set_PRIMASK(primask);
        return 0U;
    }

    /* Guardar el mensaje completo */
    memcpy(uartTxQueue[uartTxHead], message, length + 1U);

    uartTxHead =
        (uint8_t)((uartTxHead + 1U) % UART_TX_QUEUE_SIZE);

    uartTxCount++;

    /* Restaurar el estado anterior de las interrupciones */
    __set_PRIMASK(primask);

    return 1U;
}

static void UART_TxStartNext(void)
{
    uint8_t tail;

    /* Guardar el estado actual de las interrupciones */
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    /* No iniciar si UART esta ocupada o la cola esta vacia */
    if (uartTxBusy != 0U || uartTxCount == 0U)
    {
        __set_PRIMASK(primask);
        return;
    }

    /* Reservar el mensaje que vamos a transmitir */
    tail = uartTxTail;
    uartTxBusy = 1U;

    /* Restaurar las interrupciones antes de iniciar UART */
    __set_PRIMASK(primask);

    /* Iniciar transmision sin bloquear el programa */
    if (HAL_UART_Transmit_IT(
            &huart2,
            (uint8_t *)uartTxQueue[tail],
            (uint16_t)strlen(uartTxQueue[tail])) != HAL_OK)
    {
        /* Liberar el estado ocupado si no pudo iniciar */
        primask = __get_PRIMASK();
        __disable_irq();

        uartTxBusy = 0U;

        __set_PRIMASK(primask);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        if ((uartTxBusy != 0U) && (uartTxCount > 0U))
        {
            /* Retirar el mensaje que termino de transmitirse */
            uartTxTail =
                (uint8_t)((uartTxTail + 1U) % UART_TX_QUEUE_SIZE);

            uartTxCount--;
        }

        uartTxBusy = 0U;
    }
}

static void UART_SendResponse(const char *message)
{
    /* Guardar el mensaje para transmitirlo sin bloquear */
    (void)UART_TxEnqueue(message);
}

static void UART_SendStatus(void)
{
    char statusLine[96];

    uint32_t now = g_msTicks;
    uint16_t adc = g_adcValue;
    uint8_t pct = g_adcPct;
    uint8_t threshold = g_alarmThreshold;
    uint8_t mode = g_systemMode;
    uint8_t alarm = g_alarmActive;

    int length = snprintf(
        statusLine,
        sizeof(statusLine),
        "t=%lu;ADC=%u;PCT=%u;THR=%u;MODE=%s;ALARM=%u\r\n",
        (unsigned long)now,
        (unsigned int)adc,
        (unsigned int)pct,
        (unsigned int)threshold,
        (mode == MODE_RUN) ? "RUN" : "PAUSE",
        (unsigned int)alarm
    );

    if (length > 0 && length < (int)sizeof(statusLine))
    {
        UART_SendResponse(statusLine);
    }
}

static uint8_t UART_ParseThreshold(const char *command, uint8_t *threshold)
{
    uint32_t value = 0U;
    uint32_t index = 4U;

    /* El comando debe comenzar exactamente con "THR " */
    if (strncmp(command, "THR ", 4U) != 0)
    {
        return 0U;
    }

    /* Debe existir al menos un digito */
    if (command[index] == '\0')
    {
        return 0U;
    }

    /* Solo se permiten digitos decimales */
    while (command[index] != '\0')
    {
        char character = command[index];

        if (character < '0' || character > '9')
        {
            return 0U;
        }

        value = (value * 10U) + (uint32_t)(character - '0');

        /* Rechazar valores fuera de rango */
        if (value > 95U)
        {
            return 0U;
        }

        index++;
    }

    if (value < 10U)
    {
        return 0U;
    }

    *threshold = (uint8_t)value;

    return 1U;
}

static void UART_QueuePush(uint8_t type, const char *text)
{
    uint8_t head = uartQueueHead;

    /* No sobrescribir eventos pendientes */
    if (uartQueueCount >= UART_QUEUE_SIZE)
    {
        uartRxOverflowCount++;
        return;
    }

    uartQueue[head].type = type;

    if ((type == UART_EVENT_COMMAND) && (text != NULL))
    {
        size_t length = strlen(text);

        if (length > UART_MAX_LINE_LENGTH)
        {
            length = UART_MAX_LINE_LENGTH;
        }

        memcpy(uartQueue[head].text, text, length);
        uartQueue[head].text[length] = '\0';
    }
    else
    {
        uartQueue[head].text[0] = '\0';
    }

    uartQueueHead = (uint8_t)((head + 1U) % UART_QUEUE_SIZE);
    uartQueueCount++;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        char receivedChar = (char)uartRxByte;

        if (receivedChar == '\n')
        {
            if (uartDiscardLine != 0U)
            {
                /* Linea demasiado larga: generar un solo ERR */
                UART_QueuePush(UART_EVENT_ERROR, NULL);

                uartRxOverflowCount++;
                uartRxIndex = 0U;
                uartDiscardLine = 0U;
            }
            else if (uartRxIndex > 0U)
            {
                /* Linea completa: guardar el comando en la FIFO */
                uartRxBuffer[uartRxIndex] = '\0';

                UART_QueuePush(
                    UART_EVENT_COMMAND,
                    uartRxBuffer
                );

                uartLinesReceived++;
                uartRxIndex = 0U;
            }
            /* Las lineas vacias se ignoran */
        }
        else if (receivedChar == '\r')
        {
            /* Ignorar CR y esperar LF */
        }
        else if (uartDiscardLine == 0U)
        {
            if (uartRxIndex < UART_MAX_LINE_LENGTH)
            {
                uartRxBuffer[uartRxIndex] = receivedChar;
                uartRxIndex++;
            }
            else
            {
                /* Descartar hasta recibir LF */
                uartRxIndex = 0U;
                uartDiscardLine = 1U;
            }
        }

        /* Reactivar la recepcion del siguiente byte */
        (void)HAL_UART_Receive_IT(
            &huart2,
            &uartRxByte,
            1U
        );
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
