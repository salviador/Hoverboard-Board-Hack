#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif 
   
   #include "stm32f4xx_hal.h"
   
   void JoyStick_notify_Callback(int x, int y);
   
#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
   