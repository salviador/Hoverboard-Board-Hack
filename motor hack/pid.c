#include "pid.h"

TIM_HandleTypeDef Tim3PID;

struct PID_dati pid_L_,pid_R_;
struct pid_I32_t pid_L_Dati,pid_R_Dati;

#define PID_PERIOD 5    //Hz

void PID_init(uint16_t min, uint16_t max)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
    
  __HAL_RCC_TIM3_CLK_ENABLE();
  Tim3PID.Instance = TIM3;
  Tim3PID.Init.Period = 64500;                
  Tim3PID.Init.Prescaler = 198;
  Tim3PID.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  Tim3PID.Init.CounterMode = TIM_COUNTERMODE_UP;        
  HAL_TIM_Base_Init(&Tim3PID);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&Tim3PID, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&Tim3PID, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_Base_Start_IT(&Tim3PID);
  HAL_NVIC_SetPriority(TIM3_IRQn, 3, 4);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);  
 
  pid_L_Dati.min = min;
  pid_L_Dati.max = max;
  pid_R_Dati.min = min;
  pid_R_Dati.max = max;

  pid_L_.mS = HAL_GetTick();
  pid_R_.mS = HAL_GetTick();
}

void PID_set_L_costant(float P,float I,float D){
  pid_L_Dati.kp = P;
  pid_L_Dati.ki = I;
  pid_L_Dati.kd = D;
}
void PID_set_R_costant(float P,float I,float D){
  pid_R_Dati.kp = P;
  pid_R_Dati.ki = I;
  pid_R_Dati.kd = D;
}
void PID_L_task(int32_t set_speed, int32_t speed){
  uint32_t m_S;
  
  m_S = HAL_GetTick();
  if((m_S - pid_L_.mS)>=200){
    pid_L_.mS = HAL_GetTick(); 
    
     //Calculate the new duty cycle
    pid_L_.duty = pid_update_I32(set_speed, speed, &pid_L_Dati);
    
  }  
}

extern volatile __IO int32_t testVelR;
void PID_R_task(int32_t set_speed, int32_t speed){
  uint32_t m_S;
  
  m_S = HAL_GetTick();
  if((m_S - pid_R_.mS)>=200){
    pid_R_.mS = HAL_GetTick(); 
    
     //Calculate the new duty cycle
    pid_R_.duty = pid_update_I32(set_speed, testVelR, &pid_R_Dati);
    testVelR = 0;
    
  }  
}















//------------------------PRIVATE------------------------

float pid_update_f(float sp /*! The set point */,
    float pv /*! The process variable */,
    struct pid_f_t * ptr /*! A pointer to the PID constants */){
      
  float e;
  float manp;
  float tmpi;
  
  //errore
  e = ptr->e;
  ptr->e = sp - pv;
  
  //integral
  tmpi = ptr->i + ptr->e;
  
  //P*I*D
  manp = ptr->kp * ptr->e + ptr->ki * tmpi + ptr->kd * (e - ptr->e);
  
  //Limit, max;min
  if( (manp < ptr->max) && (manp > ptr->min) ){
    ptr->i = tmpi;
  } else if ( manp > ptr->max ){
    manp = ptr->max;
  } else if ( manp < ptr->min ){
    manp = ptr->min;
  }
  return manp;
}

int32_t pid_update_I32(int32_t sp /*! The set point */,
    int32_t pv /*! The process variable */,
    struct pid_I32_t * ptr /*! A pointer to the PID constants */){
      
  int32_t e;
  float manp;
  int32_t tmpi;
  int32_t manpI32;
  
  //errore
  e = ptr->e;
  ptr->e = sp - pv;
  
  //integral
  tmpi = ptr->i + ptr->e;
  if(ptr->i > 300){
    ptr->i = 300;
  }
  if(ptr->i < -300){
    ptr->i = -300;
  }
  
  //P*I*D
  manp = (ptr->kp * ptr->e) + (ptr->ki * tmpi) + (ptr->kd * (e - ptr->e));
  manpI32 = (int32_t)manp;
  
  //Limit, max;min
  if( (manpI32 < ptr->max) && (manpI32 > ptr->min) ){
    ptr->i = tmpi;
  } else if ( manpI32 > ptr->max ){
    manpI32 = ptr->max;
  } else if ( manpI32 < ptr->min ){
    manpI32 = ptr->min;
  }
  return manpI32;
}
