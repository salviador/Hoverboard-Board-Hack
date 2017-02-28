#ifndef __TELEMETRY_H
#define __TELEMETRY_H

#ifdef __cplusplus
 extern "C" {
#endif 
   
   #include "stm32f1xx_hal.h"
   
    enum TELEMETRY_TX_STATO {telemetry_wait, telemetry_send};
     
    struct TELEMETRY_dati{
      //TX
      volatile __IO uint8_t statoTX;
      volatile __IO uint32_t time_send; 
      //RX
      volatile __IO uint8_t statoRX;      
      volatile __IO uint8_t counterBufferRX;      
      volatile __IO uint8_t TelemetryRX_TramaX[5];
      volatile __IO uint8_t TelemetryRX_TramaY[5];
      volatile __IO uint8_t counterBufferRXTrama;
      
      //Public
       volatile __IO uint8_t dataREADY_JOYSTICK;
       volatile __IO uint8_t dataLast_Command;
       volatile __IO int16_t joyx;
       volatile __IO int16_t joyy;
    };
   
    
   void Telemetry_init(void);
   void Telemetry_TASK(void);
   void Telemetry_RX_parseData(uint8_t p_start_readBuffer, uint16_t size_readBuff);
   extern void Error_Handler(void);
   
#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
