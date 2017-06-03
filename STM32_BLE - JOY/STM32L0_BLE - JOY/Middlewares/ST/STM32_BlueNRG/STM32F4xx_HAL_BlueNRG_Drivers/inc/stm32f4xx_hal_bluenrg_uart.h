/**
  ******************************************************************************
  * @file    stm32f4xx_hal_bluenrg_uart.h
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
#ifndef __STM32F4XX_HAL_LPPUART_UART_H
#define __STM32F4XX_HAL_LPPUART_UART_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported defines --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
 /**
   * @brief  Enable the DMA transfer in the UART peripheral
   *
   * @param  __HANDLE__: UART handle.
   *
   * @param  __DMAREQ__: DMA Request to enabled.
   *
   * @retval None
   */
 #define __HAL_LPPUART_UART_ENABLE_DMAREQ(__HANDLE__, __DMAREQ__)   ((__HANDLE__)->Instance->CR3 |= (__DMAREQ__))

 /**
   * @brief  Disable the DMA transfer in the UART peripheral
   *
   * @param  __HANDLE__: UART handle.
   *
   * @param  __DMAREQ__: DMA Request to enabled.
   *
   * @retval None
   */
 #define __HAL_LPPUART_UART_DISABLE_DMAREQ(__HANDLE__, __DMAREQ__)   ((__HANDLE__)->Instance->CR3 &= (~(__DMAREQ__)))

 /**
   * @brief  Get the Receive UART Data peripheral address
   *
   * @param  __HANDLE__: The UART handle
   *
   * @retval The UART Data register address
   */
 #define __HAL_LPPUART_UART_GET_RX_DATA_REGISTER_ADDRESS(__HANDLE__)   ((uint32_t)&(__HANDLE__)->Instance->RDR)

 /**
   * @brief  Get the Transmit UART Data peripheral address
   *
   * @param  __HANDLE__: The UART handle
   *
   * @retval The UART Data register address
   */
 #define __HAL_LPPUART_UART_GET_TX_DATA_REGISTER_ADDRESS(__HANDLE__)   ((uint32_t)&(__HANDLE__)->Instance->TDR)

/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /*__STM32F4XX_HAL_LPPUART_UART_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
