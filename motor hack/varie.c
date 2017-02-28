#include "varie.h"
#include "stm32f1xx_hal.h"
#include "delay.h"

extern IWDG_HandleTypeDef hiwdg;

void Buzzer_init(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);
  Buzzer_Set(0);
  Buzzer_Set(1);
  Buzzer_Set(0); 
}

void Led_init(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);  
  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
}

void IS_Charge_init(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /*Configure GPIO pin : IS_CHARGE_Pin */
  GPIO_InitStruct.Pin = IS_CHARGE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IS_CHARGE_GPIO_Port, &GPIO_InitStruct);
}

void Led_Set(uint8_t stato){
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, (GPIO_PinState)stato);
}
void Buzzer_Set(uint8_t stato){
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, (GPIO_PinState)stato);
}
void Buzzer_OneBeep(void){
  for(int i=0;i<200;i++){
    Buzzer_Set(1);
    Delay_us(160);
    Buzzer_Set(0);
    Delay_us(160);
    HAL_IWDG_Refresh(&hiwdg);   //819mS    
  }
  Buzzer_Set(0); 
}
void Buzzer_TwoBeep(void){
  for(int j=0;j<2;j++){  
    for(int i=0;i<150;i++){
      Buzzer_Set(1);
      Delay_us(160);     
      Buzzer_Set(0);
      Delay_us(160);      
      HAL_IWDG_Refresh(&hiwdg);   //819mS     
    }
    Buzzer_Set(0);
    HAL_Delay(200);
  }
  Buzzer_Set(0);
}
void Buzzer_OneShortBeep(void){
  for(int i=0;i<80;i++){
    Buzzer_Set(1);
    Delay_us(160);
    Buzzer_Set(0);
    Delay_us(160);
    HAL_IWDG_Refresh(&hiwdg);   //819mS    
  }
  Buzzer_Set(0); 
}
void Buzzer_OneLongBeep(void){
  for(int i=0;i<600;i++){
    Buzzer_Set(1);
    Delay_us(160);
    Buzzer_Set(0);
    Delay_us(160);
    HAL_IWDG_Refresh(&hiwdg);   //819mS
  }
  Buzzer_Set(0); 
}

uint8_t IS_Charge(void){
  return (uint8_t)HAL_GPIO_ReadPin(IS_CHARGE_GPIO_Port, IS_CHARGE_Pin);
}
//volatile uint32_t tu32;
void DebugPin_init(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOC_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);  
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);  
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//  tu32 = (uint32_t)GPIO_PIN_4 << 16;
//  tu32 = (uint32_t)GPIO_PIN_5 << 16;
  
}
void DebugPin4_ON(void){
  GPIOC->BSRR = GPIO_PIN_4;
}
void DebugPin4_OFF(void){ 
  GPIOC->BSRR = 0x00100000;
}
void DebugPin5_ON(void){
  GPIOC->BSRR = GPIO_PIN_5;
}
void DebugPin5_OFF(void){ 
  GPIOC->BSRR = 0x00200000;
}
