#ifndef __DEBUGUART_H
#define __DEBUGUART_H

#ifdef __cplusplus
 extern "C" {
#endif 
   
   #include "stm32f4xx_hal.h"
   
   void DebugUart_init(void);
   void DebugUart_TX(char *buffer);
   
#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
   