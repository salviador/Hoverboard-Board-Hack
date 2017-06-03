#ifndef __TELEMETRY_H
#define __TELEMETRY_H

#ifdef __cplusplus
 extern "C" {
#endif 
   
    #include "cube_hal.h"
   
    enum TELEMETRY_TX_STATO {telemetry_wait, telemetry_send};
     
    struct TELEMETRY_dati{
      //TX
      volatile __IO uint8_t statoTX;
      volatile __IO uint32_t time_send; 
      //RX
      volatile __IO uint8_t statoRX;      
      volatile __IO uint8_t counterBufferRX;      
      volatile __IO uint8_t TelemetryRX_TramaBATTERIA[20];
      volatile __IO uint8_t TelemetryRX_TramaMOTORL[20];
      volatile __IO uint8_t TelemetryRX_TramaMOTORR[20];
      volatile __IO uint8_t counterBufferRXTrama;
      
      //Public
       volatile __IO uint8_t dataREADY_JOYSTICK;
       volatile __IO uint8_t dataLast_Command;
       volatile __IO int16_t joyx;
       volatile __IO int16_t joyy;
    };

         
    struct TELEMETRY_JOYSTICK_dati{
      //Public
       volatile __IO uint8_t dataREADY_JOYSTICK;
       volatile __IO int joyx;
       volatile __IO int joyy;
    };
    
    struct UART_dati{
      volatile __IO uint8_t bUartRx[101];
      volatile __IO uint8_t bUartTx[20];
      volatile __IO uint8_t UartRxReady;
      volatile __IO uint8_t UarttxReady;
    };
      
   
   extern void Error_Handler(void);

void MX_USART1_UART_Init(void);
void Uart1_TX(char *message);
uint8_t Uart1_IS_TX_free(void);
uint8_t Uart1_IS_RX_available(void);


   

    
   void Telemetry_init(void);
   void Telemetry_TASK(uint8_t connessione_stato);
   void Telemetry_RX_parseData(uint8_t p_start_readBuffer, uint16_t size_readBuff);
   extern void Error_Handler(void);
   
#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
