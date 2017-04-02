#include "debugUART.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

UART_HandleTypeDef huart2;

volatile __IO char debugBufferTX[200];


void DebugUart_init(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_USART2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /**USART2 GPIO Configuration    
  PA2     ------> USART2_TX
  PA3     ------> USART2_RX 
  */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);


  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);
  
  //HAL_UART_Receive_IT();
  
/*
    int ic=0;
  while(1){
    DebugUart_TX("ciao\r\n");
    PRINTF("Hello %d\r\n",ic);
    ic++;
    HAL_Delay(1000);
  }
  
  */
  
}

void DebugUart_TX(char *buffer){
  HAL_UART_Transmit(&huart2, (uint8_t *)&buffer[0], strlen((char *)&buffer[0]), 100);
}
  
 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    if(UartHandle->Instance==USART2){
    }
}