#ifndef __MOTOR_R__H
#define __MOTOR_R__H

#ifdef __cplusplus
 extern "C" {
#endif 
   
   #include "stm32f1xx_hal.h"


  #define HALL_RIGHT_A_Pin GPIO_PIN_10
  #define HALL_RIGHT_A_GPIO_Port GPIOC
  #define HALL_RIGHT_B_Pin GPIO_PIN_11
  #define HALL_RIGHT_B_GPIO_Port GPIOC
  #define HALL_RIGHT_C_Pin GPIO_PIN_12
  #define HALL_RIGHT_C_GPIO_Port GPIOC

   struct MOTOR_Rdati{
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
    void MotorR_init(void);
    void MotorR_start(void);
    void MotorR_stop(void);
    void MotorR_pwm(int16_t value_percent);

    // PRIVATE
    void MX_TIM1_Init(void);
    void mR_AHigh__ON(void);
    void mR_AHigh__OFF(void);
    void mR_BHigh__ON(void);
    void mR_BHigh__OFF(void);
    void mR_CHigh__ON(void);
    void mR_CHigh__OFF(void);


    void mR_PWM_Set_Ch1(uint16_t value);
    void mR_PWM_Set_Ch2(uint16_t value);
    void mR_PWM_Set_Ch3(uint16_t value);
    void mR_PWM_Set_ChALL(uint16_t value);

    void mR_Low_CH1_ON(void);  
    void mR_Low_CH1_OFF(void);
    void mR_Low_CH2_ON(void);  
    void mR_Low_CH2_OFF(void);
    void mR_Low_CH3_ON(void);  
    void mR_Low_CH3_OFF(void);    
    
    void mR_BLDCMotor(void);

    void mR_HallSensor_init(void);

    void PWM_R_ISR_Callback(void);
    void HALL_R_ISR_Callback(void);
  
    extern void Error_Handler(void);
   

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
