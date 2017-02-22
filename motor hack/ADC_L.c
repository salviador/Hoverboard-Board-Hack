#include "ADC_L.h"


ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc3;

volatile __IO struct ADC_Ldati adc_Ldati;

// ----------------------PUBLIC----------------------
void ADC_L_init(void){
 ADC_ChannelConfTypeDef sConfig;
 
  __HAL_RCC_DMA2_CLK_ENABLE();
  /* DMA2_Channel4_5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel4_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);
  
  hadc3.Instance = ADC3;
  hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
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
  sConfig.Channel = ADC_CHANNEL_10;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.Rank = 1;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_ADC_Start(&hadc3);
  HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_Ldati.data, 1);
}

// ------------NORMALIZATE----------------
float GET_MOTOR_L_AMP(void){  
  HAL_NVIC_DisableIRQ(DMA2_Channel4_5_IRQn);  
  uint16_t data = adc_Ldati.data[0];
  HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);
  if(data<ADC_MOTOR_L_CENTER){
    return 0.0;
  }
  return (data - ADC_MOTOR_L_CENTER) * MOTOR_L_AMP_CONV_AMP;
}

// ------------RAW----------------
void ADC_L_ResetFlag(void){
  HAL_NVIC_DisableIRQ(DMA2_Channel4_5_IRQn);  
  adc_Ldati.convflag = 0;
  HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);  
}
uint8_t ADC_MOTOR_LEFT_IS_CONV(void){
  uint8_t data=0;
  HAL_NVIC_DisableIRQ(DMA2_Channel4_5_IRQn);  
  data = adc_Ldati.convflag;
  HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);  
  return data;    
}
uint16_t ADC_MOTOR_LEFT(void){
  uint16_t data=0;
  HAL_NVIC_DisableIRQ(DMA2_Channel4_5_IRQn);  
  data = adc_Ldati.data[0];
  HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);  
  return data;    
}

// ----------------------PRIVATE----------------------
void ADC_L_callback(void){
  adc_Ldati.convflag = 1;
}
