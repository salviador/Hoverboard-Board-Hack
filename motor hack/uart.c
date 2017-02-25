#include "uart.h"
#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "string.h"


UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

volatile __IO struct UART_dati uartDati;


/* USART2 init function */
void MX_USART2_UART_Init(void)
{
  __HAL_RCC_USART2_CLK_ENABLE();
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 6);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 5, 7);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
 // huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  
  uartDati.UartRxReady = 0;
  uartDati.UarttxReady = 1;
}

void Console_Log(char *message)
{
    while(huart2.State != HAL_UART_STATE_READY);
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *)message, strlen(message));
//    HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message),200);
}

void Uart2_TX(char *message)
{
    uartDati.UarttxReady = 0;    //occupato
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *)message, strlen(message));
}
uint8_t Uart2_IS_TX_free(void){
  return (uartDati.UarttxReady); //(huart2.State == HAL_UART_STATE_READY);
}

uint8_t Uart2_IS_RX_available(void){
  return (uartDati.UartRxReady); //(huart2.State == HAL_UART_STATE_READY);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
     if (huart->Instance == USART2) {
        uartDati.UartRxReady = 1;
     }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
     if (huart->Instance == USART2) {
       uartDati.UarttxReady = 1;
     }
}