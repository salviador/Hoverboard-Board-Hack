#ifndef __ADC_L__H
#define __ADC_L__H

#ifdef __cplusplus
 extern "C" {
#endif 

 
   #include "stm32f1xx_hal.h"

   #define ADC_MOTOR_L_CENTER   1887
   #define MOTOR_L_AMP_CONV_AMP 0.02068965517241379310344827586207

   struct ADC_Ldati{
    volatile __IO uint16_t data[3];
    volatile __IO uint8_t convflag;   
   };
   
// ----------------------PUBLIC----------------------      
   void ADC_L_init(void);
// ------------NORMALIZATE----------------   
   float GET_MOTOR_L_AMP(void);
// ------------RAW----------------   
   uint16_t  ADC_MOTOR_LEFT(void);   
   uint8_t ADC_MOTOR_LEFT_IS_CONV(void);   
   void ADC_L_ResetFlag(void);
// ----------------------PRIVATE----------------------
   void ADC_L_callback(void); 
   extern void Error_Handler(void); 
   
#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
   