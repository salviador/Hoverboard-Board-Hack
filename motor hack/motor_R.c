#include "motor_R.h"
#include "stm32f1xx_hal.h"
#include "ADC_R.h"

TIM_HandleTypeDef htim1;

#define PWL_MOTOR_RIGHT 25000                    //PWM
volatile __IO struct MOTOR_Rdati motorR;

// PULBLIC
void MotorR_init(void){
  MX_TIM1_Init();
  mR_HallSensor_init();
  motorR.BLDCMotorL_count= 0;
  motorR.BLDCMotorL_flag = 0; 
  motorR.BLDCMotorL_velRAW = 0;
  motorR.BLDCMotorL_deltavel = 0;
  
  MotorR_stop();
}

void MotorR_start(void){
  mR_PWM_Set_ChALL(0); 
  motorR.BLDCMotorL_deltavel = 0;
  motorR.BLDCMotorL_velRAW = 0;
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);  
  mR_PWM_Set_ChALL(0);
  motorR.stop = 0;
  mR_BLDCMotor();
}

void MotorR_pwm(int16_t value_percent){
  static uint8_t last_motorR=0;
  static uint8_t last_motorRStop=0;
  
  if(value_percent > 1000){
    value_percent = 1000;
  }
  if(value_percent < -1000){
    value_percent = -1000;
  }
  if(value_percent < 0){
    value_percent = value_percent * -1;

    if(last_motorR==0){
      mR_PWM_Set_ChALL(value_percent);
    }
    motorR.reverse = 1;
    last_motorR = 1;
  }else{
    if(last_motorR==1){
      mR_PWM_Set_ChALL(value_percent);
    }
     motorR.reverse = 0;
     last_motorR = 0;
  }
  
  if((value_percent >= -5)&&(value_percent <= 5)){ //da fare > e < soglie !!
    motorR.stop = 1;    //ferma motore
    last_motorRStop = 1;    
  }else{
    motorR.stop = 0;
    if(last_motorRStop){
        //riattiva
        mR_BLDCMotor();
    }
    last_motorRStop = 0;    
  }
  
  mR_PWM_Set_ChALL(value_percent);
}

void MotorR_stop(void){
  mR_PWM_Set_ChALL(0);
  motorR.stop = 1;
  mR_Low_CH1_OFF();
  mR_Low_CH2_OFF();
  mR_Low_CH3_OFF();
  mR_AHigh__OFF();
  mR_BHigh__OFF();
  mR_CHigh__OFF();

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_11);
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);  
    motorR.BLDCMotorL_count= 0;
    motorR.BLDCMotorL_flag = 0;
}
    
// PRIVATE
/* TIM1 init function */
void MX_TIM1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  //TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

   __HAL_RCC_TIM1_CLK_ENABLE();

  motorR.uwPeriodValue = (uint32_t) ((SystemCoreClock  / PWL_MOTOR_RIGHT) - 1); 
/*  
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP; //TIM_COUNTERMODE_CENTERALIGNED1
  htim1.Init.Period = motorR.uwPeriodValue;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET; //TIM_TRGO_ENABLE; //TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCNPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 80;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE; //TIM_BREAK_ENABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_LOW;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  
  //CH1
  htim1.Instance->CCER = htim1.Instance->CCER | 0x0008; //CC1NP = 1;
  htim1.Instance->CCER = htim1.Instance->CCER & 0xFFFD; //cc1p = 0
  //CH2   
  htim1.Instance->CCER = htim1.Instance->CCER | 0x0080; //CC2NP = 1;
  htim1.Instance->CCER = htim1.Instance->CCER & 0xFFDF; //cc2p = 0
  //CH3     
  htim1.Instance->CCER = htim1.Instance->CCER | 0x0800; //CC3NP = 1;
  htim1.Instance->CCER = htim1.Instance->CCER & 0xFDFF; //cc3p = 0
  
 
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();  
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;  
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  //Start PWM signals
  //HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1);      //CH1
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);      //CH1 
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);      //CH1N
  mR_A__OFF();

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);         //CH2
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);      //CH2N
  mR_B__OFF();

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);         //CH3
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);      //CH3N
  mR_C__OFF();  

  HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_4);    //for interrupt

  */
  
  
  //Test
  

  __HAL_RCC_TIM1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  mR_Low_CH1_OFF();
  mR_Low_CH2_OFF();
  mR_Low_CH3_OFF();
   //CH1N,CH2N,CH3N -- MANUAL mode --
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  //CH1,CH2,CH3
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  htim1.Instance = TIM1;
  htim1.Init.Prescaler         = 0;
  htim1.Init.Period            = motorR.uwPeriodValue;
  htim1.Init.ClockDivision     = 0;
  htim1.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim1.Init.RepetitionCounter = 0;
  HAL_TIM_PWM_Init(&htim1); 
  
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE; //TIM_AUTOMATICOUTPUT_ENABLE; //
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_LOW; // TIM_BREAKPOLARITY_HIGH; //
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE; //TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE; //TIM_OSSR_DISABLE;  
  sBreakDeadTimeConfig.DeadTime = 40;   //10;  
  HAL_TIMEx_ConfigBreakDeadTime(&htim1,&sBreakDeadTimeConfig);
  
                                
  //##-2- Configure the PWM channels #########################################
  // Common configuration for all channels 
  sConfigOC.OCMode      = TIM_OCMODE_PWM1; // TIM_OCMODE_PWM2;
  sConfigOC.OCFastMode  = TIM_OCFAST_DISABLE; // TIM_OCFAST_DISABLE;
  sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH; //TIM_OCPOLARITY_LOW;//TIM_OCPOLARITY_HIGH; //TIM_OCPOLARITY_LOW;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW; //TIM_OCNPOLARITY_HIGH; //TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET; //TIM_OCIDLESTATE_SET; //TIM_OCNIDLESTATE_RESET; //TIM_OCIDLESTATE_SET;
  sConfigOC.OCNIdleState= TIM_OCNIDLESTATE_RESET; //TIM_OCNIDLESTATE_SET ; //TIM_OCIDLESTATE_SET; //TIM_OCNIDLESTATE_RESET;
  
  //Set the pulse value
  sConfigOC.Pulse = 0;
  HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1); 
  sConfigOC.Pulse = 0;
  HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);
  sConfigOC.Pulse = 0;
  HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3);

  //Start PWM signals
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);         //CH1
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);         //CH2
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);         //CH3
  HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_4);         //CH4  
  
  mR_PWM_Set_ChALL(0);
  
  mR_Low_CH1_OFF();
  mR_Low_CH2_OFF();
  mR_Low_CH3_OFF();
  mR_AHigh__OFF();
  mR_BHigh__OFF();
  mR_CHigh__OFF();
  
   motorR.stop = 1;
  /* Peripheral interrupt init */
  HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
  
}


void mR_Low_CH1_ON(void){  
  //Negato
  GPIOB->BSRR = 0x20000000;
}
void mR_Low_CH1_OFF(void){
  GPIOB->BSRR = GPIO_PIN_13;
}
void mR_Low_CH2_ON(void){  
  //Negato
  GPIOB->BSRR = 0x40000000;
}
void mR_Low_CH2_OFF(void){
  GPIOB->BSRR = GPIO_PIN_14;
}
void mR_Low_CH3_ON(void){  
  //Negato
  GPIOB->BSRR = 0x80000000;  
}
void mR_Low_CH3_OFF(void){
  GPIOB->BSRR = GPIO_PIN_15;
}

void mR_HallSensor_init(void){
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pins : HALL_RIGHT_A_Pin HALL_RIGHT_B_Pin HALL_RIGHT_C_Pin */
  GPIO_InitStruct.Pin = HALL_RIGHT_A_Pin|HALL_RIGHT_B_Pin|HALL_RIGHT_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;  
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void mR_AHigh__ON(void){
  htim1.Instance->CCER = htim1.Instance->CCER | 0x0001;  //mR_AHigh__ON(); --> CC1E = 1
}
void mR_AHigh__OFF(void){
  htim1.Instance->CCER = htim1.Instance->CCER & 0xFFFE;  //mR_AHigh__ON(); --> CC1E = 0
}
void mR_BHigh__ON(void){
  htim1.Instance->CCER = htim1.Instance->CCER | 0x0010;  //--> CC2E = 1
}
void mR_BHigh__OFF(void){
  htim1.Instance->CCER = htim1.Instance->CCER & 0xFFEF;  //--> CC2E = 0
}
void mR_CHigh__ON(void){
  htim1.Instance->CCER = htim1.Instance->CCER | 0x0100;  //--> CC3E = 1
}
void mR_CHigh__OFF(void){
  htim1.Instance->CCER = htim1.Instance->CCER & 0xFEFF;  //--> CC2E = 0
}


void mR_PWM_Set_Ch1(uint16_t value){
  uint32_t vpwm;
  vpwm = (uint32_t)(motorR.uwPeriodValue*value/1000);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,vpwm);
}
void mR_PWM_Set_Ch2(uint16_t value){
  uint32_t vpwm;
  vpwm = (uint32_t)(motorR.uwPeriodValue*value/1000);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,vpwm);
}
void mR_PWM_Set_Ch3(uint16_t value){
  uint32_t vpwm;
  vpwm = (uint32_t)(motorR.uwPeriodValue*value/1000);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,vpwm);
}
void mR_PWM_Set_ChALL(uint16_t value){
  uint32_t vpwm;
  vpwm = (uint32_t)(motorR.uwPeriodValue*value/1000);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,vpwm);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,vpwm);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,vpwm);  
}

void mR_BLDCMotor(void){
  motorR.newhallpos = (GPIOC->IDR & 0x1C00);
  
  mR_Low_CH1_OFF();
  mR_Low_CH2_OFF();
  mR_Low_CH3_OFF();
  mR_AHigh__OFF();
  mR_BHigh__OFF();
  mR_CHigh__OFF();

  if(motorR.stop){
    return;
  }
  
  if(motorR.reverse!=0){
    switch (motorR.newhallpos){
      case (0x400):               //1
        mR_Low_CH1_ON();
        mR_BHigh__ON();
      break;
      case (0x800):               //2
        mR_Low_CH2_ON();
        mR_CHigh__ON();        
      break;
      case (0xC00):               //3
        mR_Low_CH1_ON();        
        mR_CHigh__ON();
      break;
      case (0x1000):              //4
        mR_AHigh__ON();
        mR_Low_CH3_ON();        
      break;
      case (0x1400):              //5
        mR_BHigh__ON();
        mR_Low_CH3_ON();        
      break;
      case (0x1800):              //6
        mR_AHigh__ON();
        mR_Low_CH2_ON();        
      break;  
      default:
        __NOP();
      break;
    }   
  }else{
    switch (motorR.newhallpos){
      case (0x400):               //1
        mR_AHigh__ON();
        mR_Low_CH2_ON();
      break;
      case (0x800):               //2
        mR_BHigh__ON();
        mR_Low_CH3_ON();    
      break;
      case (0xC00):               //3
        mR_AHigh__ON();
        mR_Low_CH3_ON();    
      break;
      case (0x1000):              //4
        mR_Low_CH1_ON();    
        mR_CHigh__ON();
      break;
      case (0x1400):              //5
        mR_Low_CH2_ON();    
        mR_CHigh__ON();
      break;
      case (0x1800):              //6
        mR_Low_CH1_ON();    
        mR_BHigh__ON();
      break;  
      default:
        __NOP();
      break;      
    }     
  }
}

void HALL_R_ISR_Callback(void){
    motorR.BLDCMotorL_count= 1;        //Delay x ritardo x debounce
    motorR.BLDCMotorL_flag = 1;        //Update motor
}

//40uS Interrupt -- 25Khz
void PWM_R_ISR_Callback(void){
   if((motorR.BLDCMotorL_flag)&&(motorR.BLDCMotorL_count==0)){
      mR_BLDCMotor();        //5uS
      motorR.BLDCMotorL_flag = 0;      
      //---
      motorR.BLDCMotorL_velRAW = motorR.BLDCMotorL_deltavel;
      motorR.BLDCMotorL_deltavel = 0;      
      
   }else if((motorR.BLDCMotorL_flag)&&(motorR.BLDCMotorL_count!=0)){
      motorR.BLDCMotorL_count--;
   }
   motorR.BLDCMotorL_deltavel++;       //Calcolo velocita motore inc ogni 40uS
   if(motorR.BLDCMotorL_deltavel>=5000){
    motorR.BLDCMotorL_deltavel = 5000;
    motorR.BLDCMotorL_velRAW = 0;     //Minima vel, motore fermo
   }
}
