#ifndef __APPLICATION_H
#define __APPLICATION_H

#ifdef __cplusplus
 extern "C" {
#endif 
   #include "stm32f1xx_hal.h"

   #define ACCELLERATION_CONSTANT   0.1;  //1;  //30; //0.1;
   
   enum APP_STATO {app_init, app_soft_run, app_soft_run_nohand};
   
    struct APPLICATION_dati{
      volatile __IO uint8_t stato;
      volatile __IO uint32_t tsoft_run;
      volatile __IO float ayn;
      volatile __IO float axn; 
      volatile __IO uint32_t tcruise_decrement;
      volatile __IO float cruise_soft_run_nohand;      
      volatile __IO float center_media_X;
      volatile __IO float center_media_Y;
      volatile __IO float faccY;
      volatile __IO float faccX;     
      volatile __IO int16_t motATS;
      volatile __IO int16_t motBTS;          
   };
   
   struct BATTERY_dati{
      volatile __IO float VBatt;
     
      volatile __IO uint32_t time_batt;
      volatile __IO uint32_t somma_batt;
      volatile __IO uint8_t counter_media;
      
      volatile __IO uint32_t time_lowvoltage;      
   };
   
  void applcation_TASK(void);
  void applcation_init(void);
  void Battery_TASK(void);
  float GET_BatteryAverage(void);
  void TASK_BATTERY_LOW_VOLTAGE(void);
  void WAIT_CHARGE_FINISH(void);
  
  float normalize_y(uint8_t y);
  float normalize_x(uint8_t x);
  float get_powerMax(float Vbattery);
  float scale_y(float y, float Vbatt);
  float scale_x(float x, float Vbatt);
  float accellerationY(float value);
  float accellerationX(float value);
  void go_motor(float throttle, float steering, float Vbatt);



  extern void Error_Handler(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
   