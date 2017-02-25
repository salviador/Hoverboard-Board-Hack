/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include "motor_L.h"
#include "motor_R.h"
#include "varie.h"
#include "pid.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc3;
extern DMA_HandleTypeDef hdma_i2c2_rx;
extern DMA_HandleTypeDef hdma_i2c2_tx;
extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim8;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern void PWM_L_ISR_Callback(void);
extern void HALL_L_ISR_Callback(void);
extern void PWM_R_ISR_Callback(void);
extern void HALL_R_ISR_Callback(void);
extern TIM_HandleTypeDef Tim3PID;
/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc3;
extern DMA_HandleTypeDef hdma_adc3;

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  while(1){
    __NOP();
    __NOP();
    __NOP();
  }
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
  MotorR_stop();
  MotorL_stop();  

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
  MotorR_stop();
  MotorL_stop();  
  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */
  MotorR_stop();
  MotorL_stop();  
  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */
  MotorR_stop();
  MotorL_stop();  
  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  while(1){
    __NOP();
    __NOP();
    __NOP();
  }

  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  while(1){
    __NOP();
    __NOP();
    __NOP();
  }

  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

extern I2C_HandleTypeDef hi2c2;
void I2C1_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c2);
}

void I2C1_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hi2c2);
}


/**
* @brief This function handles DMA1 channel4 global interrupt.
*/
void DMA1_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */

  /* USER CODE END DMA1_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c2_tx);
  /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */

  /* USER CODE END DMA1_Channel4_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel5 global interrupt.
*/
void DMA1_Channel5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */

  /* USER CODE END DMA1_Channel5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c2_rx);
  /* USER CODE BEGIN DMA1_Channel5_IRQn 1 */

  /* USER CODE END DMA1_Channel5_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel6 global interrupt.
*/
void DMA1_Channel6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel6_IRQn 0 */

  /* USER CODE END DMA1_Channel6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
  /* USER CODE BEGIN DMA1_Channel6_IRQn 1 */

  /* USER CODE END DMA1_Channel6_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel7 global interrupt.
*/
void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}



/**
* @brief This function handles I2C2 error interrupt.
*/
void I2C2_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C2_ER_IRQn 0 */

  /* USER CODE END I2C2_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c2);
  /* USER CODE BEGIN I2C2_ER_IRQn 1 */

  /* USER CODE END I2C2_ER_IRQn 1 */
}

/**
* @brief This function handles EXTI line[15:10] interrupts.
*/
void EXTI15_10_IRQHandler(void)
{
  if((__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_10) != RESET)||
    (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_11) != RESET)||
    (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != RESET))
  {

    HALL_R_ISR_Callback();
     
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_11);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
  }  
}

/**
* @brief This function handles TIM1 capture compare interrupt.
*/


/**
* @brief This function handles EXTI line[9:5] interrupts.
*/
void EXTI9_5_IRQHandler(void)
{
  if((__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET)||
    (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_6) != RESET)||
    (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_7) != RESET))
  {

    HALL_L_ISR_Callback();
     
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
  }  
}

/**
* @brief This function handles TIM8 capture compare interrupt.
*/
void TIM1_CC_IRQHandler(void)
{
  //DebugPin4_ON();
  __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC1);
  __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC2);
  __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC3);
  __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC4);
  
  PWM_R_ISR_Callback();  
  HAL_ADC_Start(&hadc1);
  //DebugPin4_OFF();  
}
void TIM8_CC_IRQHandler(void)
{
  __HAL_TIM_CLEAR_IT(&htim8, TIM_IT_CC1);
  __HAL_TIM_CLEAR_IT(&htim8, TIM_IT_CC2);
  __HAL_TIM_CLEAR_IT(&htim8, TIM_IT_CC3);
  __HAL_TIM_CLEAR_IT(&htim8, TIM_IT_CC4);
  
  PWM_L_ISR_Callback();  
  HAL_ADC_Start(&hadc3);
}

//ADC1 -PC1-PC2-
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_adc1); 
}
//ADC3 -PC0-
void DMA2_Channel4_5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_adc3);
}


//extern volatile __IO int32_t testVelR;

//PID
void TIM3_IRQHandler(void)
{
  //DebugPin4_ON(); 
  __HAL_TIM_CLEAR_IT(&Tim3PID, TIM_IT_UPDATE);
  //DebugPin4_OFF(); 
//testVelR = 0;  
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
