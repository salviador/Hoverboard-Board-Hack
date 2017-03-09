#include "i2c.h"
#include "stm32f1xx_hal.h"

I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c2_rx;
DMA_HandleTypeDef hdma_i2c2_tx;

volatile __IO struct WII_JOYdati wii_JOYdati;

/* I2C2 init function */
void MX_I2C2_Init(void)
{
  __HAL_RCC_I2C2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();
  
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1, 4);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 1, 3);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  wii_JOYdati.I2CTxDone = 0;
  wii_JOYdati.I2CRxDone = 0;
  wii_JOYdati.I2CERROR = 0;
  wii_JOYdati.stato = 0;
  
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn); 
  
  WiiNunchuck_INIT();
}



void WiiNunchuck_INIT(void){
    uint8_t ai2cBuffer[2];
    
    //-- START -- init WiiNunchuck
    ai2cBuffer[0] = 0xF0;
    ai2cBuffer[1] = 0x55;
    //Originale
    ai2cBuffer[0] = 0x40;
    ai2cBuffer[1] = 0x00;
    HAL_I2C_Master_Transmit_DMA(&hi2c2, 0xA4, (uint8_t*)ai2cBuffer, 2);
    while(wii_JOYdati.I2CTxDone ==0);
    wii_JOYdati.I2CTxDone = 0;
    HAL_Delay(10);
    wii_JOYdati.done = 0;
}


void WiiNunchuck_TASK(void){
  uint8_t ai2cBuffer[7];
  uint8_t temp;
  
  switch(wii_JOYdati.stato){
    case 0:
      wii_JOYdati.I2CTxDone = 0;
      ai2cBuffer[0] = 0x00;
      HAL_I2C_Master_Transmit_DMA(&hi2c2, 0xA4, (uint8_t*)ai2cBuffer, 1);
      wii_JOYdati.stato = 12;
      wii_JOYdati.mS = HAL_GetTick();
    break;
  
    case 1:
      if((HAL_GetTick() - wii_JOYdati.mS) >= 7){             
        wii_JOYdati.I2CTxDone = 0;
        wii_JOYdati.I2CRxDone = 0;
        wii_JOYdati.I2CERROR = 0;
        wii_JOYdati.stato = 10;
      }
      if(wii_JOYdati.I2CTxDone){
        wii_JOYdati.stato = 2;
        wii_JOYdati.mS = HAL_GetTick();    
      }
      if(wii_JOYdati.I2CERROR){
        wii_JOYdati.I2CTxDone = 0;
        wii_JOYdati.I2CRxDone = 0;
        wii_JOYdati.I2CERROR = 0;
        wii_JOYdati.stato = 10;
        wii_JOYdati.ay = 127;
        wii_JOYdati.ax = 127;        
      }      
    break;
  
    case 2:
      if((HAL_GetTick() - wii_JOYdati.mS) > 1){             
        wii_JOYdati.stato = 3;      
      }
    break;
  
    case 3:
      wii_JOYdati.I2CTxDone = 0;
      wii_JOYdati.I2CRxDone = 0;
      wii_JOYdati.I2CERROR = 0;      
      HAL_I2C_Master_Receive_DMA(&hi2c2, 0xA5, (uint8_t*)wii_JOYdati.ai2cBuffer, 6);
      wii_JOYdati.stato = 4;     
      wii_JOYdati.mS = HAL_GetTick(); 
    break;
  
    case 4:
      if((HAL_GetTick() - wii_JOYdati.mS) > 50){             
        wii_JOYdati.I2CTxDone = 0;
        wii_JOYdati.I2CRxDone = 0;
        wii_JOYdati.I2CERROR = 0;
        wii_JOYdati.stato = 10;
        wii_JOYdati.ay = 127;
        wii_JOYdati.ax = 127;        
      }
      if(wii_JOYdati.I2CERROR){
        wii_JOYdati.I2CTxDone = 0;
        wii_JOYdati.I2CRxDone = 0;
        wii_JOYdati.I2CERROR = 0;
        wii_JOYdati.stato = 10;
        wii_JOYdati.ay = 127;
        wii_JOYdati.ax = 127;
      }          
      if(wii_JOYdati.I2CRxDone){
        HAL_NVIC_DisableIRQ(DMA1_Channel5_IRQn);
        wii_JOYdati.I2CRxDone = 0;
        //Copia dati
        wii_JOYdati.ay = nunchuk_decode_byte(wii_JOYdati.ai2cBuffer[1]);
        wii_JOYdati.ax = nunchuk_decode_byte(wii_JOYdati.ai2cBuffer[0]);
        temp = nunchuk_decode_byte(wii_JOYdati.ai2cBuffer[5]);
        
        wii_JOYdati.bc = temp & 0x02;
        wii_JOYdati.bc = wii_JOYdati.bc >> 1;
        wii_JOYdati.bc = wii_JOYdati.bc & 0x01;
        wii_JOYdati.bz = temp & 0x01;
        wii_JOYdati.done = 1;        

        HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
        wii_JOYdati.I2CTxDone = 0;
        wii_JOYdati.I2CRxDone = 0;
        wii_JOYdati.I2CERROR = 0;
        
        wii_JOYdati.mS = HAL_GetTick();
        wii_JOYdati.stato = 11;
      }
    break;
  
    case 10:
      wii_JOYdati.I2CTxDone = 0;
      wii_JOYdati.I2CRxDone = 0;
      wii_JOYdati.I2CERROR = 0;
      wii_JOYdati.stato = 11;
      wii_JOYdati.mS = HAL_GetTick(); 
    break;
  
    case 11:
      if((HAL_GetTick() - wii_JOYdati.mS) >= 1){             
        wii_JOYdati.I2CTxDone = 0;
        wii_JOYdati.I2CRxDone = 0;
        wii_JOYdati.I2CERROR = 0;
        wii_JOYdati.stato = 0;
      }
    break;
    
    case 12:
      if(HAL_I2C_GetState(&hi2c2) == HAL_I2C_STATE_READY){
        wii_JOYdati.stato = 1;
        wii_JOYdati.mS = HAL_GetTick();        
      }            
      if((HAL_GetTick() - wii_JOYdati.mS) >= 7){             
        wii_JOYdati.I2CTxDone = 0;
        wii_JOYdati.I2CRxDone = 0;
        wii_JOYdati.I2CERROR = 0;
        wii_JOYdati.stato = 11;
        wii_JOYdati.mS = HAL_GetTick();        
      }      
    break;
  
  }
}

  
  
  
  
  
  
uint8_t nunchuk_decode_byte (uint8_t x)
{
    x = (x ^ 0x17) + 0x17;
    return x;
}

  
  
  
    
//HAL_I2C_Master_Transmit_DMA
//HAL_I2C_Master_Receive_DMA

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->Instance == I2C2){
    wii_JOYdati.I2CTxDone = 1;
  }
}
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c){
  if(hi2c->Instance == I2C2){
    wii_JOYdati.I2CRxDone = 1;
  }
}
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->Instance == I2C2){
    wii_JOYdati.I2CERROR = 1;
  }
}