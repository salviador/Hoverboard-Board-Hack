/**
  ******************************************************************************
  * @file    stm32f4xx_hal_bluenrg_gpio.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    01-October-2014
  * @brief   HAL specific implementation for stm32xx_lppuart.c
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#include "stm32f4xx_hal.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Set the GPIO PU/PD configuration
  *
  * @param  GPIOx : The GPIO port on which the PU/PD configuration shall apply
  *
  * @param  GPIO_Pin_Source : The GPIO pin number on which the PU/PD configuration shall apply
  *
  * @param  GPIOPuPd : The PU/PD configuration that shall be applied
  *
  * @retval None
  */
void HAL_LPPUART_GPIO_Set_PUPD(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin_Source, uint32_t GPIOPuPd)
{
  /**
  * This is a temporary register to avoid any unwanted transient state when performing read/modify/write
  * operation on the destination register
  */
  uint32_t uwTempRegister;
  
  uwTempRegister = GPIOx->PUPDR;  
  uwTempRegister &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)GPIO_Pin_Source * 2));
  uwTempRegister |= (((uint32_t)GPIOPuPd) << ((uint16_t)GPIO_Pin_Source * 2));
  GPIOx->PUPDR = uwTempRegister;
  
  return;
}

/**
  * @brief  Set the GPIO Mode configuration
  *
  * @param  GPIOx : The GPIO port on which the PU/PD configuration shall apply
  *
  * @param  GPIO_Pin_Source : The GPIO pin number on which the PU/PD configuration shall apply
  *
  * @param  GPIO_Mode : The Mode configuration that shall be applied
  *
  * @retval None
  */
void HAL_LPPUART_GPIO_Set_Mode(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin_Source, uint32_t GPIO_Mode)
{
  /**
  * This is a temporary register to avoid any unwanted transient state when performing read/modify/write
  * operation on the destination register
  */
  uint32_t uwTempRegister;
  
  uwTempRegister = GPIOx->MODER;        
  uwTempRegister  &= ~(GPIO_MODER_MODER0 << (GPIO_Pin_Source * 2));
  uwTempRegister |= (((uint32_t)GPIO_Mode) << (GPIO_Pin_Source * 2));
  GPIOx->MODER = uwTempRegister;
  
  return;
}

/**
  * @brief  Set the GPIO Alternate mode configuration
  *
  * @param  GPIOx : The GPIO port on which the alternate configuration shall apply
  *
  * @param  GPIO_Pin_Source : The GPIO pin number on which the alternate configuration shall apply
  *
  * @param  GPIO_Alternate : The Alternate configuration that shall be applied
  *
  * @retval None
  */
void HAL_LPPUART_GPIO_Set_PinAFConfig(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin_Source, uint8_t GPIO_Alternate)
{
  uint32_t temp;
  
  /* Configure Alternate function mapped with the current IO */
  temp = GPIOx->AFR[GPIO_Pin_Source >> 3];
  temp &= ~((uint32_t)0x0F << ((uint32_t)(GPIO_Pin_Source & (uint32_t)0x07) * 4)) ;
  temp |= ((uint32_t)(GPIO_Alternate) << (((uint32_t)GPIO_Pin_Source & (uint32_t)0x07) * 4)) ;
  GPIOx->AFR[GPIO_Pin_Source >> 3] = temp;
  
  return;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
