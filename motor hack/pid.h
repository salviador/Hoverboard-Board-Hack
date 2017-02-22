#ifndef __PID__H
#define __PID__H

#ifdef __cplusplus
 extern "C" {
#endif 
   
   #include "stm32f1xx_hal.h"
   
   struct PID_dati{
    volatile __IO uint32_t mS;    
    volatile __IO int32_t duty;
   };
   
   struct pid_f_t{
    float max /*! Max manipulated value */;
    float min /*! Miniumum manipulated value */;
    float e /*! Error value */;
    float i /*! Integrator value */;
    float kp /*! Proportional constant */;
    float ki /*! Integrator constant */;
    float kd /*! Differential constant */;
  };
  
   struct pid_I32_t{
    int32_t max /*! Max manipulated value */;
    int32_t min /*! Miniumum manipulated value */;
    int32_t e /*! Error value */;
    int32_t i /*! Integrator value */;
    float kp /*! Proportional constant */;
    float ki /*! Integrator constant */;
    float kd /*! Differential constant */;
  };
  
void PID_init(uint16_t min, uint16_t max);
void PID_set_L_costant(float P,float I,float D);
void PID_L_task(int32_t set_speed, int32_t speed);
void PID_set_R_costant(float P,float I,float D);
void PID_R_task(int32_t set_speed, int32_t speed);


float pid_update_f(float sp, float pv, struct pid_f_t * ptr);
int32_t pid_update_I32(int32_t sp, int32_t pv, struct pid_I32_t * ptr);
         
   extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
      