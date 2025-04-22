/* Includes */
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Buffer Configuration */
#define MAX_BUFFER_SIZE 1024
uint8_t buffer[MAX_BUFFER_SIZE];
uint16_t buffer_len = 0;
SemaphoreHandle_t xBufferMutex;
TaskHandle_t SensorTaskHandle;

/* Private Variables */
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;

/* Function Prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
void SensorTask(void *argument);
void PrintTask(void *argument);

/* Redirect printf to USART2 */
int __io_putchar(int ch) {
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();

  /* Start timer interrupt */
  HAL_TIM_Base_Start_IT(&htim2);

  /* Create Mutex */
  xBufferMutex = xSemaphoreCreateMutex();

  /* Create Tasks */
  xTaskCreate(SensorTask, "SensorTask", 128, NULL, osPriorityNormal, &SensorTaskHandle);
  xTaskCreate(PrintTask, "PrintTask", 256, NULL, osPriorityLow, NULL);

  /* Start Scheduler */
  vTaskStartScheduler();

  while (1) {}
}

/* Sensor Task */
void SensorTask(void *argument) {
  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    uint8_t num_bytes = rand() % 6;
    uint8_t data[5];
    for (int i = 0; i < num_bytes; i++) {
      data[i] = rand() % 256;
    }

    if (xSemaphoreTake(xBufferMutex, portMAX_DELAY)) {
      if (buffer_len + num_bytes < MAX_BUFFER_SIZE) {
        memcpy(&buffer[buffer_len], data, num_bytes);
        buffer_len += num_bytes;
      }
      xSemaphoreGive(xBufferMutex);
    }
  }
}

/* Print Task */
void PrintTask(void *argument) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(10000));

    if (xSemaphoreTake(xBufferMutex, portMAX_DELAY)) {
      if (buffer_len >= 50) {
        printf("Latest 50 bytes (hex): ");
        for (int i = buffer_len - 50; i < buffer_len; i++) {
          printf("%02X ", buffer[i]);
        }
        printf("\n");
        buffer_len -= 50;
      }
      xSemaphoreGive(xBufferMutex);
    }
  }
}

/* Timer Callback */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(SensorTaskHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

/* TIM2 init function */
static void MX_TIM2_Init(void) {
  __HAL_RCC_TIM2_CLK_ENABLE();
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim2);
  HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

/* USART2 init function */
static void MX_USART2_UART_Init(void) {
  __HAL_RCC_USART2_CLK_ENABLE();
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);
}

/* GPIO init function */
static void MX_GPIO_Init(void) {
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* System Clock Configuration */
void SystemClock_Config(void) {
  // Clock configuration stub - configure as needed
}
