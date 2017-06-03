#include "debugUART.h"
#include "stm32l0xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

UART_HandleTypeDef huart4;

volatile __IO char debugBufferTX[200];


void DebugUart_init(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_USART4_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /**USART2 GPIO Configuration    
  PA0     ------> USART2_TX
  PA1     ------> USART2_RX 
  */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_USART4;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);


  huart4.Instance = USART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;  
  HAL_UART_Init(&huart4);
  
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
  HAL_UART_Transmit(&huart4, (uint8_t *)&buffer[0], strlen((char *)&buffer[0]), 100);
}
  