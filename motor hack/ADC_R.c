#include "ADC_R.h"


ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;


 volatile __IO uint16_t buffer[3];
 
volatile __IO struct ADC_Rdati adc_Rdati;

// ----------------------PUBLIC----------------------
void ADC_R_init(void){

  ADC_ChannelConfTypeDef sConfig;
  __HAL_RCC_DMA1_CLK_ENABLE();
  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 2);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /*
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
*/
    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }  
  
  HAL_ADC_Start(&hadc1);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_Rdati.data, 2);
  
}



// ------------NORMALIZATE----------------
float GET_BATTERY_VOLT(void){
  HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);  
  uint16_t data = adc_Rdati.data[0];
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);  
  return (float)data * ADC_BATTERY_VOLT;
}
float GET_MOTOR_R_AMP(void){  
  HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);  
  uint16_t data = adc_Rdati.data[1];
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);    
  if(data<ADC_MOTOR_R_CENTER){
    return 0.0;
  }
  return (data - ADC_MOTOR_R_CENTER) * MOTOR_R_AMP_CONV_AMP;
}
// ------------RAW----------------
uint8_t ADC_MOTOR_RIGHT_IS_CONV(void){
  uint8_t data=0;
  HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);  
  data = adc_Rdati.convflag;
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);  
  return data;  
}

uint16_t ADC_MOTOR_RIGHT(void){
  uint16_t data=0;
  HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);  
  data = adc_Rdati.data[1];
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);  
  return data;  
}
uint16_t ADC_BATTERY(void){
  uint16_t data=0;
  HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);  
  data = adc_Rdati.data[0];
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);  
  return data;  
}


void ADC_R_ResetFlag(void){
  HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);  
  adc_Rdati.convflag = 0;
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);  
}

// ----------------------PRIVATE----------------------



void ADC_R_callback(void){
  adc_Rdati.convflag = 1;
}
