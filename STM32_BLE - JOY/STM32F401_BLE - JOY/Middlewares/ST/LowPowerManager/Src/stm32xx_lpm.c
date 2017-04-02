/**
  ******************************************************************************
  * @file    stm32xx_lpm.c
  * @author  MCD Application Team
  * @version V1.0
  * @date    14-April-2014
  * @cond DOXYGEN_EXCLUDE
  * @brief   Low Power Manager
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  * @endcond
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32xx_lpm.h"
#include "stm32xx_lpm_conf.h"

/** @defgroup Middlewares
 *  @brief Contains all platform independent modules (eg. Low Power Manager, Timer Server, HCI stack, ...).
 *  @{
 */

/** @defgroup ST
 *  @{
 */
 
/** @defgroup LowPowerManager
 *  @{
 */

/** @defgroup STM32XX_LPM 
 * @{
 */
 
/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/** @defgroup STM32XX_LPM_Private_Defines 
 * @{
 */
/* Private defines -----------------------------------------------------------*/

/**
 * Masks to be used with the LPM_Mode_Request() API to read out the correct bit
 */
#define	LPM_DEEPSLEEP_MASK	0x04
#define	LPM_LPSDSR_MASK		0x02
#define	LPM_PDDS_MASK		0x01

/**
 * @}
 */
 
/* Private macros ------------------------------------------------------------*/

/** @defgroup STM32XX_LPM_Private_Variables
 * @{
 */
/* Private variables ---------------------------------------------------------*/
static uint32_t uwDeepSleep_Req = 0;	/**< Default enabled - Control the CortexM3 DeepSleep bit */
static uint32_t uwLPSDSR_Req = 0;		/**< Default enabled - Control the LDO Eco Mode */
static uint32_t uwPDDS_Req = 0;			/**< Default enabled - Control the StandBy Mode */
static uint32_t uwRUN_Req = 0;			/**< Default disabled - RUN mode NOT requested */

/**
 * @}
 */
 
/* Private function prototypes -----------------------------------------------*/

/** @defgroup STM32XX_LPM_Public_Function_Prototypes
 * @{
 */
/* Public function prototypes -----------------------------------------------*/
WEAK void LPM_ExitStopMode(void);
WEAK void LPM_EnterStandbyMode(void);
WEAK void LPM_ExitStandbyMode(void);

/**
 * @}
 */
 
/* Private functions ---------------------------------------------------------*/

/** @defgroup STM32XX_LPM_Public_Functions
 * @{
 */
/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Interface to the user to request a specific low power mode
  *
  * @note
  *
  * @param  eId: process Id
  *
  * @param  eMode: mode requested (RUN, STOP, SLEEP, ...)
  *
  * @retval None
  */
void LPM_Mode_Request(eLPM_Id eId, eLPM_Mode eMode)
{
	uint32_t uwPRIMASK_Bit;

	uwPRIMASK_Bit = __get_PRIMASK();	/**< backup PRIMASK bit */
	__disable_irq();			/**< Disable all interrupts by setting PRIMASK bit on Cortex*/

	if(eMode == eLPM_Mode_RUN)		/**< RUN Mode request Management */
	{
      uwRUN_Req |= (uint32_t)(1<<eId);	/**< RUN mode enabled - set bit */
	}
	else
	{
        uwRUN_Req &= ~((uint32_t)(1<<eId));	/**< RUN mode Disable - clear bit */

		if(eMode&LPM_DEEPSLEEP_MASK)	        /**< Register DeepSleep bit request */
		{
          uwDeepSleep_Req &= ~((uint32_t)(1<<eId));	/**< DeepSleep enabled - clear bit */
		}
		else
		{
          uwDeepSleep_Req |= (uint32_t)(1<<eId);
		}

		if(eMode&LPM_LPSDSR_MASK)	/**< Register LPSDSR bit request */
		{
          uwLPSDSR_Req &= ~((uint32_t)(1<<eId));	/**< LDO Eco Mode enabled - clear bit */
		}
		else
		{
          uwLPSDSR_Req |= (uint32_t)(1<<eId);
		}

		if(eMode&LPM_PDDS_MASK)		/**< Register PDDS bit request */
		{
          uwPDDS_Req &= ~((uint32_t)(1<<eId));	/**< StandBy Mode enabled - clear bit */
		}
		else
		{
          uwPDDS_Req |= (uint32_t)(1<<eId);
		}

        uwRUN_Req &= ~((uint32_t)(1<<eId));	/**< RUN mode disabled - clear bit */
	}

	__set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/

	return;
}

/**
  * @brief  Execute the low power mode configuration.
  *
  * @note   This function configures the MCU HW according to the lowest power
  *         mode requested by all FW modules.
  *	        This function shall not be interrupted if executed in critical
  *         section using PRIMASK on the Cortex.
  *
  * @param  None
  *
  * @retval None
  */
void LPM_Enter_Mode(void)
{
	uint32_t uwPRIMASK_Bit;

	uwPRIMASK_Bit = __get_PRIMASK();	/**< backup PRIMASK bit */

	__disable_irq();			/**< Disable all interrupts by setting PRIMASK bit on Cortex*/
        
	if(uwRUN_Req == 0)
	{
		if(uwDeepSleep_Req)
		{
			/**
			 * Enter Sleep Mode
			 */
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		}
		else if (uwPDDS_Req == 0) /* WARNING: This mode is not supported yet by any platform */
		{
			LPM_EnterStandbyMode();	/**< Call user Hook */

			/**
			 * Clear Wakeup flag
			 */
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

			/**
			 * Enter Standby Mode
			 */
			HAL_PWR_EnterSTANDBYMode();

			LPM_ExitStandbyMode();	/**< Call user Hook */
		}
		else
		{
			/**
			 * Clear Wakeup flag
			 */
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

			/**
			 * Enter Stop Mode
			 */
			if(uwLPSDSR_Req)
			{
				HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
			}
			else
			{
#ifdef USE_STM32L4XX_NUCLEO      
                                HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
#else
                                HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
#endif
			}

			LPM_ExitStopMode();	/**< Call user Hook */
		}
	}

	__set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/

    return;
}

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
   
/**
 * @}
 */
 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
