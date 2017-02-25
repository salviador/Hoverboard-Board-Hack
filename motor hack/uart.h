#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
 extern "C" {
#endif 
   
   #include "stm32f1xx_hal.h"
   
    struct UART_dati{
      volatile __IO uint8_t bUartRx[101];
      volatile __IO uint8_t bUartTx[20];
      volatile __IO uint8_t UartRxReady;
      volatile __IO uint8_t UarttxReady;
    };
      
   
   void MX_USART2_UART_Init(void);
   extern void Error_Handler(void);
   
void Console_Log(char *message);
void Uart2_TX(char *message);
uint8_t Uart2_IS_TX_free(void);
uint8_t Uart2_IS_RX_available(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
