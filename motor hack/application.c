#include "application.h"
#include "motor_L.h"
#include "motor_R.h"
#include "i2c.h"
#include "ADC_L.h"
#include "ADC_R.h"
#include "varie.h"
#include "telemetry.h"
#include <stdlib.h>

extern IWDG_HandleTypeDef hiwdg;

volatile __IO struct APPLICATION_dati app;
volatile __IO struct BATTERY_dati battery_dati;
extern volatile __IO struct WII_JOYdati wii_JOYdati;
extern struct TELEMETRY_dati telemetry;

float GET_BatteryAverage(void){
  return battery_dati.VBatt;
}
void TASK_BATTERY_LOW_VOLTAGE(void){
  if((HAL_GetTick() - battery_dati.time_lowvoltage) > 600000){          //Ogni 10 min BEEP
    battery_dati.time_lowvoltage = HAL_GetTick();
  
    Buzzer_OneLongBeep();
  }
}
void WAIT_CHARGE_FINISH(void){
  uint32_t timerBatteryC; 

  Led_Set(1);
  MotorR_stop();
  MotorL_stop();   
  HAL_IWDG_Refresh(&hiwdg);   //819mS  
  Buzzer_OneLongBeep();
  HAL_IWDG_Refresh(&hiwdg);   //819mS  
  Buzzer_OneLongBeep();  
  timerBatteryC = HAL_GetTick(); 
  
  while(IS_Charge()==0){
    HAL_IWDG_Refresh(&hiwdg);   //819mS
    HAL_Delay(250);  
    Battery_TASK();
    if(GET_BatteryAverage() < 41.250){
      timerBatteryC = HAL_GetTick();       
    }
    if((HAL_GetTick() - timerBatteryC) > 20000){ //600000       x 20Sec, batteria sopra i 42Volt, avverti che è carica!!
      Buzzer_OneLongBeep();  
      HAL_IWDG_Refresh(&hiwdg);   //819mS
    }
  }
  Led_Set(0);
  applcation_init();
}

void applcation_init(void){
  app.stato = app_init;
  MotorR_stop();
  MotorL_stop();  
  battery_dati.time_batt = HAL_GetTick();
  battery_dati.time_lowvoltage = HAL_GetTick();
  battery_dati.somma_batt = 0;
  battery_dati.counter_media = 0;
  app.cruise_soft_run_nohand = 0.0;
  
  app.faccY=0.0;
  app.faccX=0.0;  
}

void applcation_TASK(void){
  uint8_t letture,temp8;
  float tempf1,tempf2;
  
  switch(app.stato){
    case app_init:
      MotorR_stop();     
      MotorL_stop();     
      //Aspetta Center joystick e calcola la media
      app.center_media_X = 0.0;
      app.center_media_Y = 0.0;
      letture = 0;
      wii_JOYdati.done = 0;
      do{
        HAL_IWDG_Refresh(&hiwdg);   //819mS
        
        while(wii_JOYdati.done==0){WiiNunchuck_TASK();}
        wii_JOYdati.done = 0;
        if((wii_JOYdati.ay < 129) && (wii_JOYdati.ay > 124)){
          letture++;
          temp8 = wii_JOYdati.ay;
          app.center_media_Y = app.center_media_Y + ((float)temp8);
        }else{
          letture = 0;
          app.center_media_Y = 0.0;
        }
      }while(letture < 50);
      app.center_media_Y = app.center_media_Y / 50.0;
      letture = 0;
      wii_JOYdati.done = 0;
      do{
        HAL_IWDG_Refresh(&hiwdg);   //819mS
        
        while(wii_JOYdati.done==0){WiiNunchuck_TASK();}
        wii_JOYdati.done = 0;
        if((wii_JOYdati.ax < 129) && (wii_JOYdati.ax > 124)){
          letture++;
          temp8 = wii_JOYdati.ax;
          app.center_media_X = app.center_media_X + ((float)temp8);
        }else{
          letture = 0;
          app.center_media_X = 0.0;
        }
      }while(letture < 50);
      app.center_media_X = app.center_media_X / 50.0;
            
      accelleration_XY_Reset();
      
      app.tsoft_run = HAL_GetTick();
      app.stato = app_soft_run;
      MotorR_start();
      MotorL_start(); 
      
      Buzzer_OneShortBeep();
      
    break;
    
    case app_soft_run:
     //INDOOR LIMIT 30; -30
      app.ayn = normalize_y(wii_JOYdati.ay);
      app.axn = normalize_x(wii_JOYdati.ax);

      //scala x soft run
      tempf1 = app.ayn; 
      tempf2 = battery_dati.VBatt;       
      app.ayn = scale_y(tempf1, tempf2);
      tempf1 = app.axn; 
      tempf2 = battery_dati.VBatt;       
      app.axn = scale_x(tempf1, tempf2);

      app.ayn = accellerationY(app.ayn);
      app.axn = accellerationX(app.axn);
      
      tempf1 = app.ayn;
      tempf2 = app.axn;
      
      go_motor(tempf1, tempf2, battery_dati.VBatt);

      //soft_run_nohand
      temp8 = wii_JOYdati.ay;
      if((wii_JOYdati.bz == 0)&&((((float)temp8 - app.center_media_Y) > 4.0))){
        if((HAL_GetTick() - app.tsoft_run)>=1000){
            app.cruise_soft_run_nohand = 0.0;
            app.stato = app_soft_run_nohand;
            app.tcruise_decrement = HAL_GetTick();
            Buzzer_OneShortBeep();
        }
      }else{
        app.tsoft_run = HAL_GetTick();
      }                        
      //Android APP ?
      if(telemetry.dataREADY_JOYSTICK){
        telemetry.dataREADY_JOYSTICK  = 0;
        if(telemetry.dataLast_Command == 'R'){
          app.stato = app_soft_ANDROIDAPP;
          app.tAndroidAPP = HAL_GetTick();        
          accelleration_XY_Reset();
        }
        telemetry.dataLast_Command = 0;
      }
    break;
    
    case app_soft_run_nohand:
      app.ayn = normalize_y(wii_JOYdati.ay);
      app.axn = normalize_x(wii_JOYdati.ax);

      //scala x soft run
      tempf1 = app.ayn; 
      tempf2 = battery_dati.VBatt;       
      app.ayn = scale_y(tempf1, tempf2);
      tempf1 = app.axn; 
      tempf2 = battery_dati.VBatt;       
      app.axn = scale_x(tempf1, tempf2);

      app.ayn = accellerationY(app.ayn);
      app.axn = accellerationX(app.axn);

      //Attiva cruise control
      if(app.ayn > 5.0){
        tempf1 = app.cruise_soft_run_nohand;
        if((app.ayn - 5.0) > tempf1){
          app.cruise_soft_run_nohand = app.ayn - 5.0;
        }
      }
       //Decrementa Cruise
      if((app.ayn < -5.0)&&(app.ayn > -23.0)){
        //con time step
        if((HAL_GetTick() - app.tcruise_decrement)>250){
          tempf1 = app.ayn / 10.0;               
          app.cruise_soft_run_nohand = app.cruise_soft_run_nohand + tempf1;                //STEP DECREMENT
          if(app.cruise_soft_run_nohand < 0.0){
            app.cruise_soft_run_nohand = 0.0;
          }
          app.tcruise_decrement = HAL_GetTick();
        }
      }     
      //Stop Cruise
      if(app.ayn <= -23.0){
        go_motor(0, 0, battery_dati.VBatt);     
        MotorR_stop();     
        MotorL_stop();             
        accelleration_XY_Reset();        
        app.cruise_soft_run_nohand = 0.0;
        app.stato = app_init;
        break;
      }
      tempf1 = app.cruise_soft_run_nohand;
      tempf2 = app.axn;
      go_motor(tempf1, tempf2, battery_dati.VBatt);            
    break;
  
    case app_soft_ANDROIDAPP:
     //ANDROID APP
      if(telemetry.dataREADY_JOYSTICK){
        telemetry.dataREADY_JOYSTICK  = 0;
        app.tAndroidAPP = HAL_GetTick();        
        if(telemetry.dataLast_Command == 'F'){
          MotorR_stop();     
          MotorL_stop();     
          accelleration_XY_Reset();          
          app.cruise_soft_run_nohand = 0.0;
          app.stato = app_init;
          telemetry.dataLast_Command = 0;
          break;
        }
      }
      if((HAL_GetTick() - app.tAndroidAPP)>500){    //Se non ricevi risposta dalla APP esci dalla modalita ANDROID APP [BLYNK]
        go_motor(0, 0, battery_dati.VBatt); 
        MotorR_stop();     
        MotorL_stop();     
        app.cruise_soft_run_nohand = 0.0;
        app.stato = app_init;
        break;
      }


      app.ayn = (float)telemetry.joyy;
      app.axn = (float)telemetry.joyx;
      if(app.ayn > 300.0){
        app.ayn = 300.0;
      }
      if(app.axn > 300.0){
        app.axn = 300.0;
      }
      if(app.ayn < 0.0){
        app.ayn = 0.0;
      }
      if(app.axn < 0){
        app.axn = 0.0;
      }

      app.ayn = app.ayn - 75.0;
      app.axn = app.axn - 75.0;

      //scala x soft run
      tempf1 = app.ayn; 
      tempf2 = battery_dati.VBatt;       
      app.ayn = scale_y(tempf1, tempf2);
      tempf1 = app.axn; 
      tempf2 = battery_dati.VBatt;       
      app.axn = scale_x(tempf1, tempf2);

      app.ayn = accellerationY(app.ayn);
      app.axn = accellerationX(app.axn);
      tempf1 = app.ayn;
      tempf2 = app.axn;
      go_motor(tempf1, tempf2, battery_dati.VBatt);
    break;
    
    default:
        MotorR_stop();     
        MotorL_stop();     
        app.stato = app_init;
    break;
    
  }
  
}


















float normalize_y(uint8_t y){
  float ayn;
      ayn = 0.0;
      ayn = (float)((float)y - app.center_media_Y);
      //Limit
      if(ayn >= 0.0){
        //0 ~ 100
        if(ayn > 100.0){
          ayn = 100.0;
        }
      }else{
        //0 ~ -100
        if(ayn < -100.0){
          ayn = -100.0;
        }
      }
  return ayn;  
}

float normalize_x(uint8_t x){
  float axn;
      axn = 0.0;
      axn = (float)((float)x - app.center_media_X);
      //Limit
      
      if(axn >= 0.0){
        //0 ~ 100
        if(axn > 100.0){
          axn = 100.0;
        }
      }else{
        //0 ~ -100
        if(axn < -100.0){
          axn = -100.0;
        }
      }     
  return axn;
}

float scale_y(float y, float Vbatt){
  float ayn;
  ayn = 0.0;
      ayn = y * get_powerMax(Vbatt); //0.30000;
  return ayn;
}

float scale_x(float x, float Vbatt){
  float axn;
  axn = 0.0;
      axn = x * get_powerMax(Vbatt); //0.30000;
  return axn;
}
                                  //Vbatt,      Power max
const float Battery_power[][10] = {{40.0,       60.0/100.0},    //30% max potenza
                                  {38.0,        60.0/100.0},
                                  {37.0,        61.0/100.0},
                                  {36.0,        62.0/100.0},
                                  {35.0,        65.0/100.0},
                                  {34.0,        65.0/100.0},
                                  {33.0,        67.0/100.0},
                                  {32.0,        67.0/100.0},
                                  {31.0,        67.0/100.0},
                                  {30.0,        67.0/100.0}};
float get_powerMax(float Vbattery){
  if(Vbattery >= Battery_power[0][0]){
    return Battery_power[0][1];
  }else if(Vbattery >= Battery_power[1][0]){
    return Battery_power[1][1];
  }else if(Vbattery >= Battery_power[2][0]){
    return Battery_power[2][1];
  }else if(Vbattery >= Battery_power[3][0]){
    return Battery_power[3][1];
  }else if(Vbattery >= Battery_power[4][0]){
    return Battery_power[4][1];
  }else if(Vbattery >= Battery_power[5][0]){
    return Battery_power[5][1];
  }else if(Vbattery >= Battery_power[6][0]){
    return Battery_power[6][1];
  }else if(Vbattery >= Battery_power[7][0]){
    return Battery_power[7][1];
  }else if(Vbattery >= Battery_power[8][0]){
    return Battery_power[8][1];
  }else if(Vbattery >= Battery_power[9][0]){
    return Battery_power[9][1];
  }else{
    return Battery_power[9][1];
  }
}

void accelleration_XY_Reset(void){
  app.faccY = 0.0;
  app.faccX = 0.0;
}

float accellerationY(float value){
  /* value -> -1000 0 +1000 */
  float tval;
  
  tval = app.faccY;
  
  //EXPEIMENT
  if(value > tval){
    app.faccY = app.faccY + ACCELLERATION_CONSTANT;
    //limit
    if(app.faccY > value){
      app.faccY = (float)value;
    }
  }
  if(value < tval){
    app.faccY = app.faccY - ACCELLERATION_CONSTANT;
    //limit
    if(app.faccY < value){
      app.faccY = (float)value;
    }
  }  
  
  /* //ORIGINAL
  if(value > 0.0){
    if(value > tval){
      //Accelleration
      if(tval < 0.0){
        app.faccY = 0.0;
      }else{
        app.faccY = app.faccY + ACCELLERATION_CONSTANT;
        //limit
        if(app.faccY > value){
          app.faccY = (float)value;
        }
      }
    }else if(value < tval){
        app.faccY = (float)value;
    }else{
        app.faccY = (float)value;
    }    
  }else if(value < 0){
    if(value < tval){
      //Accelleration
      if(tval > 0){
        app.faccY = 0.0;
      }else{      
        app.faccY = app.faccY - ACCELLERATION_CONSTANT;
        //limit
        if(app.faccY < value){
          app.faccY = (float)value;
        }
      }
    }else if(value > tval){
        app.faccY = (float)value;
    }else{
        app.faccY = (float)value;
    }    
  }else{
    app.faccY = (float)value;
  }*/

  tval = app.faccY;
 
  return tval;
}

float accellerationX(float value){
  /* value -> -1000 0 +1000 */
  float tval;
  
  tval = app.faccX;
  
  //EXPERIMENT
  if(value > tval){
    app.faccX = app.faccX + ACCELLERATION_CONSTANT;
    //limit
    if(app.faccX > value){
      app.faccX = (float)value;
    }
  }
  if(value < tval){  
    app.faccX = app.faccX - ACCELLERATION_CONSTANT;
    //limit
    if(app.faccX < value){
      app.faccX = (float)value;
    }
  }
  /* ORIGINAL
  if(value > 0){
    if(value > tval){
      //Accelleration
      if(tval < 0.0){
        app.faccX = 0.0;
      }else{
        app.faccX = app.faccX + ACCELLERATION_CONSTANT;
        //limit
        if(app.faccX > value){
          app.faccX = (float)value;
        }
      }
    }else if(value < tval){
        app.faccX = (float)value;
    }else{
        app.faccX = (float)value;
    }    
  }else if(value < 0){
    if(value < tval){
      //Accelleration
      if(tval > 0.0){
        app.faccX = 0.0;
      }else{      
        app.faccX = app.faccX - ACCELLERATION_CONSTANT;
        //limit
        if(app.faccX < value){
          app.faccX = (float)value;
        }
      }
    }else if(value > tval){
        app.faccX = (float)value;
    }else{
        app.faccX = (float)value;
    }    
  }else{
    app.faccX = (float)value;
  }
*/
  tval = app.faccX;
 
  return tval;
}



// BATTERY TASK
/*
void Battery_TASK(void){
  if((HAL_GetTick() - battery_dati.time_batt)>200){
    battery_dati.time_batt = HAL_GetTick();
    
   battery_dati.somma_batt = battery_dati.somma_batt  + ADC_BATTERY();
   battery_dati.counter_media++;
   if(battery_dati.counter_media >= 30){
    battery_dati.counter_media = 0;
      
      //Batteria media valore
      battery_dati.VBatt = (float)battery_dati.somma_batt / 30.0;     
      battery_dati.VBatt = battery_dati.VBatt * ADC_BATTERY_VOLT;
      
      battery_dati.somma_batt = 0;
   }
  }
}
*/

//http://stackoverflow.com/questions/10990618/calculate-rolling-moving-average-in-c/10990656#10990656
//ROLLING ACERAGE
void Battery_TASK(void){
  uint32_t temp32;
  if((HAL_GetTick() - battery_dati.time_batt)>200){
    battery_dati.time_batt = HAL_GetTick();

    temp32 = battery_dati.somma_batt;
    battery_dati.somma_batt -=  temp32 / 30.0;    
    battery_dati.somma_batt += ADC_BATTERY() / 30.0;
   
      //Batteria media valore
   battery_dati.VBatt = battery_dati.somma_batt * ADC_BATTERY_VOLT;
   
  }
}




// CURRENT MOTOR TASK
/*
void Current_Motor_TASK(void){
  if((HAL_GetTick() - app.Current_time_measure)>100){
    app.Current_time_measure = HAL_GetTick();
    
   app.somma_current_m_L = app.somma_current_m_L  + ADC_MOTOR_LEFT();
   app.somma_current_m_R = app.somma_current_m_R  + ADC_MOTOR_RIGHT();
   
   app.current_counter_media++;
   if(app.current_counter_media >= 10){
    app.current_counter_media = 0;
      
      //Current media valore
      app.Current_M_LEFT = (float)app.somma_current_m_L / 10.0;     
      app.Current_M_RIGHT = (float)app.somma_current_m_R / 10.0;     
      
      if(app.Current_M_RIGHT < ADC_MOTOR_R_CENTER){
        app.Current_M_RIGHT = 0.0;
      }else{
        app.Current_M_RIGHT = ((app.Current_M_RIGHT  - ADC_MOTOR_R_CENTER) * MOTOR_R_AMP_CONV_AMP);
      }
      if(app.Current_M_LEFT < ADC_MOTOR_L_CENTER){
        app.Current_M_LEFT = 0.0;
      }else{
        app.Current_M_LEFT = ((app.Current_M_LEFT  - ADC_MOTOR_L_CENTER) * MOTOR_L_AMP_CONV_AMP);
      }
      
      app.somma_current_m_L = 0;
      app.somma_current_m_R = 0;
   }
  }
}
*/
//ROLLING ACERAGE
void Current_Motor_TASK(void){
  uint32_t temp32;
  if((HAL_GetTick() - app.Current_time_measure)>100){
    app.Current_time_measure = HAL_GetTick();
   
    temp32 = app.somma_current_m_L;
    app.somma_current_m_L -=  temp32 / 5.0;    
    app.somma_current_m_L += ADC_MOTOR_LEFT() / 5.0;
   
    temp32 = app.somma_current_m_R;
    app.somma_current_m_R -=  temp32 / 5.0;   
    app.somma_current_m_R += ADC_MOTOR_RIGHT() / 5.0;
   
   

   //Current media valore
    app.Current_M_LEFT = (float)app.somma_current_m_L;     
    app.Current_M_RIGHT = (float)app.somma_current_m_R;     
      
    if(app.Current_M_RIGHT < ADC_MOTOR_R_CENTER){
      app.Current_M_RIGHT = 0.0;
    }else{
      app.Current_M_RIGHT = ((app.Current_M_RIGHT  - ADC_MOTOR_R_CENTER) * MOTOR_R_AMP_CONV_AMP);
    }
    if(app.Current_M_LEFT < ADC_MOTOR_L_CENTER){
      app.Current_M_LEFT = 0.0;
    }else{
      app.Current_M_LEFT = ((app.Current_M_LEFT  - ADC_MOTOR_L_CENTER) * MOTOR_L_AMP_CONV_AMP);
    }
      
   
  }
}


//  MOTOR

/*
void go_motor(float throttle, float steering, float Vbatt){
  float limitSup, limitInf ;
  
  limitSup = 1000.0 * get_powerMax(Vbatt);
  limitInf = limitSup * -1;
    
      if(throttle>0.0){
        if(throttle > 20){
          app.motATS=(int8_t)(throttle+(steering * 0.333));
          app.motBTS=(int8_t)(throttle-(steering * 0.333));
        }else if((throttle > 15)){
          app.motATS=(int8_t)(throttle+(steering * 0.5));
          app.motBTS=(int8_t)(throttle-(steering * 0.5));
        }else{
          app.motATS=(int8_t)(throttle+(steering));
          app.motBTS=(int8_t)(throttle-(steering));
        }
      }else if(throttle<0.0){
          app.motATS=(int8_t)(throttle+(steering));
          app.motBTS=(int8_t)(throttle-(steering));
      }else{
          app.motATS=(int8_t)steering;
          app.motBTS=(int8_t)((steering)*-1);
      }

    //printf("%d ; %d\r\n",motATS,motBTS); 
    
      if(app.motATS >= 0){
        //0 ~ 100
        if(app.motATS > limitSup){
          app.motATS = (int16_t)limitSup;
        }
        MotorR_pwm(app.motATS);      
      }else{
        //0 ~ -100
        if(app.motATS < limitInf){
          app.motATS = (int16_t)limitInf;
        }
        //app.motATS = app.motATS * -1;
        MotorR_pwm(app.motATS);      
      }
      
      if(app.motBTS >= 0){
        //0 ~ 100
        if(app.motBTS > limitSup){
          app.motBTS = (int16_t)limitSup;
        }
        MotorL_pwm(app.motBTS );      
      }else{
        //0 ~ -100
        if(app.motBTS < limitInf){
          app.motBTS = (int16_t)limitInf;
        }
        //MOTOR_VALUE.motBTS = MOTOR_VALUE.motBTS * -1;
        MotorL_pwm(app.motBTS );      
      }    
}
*/

// EXPERIMENT
// http://www.impulseadventure.com/elec/robot-differential-steering.html
void go_motor(float throttle, float steering, float Vbatt){
  float limitSup, limitInf ;
  
  limitSup = 1000.0 * get_powerMax(Vbatt);
  limitInf = limitSup * -1;
  
  
// INPUTS
int     nJoyX;              // Joystick X input                     (-128..+127)
int     nJoyY;              // Joystick Y input                     (-128..+127)

// OUTPUTS
int     nMotMixL;           // Motor (left)  mixed output           (-128..+127)
int     nMotMixR;           // Motor (right) mixed output           (-128..+127)

// CONFIG
// - fPivYLimt  : The threshold at which the pivot action starts
//                This threshold is measured in units on the Y-axis
//                away from the X-axis (Y=0). A greater value will assign
//                more of the joystick's range to pivot actions.
//                Allowable range: (0..+127)
float fPivYLimit = 85.0;
			  		
// TEMP VARIABLES
float   nMotPremixL;    // Motor (left)  premixed output        (-128..+127)
float   nMotPremixR;    // Motor (right) premixed output        (-128..+127)
int     nPivSpeed;      // Pivot Speed                          (-128..+127)
float   fPivScale;      // Balance scale b/w drive and pivot    (   0..1   )


nJoyY = (int)throttle;
nJoyX = (int)steering;



// Calculate Drive Turn output due to Joystick X input
if (nJoyY >= 0) {
  // Forward
  nMotPremixL = (nJoyX>=0)? limitSup : (limitSup + nJoyX);
  nMotPremixR = (nJoyX>=0)? (limitSup - nJoyX) : limitSup;
} else {
  // Reverse
  nMotPremixL = (nJoyX>=0)? (limitSup - nJoyX) : limitSup;
  nMotPremixR = (nJoyX>=0)? limitSup : (limitSup + nJoyX);
}

// Scale Drive output due to Joystick Y input (throttle)
nMotPremixL = nMotPremixL * nJoyY/100.0;
nMotPremixR = nMotPremixR * nJoyY/100.0;

// Now calculate pivot amount
// - Strength of pivot (nPivSpeed) based on Joystick X input
// - Blending of pivot vs drive (fPivScale) based on Joystick Y input
nPivSpeed = nJoyX*3; //************************************
fPivScale = (abs(nJoyY)>fPivYLimit)? 0.0 : (1.0 - abs(nJoyY)/fPivYLimit);

// Calculate final mix of Drive and Pivot
nMotMixL = (int)((1.0-fPivScale)*nMotPremixL + fPivScale*( nPivSpeed));
nMotMixR = (int)((1.0-fPivScale)*nMotPremixR + fPivScale*(-nPivSpeed));



/*
if((nMotMixL>0)&&(nMotMixR>0)){
  app.motATS = nMotMixL + FRICTION;
  app.motBTS = nMotMixR + FRICTION;
}else if ((nMotMixL<0)&&(nMotMixR<0)){
  app.motATS = nMotMixL - FRICTION;
  app.motBTS = nMotMixR - FRICTION;  
}else{
  if((nMotMixL>0)&&(nMotMixR<0)){
    app.motATS = nMotMixL + FRICTION;
    app.motBTS = nMotMixR - FRICTION + FRICTION/2;  
  }else if((nMotMixL<0)&&(nMotMixR>0)){
    app.motBTS = nMotMixR + FRICTION;  
    app.motATS = nMotMixL - FRICTION  + FRICTION/2;
  }else{
    app.motATS = nMotMixL;
    app.motBTS = nMotMixR;
  }
}
*/

app.motATS = nMotMixL;
app.motBTS = nMotMixR;

      if(app.motATS >= 0){
        //0 ~ 100
        if(app.motATS > limitSup){
          app.motATS = (int16_t)limitSup;
        }
        MotorR_pwm(app.motATS);      
      }else{
        //0 ~ -100
        if(app.motATS < limitInf){
          app.motATS = (int16_t)limitInf;
        }
        //app.motATS = app.motATS * -1;
        MotorR_pwm(app.motATS);      
      }
      
      if(app.motBTS >= 0){
        //0 ~ 100
        if(app.motBTS > limitSup){
          app.motBTS = (int16_t)limitSup;
        }
        MotorL_pwm(app.motBTS );      
      }else{
        //0 ~ -100
        if(app.motBTS < limitInf){
          app.motBTS = (int16_t)limitInf;
        }
        //MOTOR_VALUE.motBTS = MOTOR_VALUE.motBTS * -1;
        MotorL_pwm(app.motBTS );      
      }    
}

