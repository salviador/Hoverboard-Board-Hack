/*                              ------ hacking hoverboard -------
MOTOR LEFT
            HALL_A = PB5
            HALL_B = PB6
            HALL_C = PB7
            A-MOSFET_+ = PC6    [Attivo alto]
            A-MOSFET_- = PA7    [Attivo basso]
            B-MOSFET_+ = PC7    [Attivo alto]
            B-MOSFET_- = PB0    [Attivo basso]
            C-MOSFET_+ = PC8    [Attivo alto]
            C-MOSFET_- = PB1    [Attivo basso]
            CURRENT_SENSE_shunt = PC0 [Analog, a vuoto 1.5747V, Rshunt=0.0036525/Guadagno_OPAMP = 10.4]
            A-VOLTAGE_PHASE_A_BACK_EMF = PA0    [Analog]  -- {NOT USE}
            B-VOLTAGE_PHASE_A_BACK_EMF = PC3    [Analog]  -- {NOT USE}

MOTOR RIGHT
            HALL_A = PC10
            HALL_B = PC11
            HALL_C = PC12
            A-MOSFET_+ = PA8    [Attivo alto]
            A-MOSFET_- = PB13   [Attivo basso]
            B-MOSFET_+ = PA9    [Attivo alto]
            B-MOSFET_- = PB14   [Attivo basso]
            C-MOSFET_+ = PA10   [Attivo alto]
            C-MOSFET_- = PB15   [Attivo basso]
            CURRENT_SENSE_shunt = PC1 [Analog, a vuoto 1.5747V, Rshunt=0.0036525/Guadagno_OPAMP = 10.4]
            A-VOLTAGE_PHASE_A_BACK_EMF = PC4    [Analog]  -- {NOT USE}
            B-VOLTAGE_PHASE_A_BACK_EMF = PC5    [Analog]  -- {NOT USE}

VARIE
            LED = PB2
            BUZZER = PA4
            FRONT_LEFT[connettore 4 poli] = +15V ; PA2 ; PA3 ; MASSA    -- {NOT USE}
            FRONT_RIGHT[connettore 4 poli] = +15V ; PB10 ; PB11 ; MASSA -- {USE x JOYSTICK}
            SWITCH = PA1                                                -- {NOT USE}
            VBATT_MEASURE = PC2         [Analog, rapporto 956.5 ohm/29910 ohm]
            IS_BATTERY_IN_CHARGE = PA12 [input, need pullup]
            AUTO-RITENUTA-SWITCH = PA5 [output, mantiene attivo il tip127 che funge da interruttore generale]   -- {NOT USE} 
            NOT-IDENTIFITY = PB12
            NOT-IDENTIFITY = PA6
            
*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "motor_L.h"
#include "motor_R.h"
#include "varie.h"
#include "ADC_L.h"
#include "ADC_R.h"
#include "delay.h"
#include "pid.h"
#include "application.h"
#include "telemetry.h"

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
//extern struct PID_dati pid_R_;
static void MX_IWDG_Init(void);
IWDG_HandleTypeDef hiwdg;

extern struct TELEMETRY_dati telemetry;

//TEMP
/*
extern volatile __IO struct MOTOR_Rdati motorR;
volatile __IO uint8_t temp8,temp8case,temp_MOTOR_R_START;
volatile __IO int32_t temp_SET_SPPED;
volatile __IO uint8_t bufferTX[100],ai2cBuffer[10];
int32_t speed;
*/
volatile __IO uint32_t counterTemp,counterTempTT;

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  MX_IWDG_Init();
  
  _init_us();
 
  MX_I2C2_Init();
  Telemetry_init();
  
 /* 
  while(1){
  
      Telemetry_TASK();
      if(telemetry.dataREADY_JOYSTICK){
          telemetry.dataREADY_JOYSTICK = 0;
      }
  
  }
  */
  
  Buzzer_init();
  Led_init();
  IS_Charge_init();
  
  ADC_L_init();  
  ADC_R_init();
  MotorL_init();
  MotorR_init();

  //PID_init(0,900); //pwm limit
  //PID_set_L_costant(0.05,0.01,0.0);
  //PID_set_R_costant(2.0,0.5,0.0);
  
//DebugPin_init();

  Led_Set(1);
  Buzzer_OneBeep();
  HAL_Delay(350);
  Led_Set(0);
  
  applcation_init();
  
  while(1){
    counterTemp = HAL_GetTick();
      
    Battery_TASK();
    Current_Motor_TASK();    
    WiiNunchuck_TASK();
    applcation_TASK();
    Telemetry_TASK();
   
    //Batteria Scarica?
    if(GET_BatteryAverage() < 31.0){
      TASK_BATTERY_LOW_VOLTAGE();    
    }
    //In Carica?
    if(IS_Charge()==0){
      WAIT_CHARGE_FINISH();
    }

    HAL_IWDG_Refresh(&hiwdg);   //819mS
    
    counterTempTT = HAL_GetTick() - counterTemp;
    
    


    
    
  }
 
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* IWDG init function */
static void MX_IWDG_Init(void)
{
  __HAL_RCC_WWDG_CLK_ENABLE();
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_8;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_IWDG_Start(&hiwdg);
}
 

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
  if(hadc->Instance == ADC1){
    ADC_R_callback();
  }
  if(hadc->Instance == ADC3){
    ADC_L_callback();
  }  
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  MotorR_stop();
  MotorL_stop();  
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
