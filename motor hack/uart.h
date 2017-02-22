#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
 extern "C" {
#endif 
   
   
   
   void MX_USART2_UART_Init(void);
   extern void Error_Handler(void);
   
void Console_Log(char *message);
#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
