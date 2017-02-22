#ifndef __MOTOR_L__H
#define __MOTOR_L__H

#ifdef __cplusplus
 extern "C" {
#endif 
   
   #include "stm32f1xx_hal.h"


  #define HALL_LEFT_A_Pin GPIO_PIN_5
  #define HALL_LEFT_A_GPIO_Port GPIOB
  #define HALL_LEFT_B_Pin GPIO_PIN_6
  #define HALL_LEFT_B_GPIO_Port GPIOB
  #define HALL_LEFT_C_Pin GPIO_PIN_7
  #define HALL_LEFT_C_GPIO_Port GPIOB

   struct MOTOR_Ldati{
    volatile __IO uint32_t uwPeriodValue;
    volatile __IO uint8_t BLDCMotorL_count;
    volatile __IO uint8_t BLDCMotorL_flag; 
    volatile __IO uint16_t BLDCMotorL_velRAW;
    volatile __IO uint16_t BLDCMotorL_deltavel;
    volatile __IO uint8_t reverse;
    volatile __IO uint16_t newhallpos;
    volatile __IO uint8_t stop;      
   };
   
    // PUBLIC  
    void MotorL_init(void);
    void MotorL_start(void);
    void MotorL_stop(void);
    void MotorL_pwm(int16_t value_percent);

    // PRIVATE
    void MX_TIM8_Init(void);
    void mL_AHigh__ON(void);
    void mL_AHigh__OFF(void);
    void mL_BHigh__ON(void);
    void mL_BHigh__OFF(void);
    void mL_CHigh__ON(void);
    void mL_CHigh__OFF(void);
    void mL_PWM_Set_Ch1(uint16_t value);
    void mL_PWM_Set_Ch2(uint16_t value);
    void mL_PWM_Set_Ch3(uint16_t value);
    void mL_PWM_Set_ChALL(uint16_t value);
    void mL_BLDCMotor(void);
    void mL_Low_CH1_ON(void);  
    void mL_Low_CH1_OFF(void);
    void mL_Low_CH2_ON(void);  
    void mL_Low_CH2_OFF(void);
    void mL_Low_CH3_ON(void);  
    void mL_Low_CH3_OFF(void); 
    void mL_HallSensor_init(void);

    void PWM_L_ISR_Callback(void);
    void HALL_L_ISR_Callback(void);
  
    extern void Error_Handler(void);
   

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
