#include "telemetry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
volatile __IO struct UART_dati uartDati;
volatile __IO struct TELEMETRY_dati telemetry;
volatile __IO uint32_t leftToTransfer;
volatile __IO struct TELEMETRY_JOYSTICK_dati telemetry_joystick_dati;
volatile __IO uint8_t counterTXfree;


void MX_USART1_UART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&huart1);
  
    /* Peripheral DMA init*/
  
    hdma_usart1_rx.Instance = DMA1_Channel3;
    hdma_usart1_rx.Init.Request = DMA_REQUEST_3;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
    HAL_DMA_Init(&hdma_usart1_rx);

    __HAL_LINKDMA(&huart1,hdmarx,hdma_usart1_rx);

    hdma_usart1_tx.Instance = DMA1_Channel2;
    hdma_usart1_tx.Init.Request = DMA_REQUEST_3;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_usart1_tx);

    __HAL_LINKDMA(&huart1,hdmatx,hdma_usart1_tx);
  
  uartDati.UartRxReady = 0;
  uartDati.UarttxReady = 1;
  
  telemetry_joystick_dati.dataREADY_JOYSTICK = 0;
  telemetry_joystick_dati.joyx = 75;
  telemetry_joystick_dati.joyy = 75;
}


void Uart1_TX(char *message)
{
    uartDati.UarttxReady = 0;    //occupato
    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)message, strlen(message));
}
uint8_t Uart1_IS_TX_free(void){
  return (uartDati.UarttxReady); //(huart2.State == HAL_UART_STATE_READY);
}

uint8_t Uart1_IS_RX_available(void){
  return (uartDati.UartRxReady); //(huart2.State == HAL_UART_STATE_READY);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
     if (huart->Instance == USART1) {
        uartDati.UartRxReady = 1;
     }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
     if (huart->Instance == USART1) {
       uartDati.UarttxReady = 1;
     }
}

















//----------------------TELEMETRY-----------------------





void Telemetry_init(void){
  MX_USART1_UART_Init();
  telemetry.statoTX = telemetry_wait;
  telemetry.time_send = HAL_GetTick();
  telemetry.statoRX = 0;

  
  telemetry.counterBufferRX = 0;
  telemetry.counterBufferRXTrama = 0;
  telemetry.dataREADY_JOYSTICK = 0;
  telemetry.joyx = 0;
  telemetry.joyy = 0;
  telemetry.dataLast_Command = 0;
    
  HAL_UART_Receive_DMA(&huart1, (uint8_t *)&uartDati.bUartRx[0], 100);
 
  counterTXfree = 0;
}


volatile __IO uint16_t datatoRead;

void Telemetry_TASK(uint8_t connessione_stato){
  float tf1,tf2,tf3;
  uint8_t temp1;
  uint16_t temp16;
  
 // uint8_t datatoRead,ic;
  
  
  //Trasmit telemetry
  switch(telemetry.statoTX){
    case (telemetry_wait):
      if((HAL_GetTick() - telemetry.time_send) > 50){
        if(Uart1_IS_TX_free()){
            telemetry.statoTX = telemetry_send;
        }
        telemetry.time_send = HAL_GetTick();      
      }
    break;
  
    case (telemetry_send):
      if(connessione_stato){
        //Se Connessione attiva con smartphone, trasmetti joystick
        if(telemetry_joystick_dati.dataREADY_JOYSTICK){
          telemetry_joystick_dati.dataREADY_JOYSTICK = 0;
          sprintf((char *)&uartDati.bUartTx[0],"R%d;%d\n",telemetry_joystick_dati.joyx,telemetry_joystick_dati.joyy);
          Uart1_TX((char *)&uartDati.bUartTx[0]);
          counterTXfree = 0;
        }
      }else{
        if(counterTXfree<3){
          //if(telemetry_joystick_dati.dataREADY_JOYSTICK){
            telemetry_joystick_dati.dataREADY_JOYSTICK = 0;
            sprintf((char *)&uartDati.bUartTx[0],"F%d;%d\n",75,75);
            Uart1_TX((char *)&uartDati.bUartTx[0]);
            counterTXfree++;
          //}
        }
      }
      telemetry.statoTX = telemetry_wait;
      telemetry.time_send = HAL_GetTick();     
    break; 
  }
  
  
  
  //-----------------------RX TELEMETRY------------------------
  
  //Data Read in buffer ? , quantitizza dati da leggere
  datatoRead = 0;
  leftToTransfer = 100 -  __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);  
  temp1 = telemetry.counterBufferRX;
  if(temp1 > leftToTransfer){
    datatoRead = 100 - telemetry.counterBufferRX;
    temp16 = datatoRead;
    datatoRead = temp16 + leftToTransfer;
  }else{
    temp1 = telemetry.counterBufferRX;
    datatoRead = leftToTransfer - temp1;
  }
  //processa dati, Parse ALL DATA!!
  if(datatoRead!=0){
    temp1 = telemetry.counterBufferRX;
    Telemetry_RX_parseData(temp1, datatoRead);
    
    //Aggiorna puntatore , dati letti e processati
    temp1 = telemetry.counterBufferRX;
    telemetry.counterBufferRX = (temp1 + datatoRead) % 100;
  }  
}

volatile __IO uint16_t ic, temp_pointerCNT;
volatile __IO uint8_t rxvalue;
volatile __IO uint8_t temp_pointerBuff;

void Telemetry_RX_parseData(uint8_t p_start_readBuffer, uint16_t size_readBuff){
  uint8_t temp11;
    
  temp_pointerBuff = p_start_readBuffer;
  
  for(ic=0;ic<size_readBuff;ic++){
    temp11 = temp_pointerBuff;
    temp_pointerCNT = (temp11 + ic) % 100;

    rxvalue = uartDati.bUartRx[temp_pointerCNT];
    
    switch(telemetry.statoRX){
      case 0:
        //Find pattern 'T'
        if(rxvalue=='T'){
          telemetry.statoRX = 1;
          telemetry.counterBufferRXTrama = 0;
          telemetry.TelemetryRX_TramaBATTERIA[0] = 0;
          telemetry.TelemetryRX_TramaMOTORL[0] = 0;          
          telemetry.TelemetryRX_TramaMOTORR[0] = 0;          
          telemetry.dataLast_Command = 'T';
        }
      break;
      case 1:
        if(rxvalue != ';'){
          temp11 = telemetry.counterBufferRXTrama;
          telemetry.TelemetryRX_TramaBATTERIA[temp11] = rxvalue;
          telemetry.counterBufferRXTrama++;
          if(telemetry.counterBufferRXTrama>4){
            //Error reset
            telemetry.statoRX = 0;
            telemetry.dataLast_Command = 0;
          }                
        }else{
            telemetry.TelemetryRX_TramaBATTERIA[telemetry.counterBufferRXTrama] = 0;            
            telemetry.counterBufferRXTrama = 0;
            telemetry.statoRX = 2;        
        }
      break;
      
      case 2:
        if(rxvalue != ';'){
          temp11 = telemetry.counterBufferRXTrama;
          telemetry.TelemetryRX_TramaMOTORL[temp11] = rxvalue;
          telemetry.counterBufferRXTrama++;
          if(telemetry.counterBufferRXTrama>4){
            //Error reset
            telemetry.statoRX = 0;
            telemetry.dataLast_Command = 0;
          }                
        }else{
            telemetry.TelemetryRX_TramaMOTORL[telemetry.counterBufferRXTrama] = 0;            
            telemetry.counterBufferRXTrama = 0;
            telemetry.statoRX = 3;        
        }
      break;      
      
      case 3:
        if(rxvalue != '\n'){
          temp11 = telemetry.counterBufferRXTrama;
          telemetry.TelemetryRX_TramaMOTORR[temp11] = rxvalue;
          telemetry.counterBufferRXTrama++;
          if(telemetry.counterBufferRXTrama>=5){
            //Error reset
            telemetry.statoRX = 0;
            telemetry.dataLast_Command = 0;            
          }                
        }else{
            telemetry.TelemetryRX_TramaMOTORR[telemetry.counterBufferRXTrama] = 0;            
            telemetry.counterBufferRXTrama = 0;
            telemetry.statoRX = 0;        
            //Data ...ok !!!!!!!!!!!!!!!!!!!!!!
            telemetry.dataREADY_JOYSTICK = 1;           //Attiva FLAG --> Data redy!!!!!!!!!!
        }
      break;    
    }
  }

}
