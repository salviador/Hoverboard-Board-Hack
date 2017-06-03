/**
  ******************************************************************************
  * @file    stm32l0xx_hal_bluenrg_syscfg.c
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
#include "stm32l0xx_hal.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
/**
  * @brief  Selects the GPIO pin used as EXTI Line.
  *
  * @param  GPIOx : selects the GPIO port to be used as source
  *
  * @param  GPIO_Pin_Source: specifies the EXTI line to be configured.
  *
  * @retval None
  */
void HAL_LPPUART_SYSCFG_Set_EXTI_Conf(uint8_t SYSCFG_EXTI_PORTx, uint32_t GPIO_Pin_Source)
{
	uint32_t temp;

    temp = SYSCFG->EXTICR[GPIO_Pin_Source >> 2];
    temp &= ~((uint32_t)0x0F) << (4 * (GPIO_Pin_Source & 0x03));
    temp |= ((uint32_t)(SYSCFG_EXTI_PORTx) << (4 * (GPIO_Pin_Source & 0x03)));
    SYSCFG->EXTICR[GPIO_Pin_Source >> 2] = temp;

	return;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
