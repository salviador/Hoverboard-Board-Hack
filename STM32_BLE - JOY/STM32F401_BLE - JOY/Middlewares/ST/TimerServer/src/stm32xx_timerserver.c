/**
  ******************************************************************************
  * @file    stm32xx_timerserver.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-November-2014
  * @brief   Timer server
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
#include "stm32xx_timerserver.h"
#include "stm32xx_timerserver_conf.h"

/** @addtogroup Middlewares
 *  @{
 */

/** @addtogroup ST
 *  @{
 */
 
/** @defgroup TimerServer
 *  @{
 */

/** @defgroup STM32XX_TIMERSERVER 
 * @{
 */
 
/* External variables --------------------------------------------------------*/

/** @defgroup STM32XX_TIMERSERVER_Private_Types
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
typedef enum{
  eTimerID_Free,
  eTimerID_Created,
  eTimerID_Running
}eTimerStatus_t;

typedef enum{
  eSSR_reset,
  eSSR_kept
}eRequestResetSSR_t;

typedef enum{
  eWakeupTimerValue_Overpassed,
  eWakeupTimerValue_LargeEnough
}eWakeupTimerLimitation_Status_t;

typedef struct{
  pf_TIMER_TimerCallBack_t pfTimerCallBack;
  uint32_t			       CounterInit;
  uint32_t			       CountLeft;
  eTimerStatus_t	       eTimerStatus;
  eTIMER_TimerMode_t       eTimerMode;
  eTimerProcessID_t        eTimerProcessID;
  uint8_t                  ubPreviousID;
  uint8_t                  ubNextID;
}sTimerContext_t;

/**
 * @}
 */

/* Private macros ------------------------------------------------------------*/

/** @defgroup STM32XX_TIMERSERVER_Private_Defines
  * @{
  */
/* Private defines -----------------------------------------------------------*/
#define SSR_RESET	0xFFFF

/**
 * @}
 */

/** @defgroup STM32XX_TIMERSERVER_Private_Variables
  * @{
  */
/* Private variables ---------------------------------------------------------*/
static volatile sTimerContext_t aTimerContext[MAX_NBR_CONCURRENT_TIMER];
static volatile uint8_t ubCurrentRunningTimerID;
static volatile uint8_t PreviousRunningTimerID;
static RTC_HandleTypeDef *phrtc;	/**< RTC handle */
static volatile uint16_t	SSRValueOnLastSetup;
static volatile eWakeupTimerLimitation_Status_t	WakeupTimerLimitation;

static uint8_t	WakeupTimerPrescaler;
static uint8_t	AsynchPrescalerUserConfig;
static uint16_t SynchPrescalerUserConfig;
static volatile uint16_t MaxWakeupTimerSetup;

/**
 * @}
 */

/** @defgroup STM32XX_TIMERSERVER_Private_Function_Prototypes
  * @{
  */
/* Private function prototypes -----------------------------------------------*/
static void RestartWakeupCounter(uint16_t Value);
static uint16_t ReadSSR(void);
static uint16_t ReturnTimeElapsed(void);
static void RescheduleTimerList(void);
static void UnlinkTimer(uint8_t TimerID, eRequestResetSSR_t eRequestResetSSR);
static void linkTimerBefore(uint8_t TimerID, uint8_t RefTimerID);
static void linkTimerAfter(uint8_t TimerID, uint8_t RefTimerID);

/**
 * @}
 */
 
/** @defgroup STM32XX_TIMERSERVER_Private_Functions
  * @{
  */
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Insert a Timer in the list after the Id Specified
  *
  * @note
  *
  * @param  TimerID: Timer Id
  *
  * @param  RefTimerID
  *
  * @retval None
  */
static void linkTimerAfter(uint8_t TimerID, uint8_t RefTimerID)
{
	uint8_t NextId;

	NextId = aTimerContext[RefTimerID].ubNextID;

	if(NextId != MAX_NBR_CONCURRENT_TIMER)
	{
		aTimerContext[NextId].ubPreviousID = TimerID;
	}
	aTimerContext[TimerID].ubNextID = NextId;
	aTimerContext[TimerID].ubPreviousID = RefTimerID ;
	aTimerContext[RefTimerID].ubNextID = TimerID;

	return;
}

/**
  * @brief  Insert a Timer in the list before the Id Specified
  *
  * @note
  *
  * @param  TimerID: Timer Id
  *
  * @param  RefTimerID
  *
  * @retval None
  */
static void linkTimerBefore(uint8_t TimerID, uint8_t RefTimerID)
{
	uint8_t PreviousId;

	if(RefTimerID != ubCurrentRunningTimerID)
	{
		PreviousId = aTimerContext[RefTimerID].ubPreviousID;

		aTimerContext[PreviousId].ubNextID = TimerID;
		aTimerContext[TimerID].ubNextID = RefTimerID;
		aTimerContext[TimerID].ubPreviousID = PreviousId ;
		aTimerContext[RefTimerID].ubPreviousID = TimerID;
	}
	else
	{
		aTimerContext[TimerID].ubNextID = RefTimerID;
		aTimerContext[RefTimerID].ubPreviousID = TimerID;
	}

	return;
}

/**
  * @brief  Insert a Timer in the list
  *
  * @note
  *
  * @param  TimerID: Timer Id
  *
  * @retval None
  */
static uint16_t linkTimer(uint8_t TimerID)
{
  uint32_t TimeLeft;
  uint16_t TimeElapsed;
  uint8_t ubTimerIDLookUp;
  uint8_t NextId;
  
  if(ubCurrentRunningTimerID == MAX_NBR_CONCURRENT_TIMER)
  {
    /**
    * No timer in the list
    */
    PreviousRunningTimerID = ubCurrentRunningTimerID;
    ubCurrentRunningTimerID = TimerID;
    aTimerContext[TimerID].ubNextID = MAX_NBR_CONCURRENT_TIMER;
    
    SSRValueOnLastSetup = SSR_RESET;
	TimeElapsed = 0;
  }
  else
  {
    TimeElapsed = ReturnTimeElapsed();
    
    /**
    * update count of the timer to be linked
    */
    aTimerContext[TimerID].CountLeft += TimeElapsed;
    TimeLeft = aTimerContext[TimerID].CountLeft;
    
    /**
    * Search for index where the new timer shall be linked
    */
    if(aTimerContext[ubCurrentRunningTimerID].CountLeft <= TimeLeft)
    {
      /**
      * Search for the ID after the first one
      */
      ubTimerIDLookUp = ubCurrentRunningTimerID;
      NextId = aTimerContext[ubTimerIDLookUp].ubNextID;
      while((NextId != MAX_NBR_CONCURRENT_TIMER) && (aTimerContext[NextId].CountLeft <= TimeLeft))
      {
        ubTimerIDLookUp = aTimerContext[ubTimerIDLookUp].ubNextID;
        NextId = aTimerContext[ubTimerIDLookUp].ubNextID;
      }
      
      /**
      * Link after the ID
      */
      linkTimerAfter(TimerID, ubTimerIDLookUp);
    }
    else
    {
      /**
      * Link before the first ID
      */
      linkTimerBefore(TimerID, ubCurrentRunningTimerID);
      PreviousRunningTimerID = ubCurrentRunningTimerID;
      ubCurrentRunningTimerID = TimerID;
    }
  }
  
  return TimeElapsed;
}
    
/**
  * @brief  Remove a Timer from the list
  *
  * @note
  *
  * @param  TimerID: Timer Id
  *
  * @param  eRequestResetSSR
  *
  * @retval None
  */
static void UnlinkTimer(uint8_t TimerID, eRequestResetSSR_t eRequestResetSSR)
{
	uint8_t PreviousId;
	uint8_t NextId;

	if(TimerID == ubCurrentRunningTimerID)
	{
		PreviousRunningTimerID = ubCurrentRunningTimerID;
		ubCurrentRunningTimerID = aTimerContext[TimerID].ubNextID;
	}
	else
	{
		PreviousId = aTimerContext[TimerID].ubPreviousID;
		NextId = aTimerContext[TimerID].ubNextID;

		aTimerContext[PreviousId].ubNextID = aTimerContext[TimerID].ubNextID;
		if(NextId != MAX_NBR_CONCURRENT_TIMER)
		{
			aTimerContext[NextId].ubPreviousID = aTimerContext[TimerID].ubPreviousID;
		}
	}

	/**
	 * Timer is out of the list
	 */
	aTimerContext[TimerID].eTimerStatus = eTimerID_Created;

	if((ubCurrentRunningTimerID == MAX_NBR_CONCURRENT_TIMER) && (eRequestResetSSR == eSSR_reset))
	{
		SSRValueOnLastSetup = SSR_RESET;
	}

	return;
}

/**
  * @brief  Read the SSR register
  *
  * @note
  *
  * @param  None
  *
  * @retval uint16_t: SSR Value
  */
static uint16_t ReadSSR(void)
{
	uint32_t	ReturnValue;

	ReturnValue = SSR_RESET;

	/*
	 * It needs to be checked the reading of the SSR register is stable
	 */
	while (ReturnValue != __HAL_TIMERSERVER_RTC_GET_SSR(phrtc))
	{
		ReturnValue = __HAL_TIMERSERVER_RTC_GET_SSR(phrtc);
	}

	return (uint16_t)ReturnValue;
}

/**
  * @brief  Return the number of ticks expired since the startup of the counter
  *
  * @note	The API is reading the SSR register to get how many ticks have been counted
  * 		since the time the timer has been stared
  *
  * @param  None
  *
  * @retval uint16_t: Time expired in Ticks
  */
static uint16_t ReturnTimeElapsed(void)
{
	uint32_t	ReturnValue;
        uint32_t	WrapCounter;

	if(SSRValueOnLastSetup != SSR_RESET)
	{
		ReturnValue = ReadSSR();	/**< Read SSR register first */

		if (SSRValueOnLastSetup >= ReturnValue)
		{
			ReturnValue = SSRValueOnLastSetup - ReturnValue;
		}
		else
		{
			WrapCounter = (SynchPrescalerUserConfig - ReturnValue) + 1;
			ReturnValue = SSRValueOnLastSetup + WrapCounter;
		}

		/**
		 * At this stage, ReturnValue holds the number of ticks counted by SSR
		 * Need to translate in number of ticks counted by the Wakeuptimer
		 */
		ReturnValue = ReturnValue*AsynchPrescalerUserConfig;
		ReturnValue = ReturnValue >> WakeupTimerPrescaler;
	}
	else
	{
		ReturnValue = 0;
	}

	return (uint16_t)ReturnValue;
}

/**
  * @brief  Set the wakeup counter
  *
  * @note	The API is writing the counter value so that :
  * 		1) The value is decreased by one to cope with the fact the interrupt is generated with 1 extra clock cycle (See RefManuel)
  * 		2) It checks the minimum value written is '1' (to cope with the case it is forbidden to write '0' when WUCKSEL[2:0] = 3
  *
  * 		It assumes all condition are met to be allowed to write the wakeup counter
  *
  * @param  Value: Value to be written in the counter
  *
  * @retval None
  */
static void RestartWakeupCounter(uint16_t Value)
{
	/**
	 * The wakeuptimer has been disabled in the calling function to reduce the time to poll the WUTWF
	 * FLAG when the new value will have to be written
	 * 	__HAL_RTC_WAKEUPTIMER_DISABLE(phrtc);
	 */

	if(Value == 0)
	{

	SSRValueOnLastSetup = ReadSSR();

		/**
		 * Simulate that the Timer expired
		 */
		HAL_NVIC_SetPendingIRQ(NVIC_RTC_WAKEUP_HANDLER_ID);
	}
	else
	{
		if((Value > 1) ||(WakeupTimerPrescaler != 1))
		{
			Value -= 1;
		}

		while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(phrtc, RTC_FLAG_WUTWF) == RESET);		
		__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(phrtc, RTC_FLAG_WUTF);		/**<  Clear flag in RTC module */
		__HAL_RTC_EXTI_CLEAR_FLAG(RTC_EXTI_LINE_WAKEUPTIMER_EVENT);	/**<  Clear flag in EXTI module */
		HAL_NVIC_ClearPendingIRQ(NVIC_RTC_WAKEUP_HANDLER_ID);		/**<  Clear pending bit in NVIC */
		__HAL_TIMERSERVER_RTC_SET_WAKEUPCOUNTER(phrtc, Value);

        /**
		 * Update the value here after the WUTWF polling that may take some time
		 */
		SSRValueOnLastSetup = ReadSSR();

		__HAL_RTC_WAKEUPTIMER_ENABLE(phrtc);		/**<  Enable the Wakeup Timer */
	}

	return ;
}

/**
  * @brief  Reschedule the list of timer
  *
  * @note	1) Update the count left for each timer in the list
  * 		2) Setup the wakeuptimer
  *
  * @param  None
  *
  * @retval None
  */
static void RescheduleTimerList(void)
{
	uint8_t		localTimerID;
	uint32_t	timecountleft;
	uint16_t	WakeupTimerValue;
	uint16_t TimeElapsed;


	/**
	 * The wakeuptimer is disabled now to reduce the time to poll the WUTWF
	 * FLAG when the new value will have to be written
	 */
	if(__HAL_TIMERSERVER_RTC_GET_WAKEUPTIMER_STATUS(phrtc) == SET)
	{
		/**
		 * Wait for the flag to be back to 0 when the wakeup timer is enabled
		 */
		while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(phrtc, RTC_FLAG_WUTWF) == SET);
	}
	__HAL_RTC_WAKEUPTIMER_DISABLE(phrtc);		/**<  Disable the Wakeup Timer */

	localTimerID = ubCurrentRunningTimerID;

	/**
	 * Calculate what will be the value to write in the wakeuptimer
	 */
	timecountleft = aTimerContext[localTimerID].CountLeft;

	
	TimeElapsed = ReturnTimeElapsed();
	if(timecountleft < TimeElapsed )
	{
		/**
		 * There is no tick left to count
		 */
		WakeupTimerValue = 0;
		WakeupTimerLimitation = eWakeupTimerValue_LargeEnough;
	}
	else
	{
		if(timecountleft > (TimeElapsed + MaxWakeupTimerSetup))
		{
			/**
			 * The number of tick left is greater than the Wakeuptimer maximum value
			 */
			WakeupTimerValue = MaxWakeupTimerSetup;

			WakeupTimerLimitation = eWakeupTimerValue_Overpassed;
		}
		else
		{
			WakeupTimerValue = timecountleft - TimeElapsed;
			WakeupTimerLimitation = eWakeupTimerValue_LargeEnough;
		}

	}

	/**
	 * update ticks left to be counted for each timer
	 */
	while(localTimerID != MAX_NBR_CONCURRENT_TIMER)
	{
		if (aTimerContext[localTimerID].CountLeft < TimeElapsed)
		{
			aTimerContext[localTimerID].CountLeft = 0;
		}
		else
		{
			aTimerContext[localTimerID].CountLeft -= TimeElapsed;
		}
		localTimerID = aTimerContext[localTimerID].ubNextID;
	}

	/**
	 * Write next count
	 */
	RestartWakeupCounter(WakeupTimerValue);

	return ;
}


/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Schedule the timer list on the timer interrupt handler
  *
  * @note
  *
  * @param  None
  *
  * @retval None
  */
void TIMER_RTC_Wakeup_Handler(void)
{
  pf_TIMER_TimerCallBack_t pfTimerCallBack;
  eTimerProcessID_t eTimerProcessID;
  uint8_t	localCurrentRunningTimerID;
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
  uint32_t uwPRIMASK_Bit;
#endif
  
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
  uwPRIMASK_Bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/
#endif
  
  /**
  * Disable the Wakeup Timer
  */
  __HAL_RTC_WAKEUPTIMER_DISABLE(phrtc);
  
  /**
  * Clear WUTF flag in RTC
  */
  __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(phrtc, RTC_FLAG_WUTF);
  
  /**
  * Clear flag in EXTI module
  */
  __HAL_RTC_EXTI_CLEAR_FLAG(RTC_EXTI_LINE_WAKEUPTIMER_EVENT);
  
  localCurrentRunningTimerID = ubCurrentRunningTimerID;
  
  if(aTimerContext[localCurrentRunningTimerID].eTimerStatus == eTimerID_Running)
  {
    pfTimerCallBack = aTimerContext[localCurrentRunningTimerID].pfTimerCallBack;
    eTimerProcessID = aTimerContext[localCurrentRunningTimerID].eTimerProcessID;
    
    /**
    * It should be good to check whether the TimeElapsed is greater or not than the tick left to be counted
    * However, due to the inaccuracy of the reading of the time elapsed, it may return there is 1 tick
    * to be left whereas the count is over
    * A more secure implementation has been done with a flag to state whereas the full count has been written
    * in the wakeuptimer or not
    */
    if(WakeupTimerLimitation != eWakeupTimerValue_Overpassed)
    {
      if(aTimerContext[localCurrentRunningTimerID].eTimerMode == eTimerMode_Repeated)
      {
        UnlinkTimer(localCurrentRunningTimerID, eSSR_kept);
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
        __set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/
#endif
        TIMER_Start(localCurrentRunningTimerID, aTimerContext[localCurrentRunningTimerID].CounterInit);
      }
      else
      {
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
        __set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/
#endif
        TIMER_Stop(localCurrentRunningTimerID);
      }
      
      TIMER_Notification(eTimerProcessID, localCurrentRunningTimerID, pfTimerCallBack);
    }
    else
    {
      RescheduleTimerList();
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
      __set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/
#endif
    }
  }
  else
  {
    
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
    __set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/
#endif
    
  }
  
  return;
}

/**
  * @brief  Initialize the timer server
  *
  * @note
  *
  * @param  hrtc: RTC Handle
  *
  * @retval None
  */
void TIMER_Init(RTC_HandleTypeDef *hrtc)
{
  uint8_t	ubLoop;
  uint32_t localmaxwakeuptimersetup;
  
  /**
  * Get RTC handler
  */
  phrtc = hrtc;
    
  /**
  * Readout the user config
  */
  WakeupTimerPrescaler = (4 - (__HAL_TIMERSERVER_RTC_GET_WUCKSEL(phrtc)));
  AsynchPrescalerUserConfig = (uint8_t)__HAL_TIMERSERVER_RTC_GET_PREDIVA(phrtc) + 1;
  SynchPrescalerUserConfig = (uint16_t)__HAL_TIMERSERVER_RTC_GET_PREDIVS(phrtc);
  
  /**
  *  Margin is taken to avoid wrong calculation when the wrap around is there and some
  *  application interrupts may have delayed the reading
  */
  localmaxwakeuptimersetup = (((SynchPrescalerUserConfig - 1)*AsynchPrescalerUserConfig) >> WakeupTimerPrescaler) - RTC_HANDLER_MAX_DELAY;
  
	if(localmaxwakeuptimersetup >= 0xFFFF)
	{
		MaxWakeupTimerSetup = 0xFFFF;
	}
	else
	{
		MaxWakeupTimerSetup = (uint16_t)localmaxwakeuptimersetup;
	}
  WakeupTimerLimitation = eWakeupTimerValue_LargeEnough;
  SSRValueOnLastSetup = SSR_RESET;
  
  /**
  * Initialize the timer server
  */
  for(ubLoop = 0; ubLoop < MAX_NBR_CONCURRENT_TIMER; ubLoop++)
  {
    aTimerContext[ubLoop].eTimerStatus = eTimerID_Free;
  }

  ubCurrentRunningTimerID = MAX_NBR_CONCURRENT_TIMER;		/**<  Set ID to non valid value */
  
  /**
  * Reinit everything for the scope of this projet
  */
  __HAL_RTC_WAKEUPTIMER_DISABLE(phrtc);						/**<  Disable the Wakeup Timer */
  __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(phrtc, RTC_FLAG_WUTF);		/**<  Clear flag in RTC module */
  __HAL_RTC_EXTI_CLEAR_FLAG(RTC_EXTI_LINE_WAKEUPTIMER_EVENT);	/**<  Clear flag in EXTI module  */
  HAL_NVIC_ClearPendingIRQ(NVIC_RTC_WAKEUP_HANDLER_ID);			/**<  Clear pending bit in NVIC  */
  __HAL_RTC_WAKEUPTIMER_ENABLE_IT(phrtc, RTC_IT_WUT);			/**<  Enable interrupt in RTC module  */
  
  /**
  * Configure EXTI module
  */
  __HAL_TIMERSERVER_EXTI_RTC_SET_WAKEUPTIMER_RISING_EDGE();
  __HAL_RTC_EXTI_ENABLE_IT(RTC_EXTI_LINE_WAKEUPTIMER_EVENT);
    
  HAL_NVIC_SetPriority(NVIC_RTC_WAKEUP_HANDLER_ID, NVIC_UART_RTC_WAKEUP_IT_PREEMPTPRIORITY, NVIC_UART_RTC_WAKEUP_IT_SUBPRIORITY);		/**<  Set NVIC priority */
  
  return;
}

/**
  * @brief  Interface to create a virtual timer
  *
  * @note
  *
  * @param  eTimerProcessID: This is an identifier provided by the user and returned in the callback to allow
  * 				identification of the requester
  *
  * @param  pTimerId: Timer Id returned to the user to request operation (start, stop, delete)
  *
  * @param  eTimerMode: Mode of the virtual timer (Single shot or repeated)
  *
  * @param  pftimeout_handler:	Callback when the virtual timer expires
  *
  * @retval eReturnStatus_t 
  */
eReturnStatus_t TIMER_Create(eTimerProcessID_t eTimerProcessID, uint8_t *pTimerId, eTIMER_TimerMode_t eTimerMode, pf_TIMER_TimerCallBack_t pftimeout_handler)
{
  eReturnStatus_t localreturnstatus;
  uint8_t	  ubLoop = 0;
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
  uint32_t    uwPRIMASK_Bit;
#endif
  
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
  uwPRIMASK_Bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/
#endif
  
  while((ubLoop < MAX_NBR_CONCURRENT_TIMER) && (aTimerContext[ubLoop].eTimerStatus != eTimerID_Free))
  {
    ubLoop++;
  }
  
  if(ubLoop != MAX_NBR_CONCURRENT_TIMER)
  {
    aTimerContext[ubLoop].eTimerStatus = eTimerID_Created;
    
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
    __set_PRIMASK(uwPRIMASK_Bit);  /**< Restore PRIMASK bit*/
#endif
    
    aTimerContext[ubLoop].eTimerProcessID = eTimerProcessID;
    aTimerContext[ubLoop].eTimerMode = eTimerMode;
    aTimerContext[ubLoop].pfTimerCallBack = pftimeout_handler;
    *pTimerId = ubLoop;
    
    localreturnstatus = eTimerID_Successful;
  }
  else
  {
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
    __set_PRIMASK(uwPRIMASK_Bit);  /**< Restore PRIMASK bit*/
#endif
    
    localreturnstatus = eTimerID_Failed;
  }
  
  return(localreturnstatus);
}

/**
  * @brief  Delete a virtual timer from the list
  *
  * @note   If the virtual timer is running, it is stopped and deleted.
  *
  * @param  ubTimerID:	Id of the timer to remove from the list
  *
  * @retval None
  */
void TIMER_Delete(uint8_t ubTimerID)
{
  TIMER_Stop(ubTimerID);
  
  aTimerContext[ubTimerID].eTimerStatus = eTimerID_Free;  /**<  release ID */
  
  return;
}

/**
  * @brief  Stop a virtual timer
  *
  * @note   A virtual timer that has been stopped may be restarted
  *
  * @param  ubTimerID:	Id of the timer to stop
  *
  * @retval None
  */
void TIMER_Stop(uint8_t ubTimerID)
{
  uint8_t localcurrentrunningtimerid;
  
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
  uint32_t uwPRIMASK_Bit;
#endif
  
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
  uwPRIMASK_Bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/
#endif
  
  HAL_NVIC_DisableIRQ(NVIC_RTC_WAKEUP_HANDLER_ID);  /**<  Disable NVIC */
  
  if(aTimerContext[ubTimerID].eTimerStatus == eTimerID_Running)
  {
    UnlinkTimer(ubTimerID, eSSR_reset);
    localcurrentrunningtimerid = ubCurrentRunningTimerID;
    if((PreviousRunningTimerID != localcurrentrunningtimerid) && (localcurrentrunningtimerid != MAX_NBR_CONCURRENT_TIMER))
    {
      RescheduleTimerList();
    }
  }
  
  HAL_NVIC_EnableIRQ(NVIC_RTC_WAKEUP_HANDLER_ID);  /**<  Enable NVIC */
  
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
  __set_PRIMASK(uwPRIMASK_Bit);	 /**< Restore PRIMASK bit*/
#endif
  
  return;
}

/**
  * @brief  Start a virtual timer
  *
  * @note   The value of the ticks is depending on the user RTC clock input configuration
  *
  * @param  ubTimerID: Id of the timer to star
  *
  * @param  timeout_ticks: Number of ticks of the virtual timer
  *
  * @retval None
  */
void TIMER_Start(uint8_t ubTimerID, uint32_t timeout_ticks)
{
  uint16_t time_elapsed;
  uint8_t localcurrentrunningtimerid;
  
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
  uint32_t uwPRIMASK_Bit;
#endif
  
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
  uwPRIMASK_Bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/
#endif
  
  HAL_NVIC_DisableIRQ(NVIC_RTC_WAKEUP_HANDLER_ID);  /**<  Disable NVIC */
  
  aTimerContext[ubTimerID].eTimerStatus = eTimerID_Running;
  
  aTimerContext[ubTimerID].CountLeft = timeout_ticks;
  aTimerContext[ubTimerID].CounterInit = timeout_ticks;
  
  time_elapsed =  linkTimer(ubTimerID);
  
  localcurrentrunningtimerid = ubCurrentRunningTimerID;
  
  if(PreviousRunningTimerID != localcurrentrunningtimerid)
  {
    RescheduleTimerList();
  }
  else
  {
    aTimerContext[ubTimerID].CountLeft -= time_elapsed;
  }
  
  HAL_NVIC_EnableIRQ(NVIC_RTC_WAKEUP_HANDLER_ID);  /**<  Enable NVIC */
  
#ifdef TIMER_USE_PRIMASK_AS_CRITICAL_SECTION
  __set_PRIMASK(uwPRIMASK_Bit);  /**< Restore PRIMASK bit*/
#endif
  
  return;
}

/**
  * @brief  Timer notification
  * @param  eTimerProcessID: Process ID
  * @param  ubTimerID: Timer ID
  * @param  pfTimerCallBack: Timer Callback
  * @retval None
  */
__weak void TIMER_Notification(eTimerProcessID_t eTimerProcessID, uint8_t ubTimerID, pf_TIMER_TimerCallBack_t pfTimerCallBack)
{
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
