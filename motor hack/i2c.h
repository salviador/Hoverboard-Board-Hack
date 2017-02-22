#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
 extern "C" {
#endif 
   #include "stm32f1xx_hal.h"
   
    struct WII_JOYdati{
      volatile __IO uint8_t ax;
      volatile __IO uint8_t ay;
      volatile __IO uint8_t bc;
      volatile __IO uint8_t bz;
      volatile __IO uint8_t done;

      //Private
      volatile __IO uint8_t ai2cBuffer[7];
      volatile __IO uint8_t I2CTxDone;
      volatile __IO uint8_t I2CRxDone;
      volatile __IO uint8_t I2CERROR;
      volatile __IO uint8_t stato;
      volatile __IO uint32_t mS;
   };
   
   void MX_I2C2_Init(void);
   extern void Error_Handler(void);
   void WiiNunchuck_INIT(void);
   void WiiNunchuck_TASK(void);
  uint8_t nunchuk_decode_byte (uint8_t x);


   void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
