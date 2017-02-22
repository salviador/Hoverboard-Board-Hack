#include "stm32f1xx_hal.h"
#include <stdint.h>
#include "delay.h"

TIM_HandleTypeDef TimHandle;

// Init timer for Microseconds delays
void _init_us() {
        __HAL_RCC_TIM2_CLK_ENABLE();
        TimHandle.Instance = TIM2;
        TimHandle.Init.Period = UINT16_MAX;
        TimHandle.Init.Prescaler = (SystemCoreClock/DELAY_TIM_FREQUENCY_US)-1;;
        TimHandle.Init.ClockDivision = 0;
        TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;        
        HAL_TIM_Base_Init(&TimHandle);
          
        HAL_TIM_Base_Start(&TimHandle);
}

// Init and start timer for Milliseconds delays
void _init_ms() {
        __HAL_RCC_TIM2_CLK_ENABLE();
        TimHandle.Instance = TIM2;
        TimHandle.Init.Period = UINT16_MAX;
        TimHandle.Init.Prescaler = (SystemCoreClock/DELAY_TIM_FREQUENCY_MS)-1;;
        TimHandle.Init.ClockDivision = 0;
        TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;        
        HAL_TIM_Base_Init(&TimHandle);

        HAL_TIM_Base_Start(&TimHandle);       
}


// Stop timer
void _stop_timer() {
	HAL_TIM_Base_Stop(&TimHandle);
        __HAL_RCC_TIM2_CLK_DISABLE();
}

// Do delay for nTime milliseconds
void Delay_ms(uint32_t mSecs) {
	// Init and start timer
	_init_ms();

	// Dummy loop with 16 bit count wrap around
	uint32_t start = TIM2->CNT;
	while((TIM2->CNT-start) <= mSecs);

	// Stop timer
	_stop_timer();
}

// Do delay for nTime microseconds
void Delay_us(uint32_t uSecs) {
	// Init and start timer
	_init_us();

	// Dummy loop with 16 bit count wrap around
	uint32_t start = TIM2->CNT;
	while((TIM2->CNT-start) <= uSecs);

	// Stop timer
	_stop_timer();
}

