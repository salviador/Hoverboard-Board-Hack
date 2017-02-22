#ifndef __DELAY_H
#define __DELAY_H

#define DELAY_TIM_FREQUENCY_US 1000000		/* = 1MHZ -> timer runs in microseconds */
#define DELAY_TIM_FREQUENCY_MS 1000			/* = 1kHZ -> timer runs in milliseconds */


   void _init_us(void);
   void _init_ms(void);
   void _stop_timer(void);
   void Delay_ms(uint32_t mSecs);
   void Delay_us(uint32_t uSecs);

    
#endif