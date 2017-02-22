#ifndef __VARIE__H
#define __VARIE__H

#ifdef __cplusplus
 extern "C" {
#endif 
   
   #include "stm32f1xx_hal.h"
   
   
  #define BUZZER_Pin GPIO_PIN_4
  #define BUZZER_GPIO_Port GPIOA
  #define LED_Pin GPIO_PIN_2
  #define LED_GPIO_Port GPIOB
  #define IS_CHARGE_Pin GPIO_PIN_12
  #define IS_CHARGE_GPIO_Port GPIOA
   
   void Buzzer_init(void);
   void Buzzer_Set(uint8_t stato);
   void Buzzer_OneBeep(void);
   void Buzzer_TwoBeep(void);
   void Buzzer_OneShortBeep(void);
   void Buzzer_OneLongBeep(void);

   void Led_init(void);
   void Led_Set(uint8_t stato);
   
   void IS_Charge_init(void);
   uint8_t IS_Charge(void);
   
   void DebugPin_init(void);
   void DebugPin4_ON(void);
   void DebugPin4_OFF(void);
   void DebugPin5_ON(void);
   void DebugPin5_OFF(void);  
   extern void Error_Handler(void);
   

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
   