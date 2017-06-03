/**
  ******************************************************************************
  * @file    stm32f4xx_hal_bluenrg_gpio.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4XX_HAL_LPPUART_GPIO_H
#define __STM32F4XX_HAL_LPPUART_GPIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported defines --------------------------------------------------------*/
#define GPIO_PIN_POSITION_0                 ((uint8_t)0x00)
#define GPIO_PIN_POSITION_1                 ((uint8_t)0x01)
#define GPIO_PIN_POSITION_2                 ((uint8_t)0x02)
#define GPIO_PIN_POSITION_3                 ((uint8_t)0x03)
#define GPIO_PIN_POSITION_4                 ((uint8_t)0x04)
#define GPIO_PIN_POSITION_5                 ((uint8_t)0x05)
#define GPIO_PIN_POSITION_6                 ((uint8_t)0x06)
#define GPIO_PIN_POSITION_7                 ((uint8_t)0x07)
#define GPIO_PIN_POSITION_8                 ((uint8_t)0x08)
#define GPIO_PIN_POSITION_9                 ((uint8_t)0x09)
#define GPIO_PIN_POSITION_10                ((uint8_t)0x0A)
#define GPIO_PIN_POSITION_11                ((uint8_t)0x0B)
#define GPIO_PIN_POSITION_12                ((uint8_t)0x0C)
#define GPIO_PIN_POSITION_13                ((uint8_t)0x0D)
#define GPIO_PIN_POSITION_14                ((uint8_t)0x0E)
#define GPIO_PIN_POSITION_15                ((uint8_t)0x0F)

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
 void HAL_LPPUART_GPIO_Set_PUPD(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin_Source, uint32_t GPIOPuPd);
 void HAL_LPPUART_GPIO_Set_Mode(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin_Source, uint32_t GPIO_Mode);
 void HAL_LPPUART_GPIO_Set_PinAFConfig(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin_Source, uint8_t GPIO_Alternate);


#ifdef __cplusplus
}
#endif

#endif /*__STM32F4XX_HAL_LPPUART_GPIO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
