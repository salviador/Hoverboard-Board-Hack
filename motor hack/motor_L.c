#include "motor_L.h"
#include "stm32f1xx_hal.h"
#include "ADC_L.h"

TIM_HandleTypeDef htim8;

#define PWL_MOTOR_LEFT 25000                    //PWM
volatile __IO struct MOTOR_Ldati motorL;

// PULBLIC
void MotorL_init(void){
  MX_TIM8_Init();
  mL_HallSensor_init();
  
  motorL.BLDCMotorL_count= 0;
  motorL.BLDCMotorL_flag = 0; 
  motorL.BLDCMotorL_velRAW = 0;
  motorL.BLDCMotorL_deltavel = 0;
  
  MotorL_stop();
}

void MotorL_start(void){
  mL_PWM_Set_ChALL(0);
  motorL.BLDCMotorL_deltavel = 0;
  motorL.BLDCMotorL_velRAW = 0;
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);  
  mL_PWM_Set_ChALL(0);
  motorL.stop = 0;  
  mL_BLDCMotor();
}

void MotorL_pwm(int16_t value_percent){
  static uint8_t last_motorL=0;
  static uint8_t last_motorLStop=0;
 
  if(value_percent > 1000){
    value_percent = 1000;
  }
  if(value_percent < -1000){
    value_percent = -1000;
  }
  if(value_percent < 0){
    value_percent = value_percent * -1;

    if(last_motorL==0){
      mL_PWM_Set_ChALL(value_percent);
    }
    motorL.reverse = 1;
    last_motorL = 1;
  }else{
    if(last_motorL==1){
      mL_PWM_Set_ChALL(value_percent);
    }
     motorL.reverse = 0;
     last_motorL = 0;
  }
  
if((value_percent >= -5)&&(value_percent <= 5)){ //da fare > e < soglie !!
    motorL.stop = 1;    //ferma motore
    last_motorLStop = 1;    
  }else{
    motorL.stop = 0;
    if(last_motorLStop){
        //riattiva
        mL_BLDCMotor();
    }
    last_motorLStop = 0;    
  }
  
  mL_PWM_Set_ChALL(value_percent);
}

void MotorL_stop(void){
  mL_PWM_Set_ChALL(0);
  motorL.stop = 1;
  mL_Low_CH1_OFF();
  mL_Low_CH2_OFF();
  mL_Low_CH3_OFF();
  mL_AHigh__OFF();
  mL_BHigh__OFF();
  mL_CHigh__OFF();

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);  
    motorL.BLDCMotorL_count= 0;
    motorL.BLDCMotorL_flag = 0;
}
    
// PRIVATE
/* TIM8 init function */
void MX_TIM8_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  //TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

   __HAL_RCC_TIM8_CLK_ENABLE();
   
  motorL.uwPeriodValue = (uint32_t) ((SystemCoreClock  / PWL_MOTOR_LEFT) - 1); //18000
  
  /*
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP; //TIM_COUNTERMODE_UP;
  htim8.Init.Period = motorL.uwPeriodValue;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET; //TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCNPOLARITY_HIGH; //TIM_OCPOLARITY_LOW;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCIDLESTATE_RESET; //TIM_OCNIDLESTATE_SET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
    sConfigOC.Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
    
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 80;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_LOW;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  
  //CH1
  htim8.Instance->CCER = htim8.Instance->CCER | 0x0008; //CC1NP = 1;
  htim8.Instance->CCER = htim8.Instance->CCER & 0xFFFD; //cc1p = 0
  //CH2   
  htim8.Instance->CCER = htim8.Instance->CCER | 0x0080; //CC2NP = 1;
  htim8.Instance->CCER = htim8.Instance->CCER & 0xFFDF; //cc2p = 0
  //CH3     
  htim8.Instance->CCER = htim8.Instance->CCER | 0x0800; //CC3NP = 1;
  htim8.Instance->CCER = htim8.Instance->CCER & 0xFDFF; //cc3p = 0
  

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();  
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
 
    //Start PWM signals
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_1);      //CH1N   
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);      //CH1
    mL_A__OFF();

    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);         //CH2
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_2);      //CH2N
    mL_B__OFF();

    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);         //CH3
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_3);      //CH3N
    mL_C__OFF();  

    HAL_TIM_PWM_Start_IT(&htim8, TIM_CHANNEL_4);      //x interrupt
   */
  
__HAL_RCC_TIM8_CLK_ENABLE();
__HAL_RCC_GPIOA_CLK_ENABLE();
__HAL_RCC_GPIOB_CLK_ENABLE();
__HAL_RCC_GPIOC_CLK_ENABLE();
  mL_Low_CH1_OFF();
  mL_Low_CH2_OFF();
  mL_Low_CH3_OFF();
  //CH1N,CH2N,CH3N -- MANUAL mode -- 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLUP;  
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); 

  htim8.Instance = TIM8;
  htim8.Init.Prescaler         = 0;
  htim8.Init.Period            = motorL.uwPeriodValue;
  htim8.Init.ClockDivision     = 0;
  htim8.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim8.Init.RepetitionCounter = 0;
  HAL_TIM_PWM_Init(&htim8); 

  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE; //TIM_AUTOMATICOUTPUT_ENABLE; //
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_LOW; // TIM_BREAKPOLARITY_HIGH; //
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE; //TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE; //TIM_OSSR_DISABLE;  
  sBreakDeadTimeConfig.DeadTime = 40;   //10;  
  HAL_TIMEx_ConfigBreakDeadTime(&htim8,&sBreakDeadTimeConfig);
  
                                
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
  HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1); 
  sConfigOC.Pulse = 0;
  HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2);
  sConfigOC.Pulse = 0;
  HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3);

  //Start PWM signals
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);         //CH1
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);         //CH2
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);         //CH3
  HAL_TIM_PWM_Start_IT(&htim8, TIM_CHANNEL_4);         //CH4  
  
  mL_PWM_Set_ChALL(0);
  
  mL_Low_CH1_OFF();
  mL_Low_CH2_OFF();
  mL_Low_CH3_OFF();
  mL_AHigh__OFF();
  mL_BHigh__OFF();
  mL_CHigh__OFF();
    
  motorL.stop = 1;
 /* Peripheral interrupt init */
  HAL_NVIC_SetPriority(TIM8_CC_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
}

void mL_HallSensor_init(void){
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : HALL_LEFT_A_Pin HALL_LEFT_B_Pin HALL_LEFT_C_Pin */
  GPIO_InitStruct.Pin = HALL_LEFT_A_Pin|HALL_LEFT_B_Pin|HALL_LEFT_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;  
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void mL_Low_CH1_ON(void){  
  //Negato
  GPIOA->BSRR = 0x00800000;
}
void mL_Low_CH1_OFF(void){
  GPIOA->BSRR = GPIO_PIN_7;
}
void mL_Low_CH2_ON(void){  
  //Negato
  GPIOB->BSRR = 0x00010000;
}
void mL_Low_CH2_OFF(void){
  GPIOB->BSRR = GPIO_PIN_0;
}
void mL_Low_CH3_ON(void){  
  //Negato
  GPIOB->BSRR = 0x00020000;  
}
void mL_Low_CH3_OFF(void){
  GPIOB->BSRR = GPIO_PIN_1;
}

void mL_AHigh__ON(void){
  htim8.Instance->CCER = htim8.Instance->CCER | 0x0001;  //mR_AHigh__ON(); --> CC1E = 1
}
void mL_AHigh__OFF(void){
  htim8.Instance->CCER = htim8.Instance->CCER & 0xFFFE;  //mR_AHigh__ON(); --> CC1E = 0
}
void mL_BHigh__ON(void){
  htim8.Instance->CCER = htim8.Instance->CCER | 0x0010;  //--> CC2E = 1
}
void mL_BHigh__OFF(void){
  htim8.Instance->CCER = htim8.Instance->CCER & 0xFFEF;  //--> CC2E = 0
}
void mL_CHigh__ON(void){
  htim8.Instance->CCER = htim8.Instance->CCER | 0x0100;  //--> CC3E = 1
}
void mL_CHigh__OFF(void){
  htim8.Instance->CCER = htim8.Instance->CCER & 0xFEFF;  //--> CC2E = 0
}


void mL_PWM_Set_Ch1(uint16_t value){
  uint32_t vpwm;
  vpwm = (uint32_t)(motorL.uwPeriodValue*value/1000);
  __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_1,vpwm);
}
void mL_PWM_Set_Ch2(uint16_t value){
  uint32_t vpwm;
  vpwm = (uint32_t)(motorL.uwPeriodValue*value/1000);
  __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_2,vpwm);
}
void mL_PWM_Set_Ch3(uint16_t value){
  uint32_t vpwm;
  vpwm = (uint32_t)(motorL.uwPeriodValue*value/1000);
  __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_3,vpwm);
}
void mL_PWM_Set_ChALL(uint16_t value){
  uint32_t vpwm;
  vpwm = (uint32_t)(motorL.uwPeriodValue*value/1000);
  __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_1,vpwm);
  __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_2,vpwm);
  __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_3,vpwm);  
}




void mL_BLDCMotor(void){
  motorL.newhallpos = (GPIOB->IDR & 0x00E0);    //b5,b6,b7
    
  mL_Low_CH1_OFF();
  mL_Low_CH2_OFF();
  mL_Low_CH3_OFF();
  mL_AHigh__OFF();
  mL_BHigh__OFF();
  mL_CHigh__OFF();

  if(motorL.stop){
    return;
  }
  
  if(motorL.reverse!=0){
    switch (motorL.newhallpos){
      case (0x0020):               //1
        mL_AHigh__ON();
        mL_Low_CH2_ON();
      break;
      case (0x0040):               //2
        mL_BHigh__ON();
        mL_Low_CH3_ON();    
      break;
      case (0x0060):               //3
        mL_AHigh__ON();
        mL_Low_CH3_ON();    
      break;
      case (0x0080):              //4
        mL_Low_CH1_ON();    
        mL_CHigh__ON();
      break;
      case (0x00A0):              //5
        mL_Low_CH2_ON();    
        mL_CHigh__ON();
      break;
      case (0x00C0):              //6
        mL_Low_CH1_ON();    
        mL_BHigh__ON();
      break;  
      default:
        __NOP();
      break;      
    }    
  }else{
    switch (motorL.newhallpos){
      case (0x0020):               //1
        mL_Low_CH1_ON();
        mL_BHigh__ON();
      break;
      case (0x0040):               //2
        mL_Low_CH2_ON();
        mL_CHigh__ON();        
      break;
      case (0x0060):               //3
        mL_Low_CH1_ON();        
        mL_CHigh__ON();
      break;
      case (0x0080):              //4
        mL_AHigh__ON();
        mL_Low_CH3_ON();        
      break;
      case (0x00A0):              //5
        mL_BHigh__ON();
        mL_Low_CH3_ON();        
      break;
      case (0x00C0):              //6
        mL_AHigh__ON();
        mL_Low_CH2_ON();        
      break;  
      default:
        __NOP();
      break;
    }   
  }
}





void HALL_L_ISR_Callback(void){
    motorL.BLDCMotorL_count= 1;        //Delay x ritardo x debounce
    motorL.BLDCMotorL_flag = 1;        //Update motor
}

//40uS Interrupt -- 25Khz
void PWM_L_ISR_Callback(void){
   if((motorL.BLDCMotorL_flag)&&(motorL.BLDCMotorL_count==0)){
      mL_BLDCMotor();        //5uS
      motorL.BLDCMotorL_flag = 0;      
      //---
      motorL.BLDCMotorL_velRAW = motorL.BLDCMotorL_deltavel;
      motorL.BLDCMotorL_deltavel = 0;      
   }else if((motorL.BLDCMotorL_flag)&&(motorL.BLDCMotorL_count!=0)){
      motorL.BLDCMotorL_count--;
   }
   motorL.BLDCMotorL_deltavel++;       //Calcolo velocita motore inc ogni 40uS
   if(motorL.BLDCMotorL_deltavel>=5000){
    motorL.BLDCMotorL_deltavel = 5000;
    motorL.BLDCMotorL_velRAW = 0;     //Minima vel, motore fermo
   }
}
