/**
  ******************************************************************************
  * @file    stm32xx_hal_timerserver_exti.h
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
#ifndef __STM32XX_HAL_TIMERSERVER_EXTI_H
#define __STM32XX_HAL_TIMERSERVER_EXTI_H

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

/** @defgroup STM32XX_HAL_TIMERSERVER_EXTI
 *  @{
 */
 
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup STM32XX_HAL_TIMERSERVER_EXTI_Exported_Macros
 *  @{
 */
/* Exported macros -----------------------------------------------------------*/
 /**
   * @brief  Set Rising edge in EXTI for Wakeup Timer.
   * @param  None
   * @retval None
   */
#ifdef STM32L053xx
 #define __HAL_TIMERSERVER_EXTI_RTC_SET_WAKEUPTIMER_RISING_EDGE()   (EXTI->RTSR |= (RTC_EXTI_LINE_WAKEUPTIMER_EVENT))
#endif
 
#ifdef STM32L476xx
 #define __HAL_TIMERSERVER_EXTI_RTC_SET_WAKEUPTIMER_RISING_EDGE()   (EXTI->RTSR1 |= (RTC_EXTI_LINE_WAKEUPTIMER_EVENT))
#endif

#ifdef STM32F401xE
 #define __HAL_TIMERSERVER_EXTI_RTC_SET_WAKEUPTIMER_RISING_EDGE()   (EXTI->RTSR |= (RTC_EXTI_LINE_WAKEUPTIMER_EVENT))
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

#endif /*__STM32XX_HAL_TIMERSERVER_EXTI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
