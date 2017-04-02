/**
  ******************************************************************************
  * @file    stm32xx_hal_timerserver_rtc.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    01-October-2014
  * @brief   Header for stm32xx_timerserver.c module
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
#ifndef __STM32XX_HAL_TIMERSERVER_RTC_H
#define __STM32XX_HAL_TIMERSERVER_RTC_H

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup Middlewares
 *  @{
 */

/** @addtogroup ST
 *  @{
 */
 
/** @addtogroup TimerServer
 *  @{
 */

/** @defgroup STM32XX_HAL_TIMERSERVER_RTC
 *  @{
 */
 
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup STM32XX_HAL_TIMERSERVER_RTC_Exported_Macros
 *  @{
 */
/* Exported macros -----------------------------------------------------------*/
/**
  * @brief  Read if the RTC WakeUp Timer peripheral is enabled.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */ 
#define __HAL_TIMERSERVER_RTC_GET_WAKEUPTIMER_STATUS(__HANDLE__)	(((((__HANDLE__)->Instance->CR) & (RTC_CR_WUTE)) != RESET)? SET : RESET)

 /**
   * @brief  Write the RTC Wakeup counter.
   * @param  __HANDLE__:	specifies the RTC handle.
   * @param  __COUNTER__: 	Value of the counter to be written.
   * @retval None
   */
#define __HAL_TIMERSERVER_RTC_SET_WAKEUPCOUNTER(__HANDLE__, __COUNTER__)		((__HANDLE__)->Instance->WUTR = (__COUNTER__))

 /**
   * @brief  Read the SSR register
   * @param  __HANDLE__:	specifies the RTC handle.
   * @retval SSR register value
   */
 #define __HAL_TIMERSERVER_RTC_GET_SSR(__HANDLE__)		((__HANDLE__)->Instance->SSR)

 /**
   * @brief  Read the WUCKSEL Configuration
   * @param  __HANDLE__:	specifies the RTC handle.
   * @retval WUCKSEL value
   */
 #define __HAL_TIMERSERVER_RTC_GET_WUCKSEL(__HANDLE__)		(((__HANDLE__)->Instance->CR) & RTC_CR_WUCKSEL)

 /**
   * @brief  Read the PREDIV_A Configuration
   * @param  __HANDLE__:	specifies the RTC handle.
   * @retval PREDIV_A value
   */
 #define __HAL_TIMERSERVER_RTC_GET_PREDIVA(__HANDLE__)		((((__HANDLE__)->Instance->PRER) & RTC_PRER_PREDIV_A)>>16)

 /**
   * @brief  Read the PREDIV_S Configuration
   * @param  __HANDLE__:	specifies the RTC handle.
   * @retval PREDIV_S value
   */
#ifndef CMSIS_RTC_GET_PREDIVS_MASK_CORRECTED
  #define __HAL_TIMERSERVER_RTC_GET_PREDIVS(__HANDLE__) (((__HANDLE__)->Instance->PRER) & (0x7FFF))
#else
  #define __HAL_TIMERSERVER_RTC_GET_PREDIVS(__HANDLE__) (((__HANDLE__)->Instance->PRER) & RTC_PRER_PREDIV_S)
#endif



/**
 * @}
 */
 
/* Exported functions ------------------------------------------------------- */

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */
 
#ifdef __cplusplus
}
#endif

#endif /*__STM32XX_HAL_TIMERSERVER_RTC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
