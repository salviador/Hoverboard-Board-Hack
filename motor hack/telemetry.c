#include "telemetry.h"
#include "uart.h"
#include "application.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern volatile __IO struct UART_dati uartDati;
extern volatile __IO struct APPLICATION_dati app;
extern volatile __IO struct BATTERY_dati battery_dati;
volatile __IO struct TELEMETRY_dati telemetry;

 
 
 volatile __IO uint32_t leftToTransfer;
 
void Telemetry_init(void){
  MX_USART2_UART_Init();
  telemetry.statoTX = telemetry_wait;
  telemetry.time_send = HAL_GetTick();
  telemetry.statoRX = 0;

  
  telemetry.counterBufferRX = 0;
  telemetry.counterBufferRXTrama = 0;
  telemetry.dataREADY_JOYSTICK = 0;
  telemetry.joyx = 0;
  telemetry.joyy = 0;
  telemetry.dataLast_Command = 0;
    
  HAL_UART_Receive_DMA(&huart2, (uint8_t *)&uartDati.bUartRx[0], 100);
 
}


volatile __IO uint16_t datatoRead;

void Telemetry_TASK(void){
  float tf1,tf2,tf3;
  uint8_t temp1;
  uint16_t temp16;
  
 // uint8_t datatoRead,ic;
  
  
  //Trasmit telemetry
  switch(telemetry.statoTX){
    case (telemetry_wait):
      if((HAL_GetTick() - telemetry.time_send) > 250){
        if(Uart2_IS_TX_free()){
            telemetry.statoTX = telemetry_send;
        }
        telemetry.time_send = HAL_GetTick();      
      }
    break;
  
    case (telemetry_send):
      tf1 = battery_dati.VBatt;
      tf2 = app.Current_M_LEFT;
      tf3 = app.Current_M_RIGHT;
      sprintf((char *)&uartDati.bUartTx[0],"T%.1f;%.1f;%.1f\n",tf1,tf2,tf3);
      //sprintf((char *)&uartDati.bUartTx[0],"Ciao");

      Uart2_TX((char *)&uartDati.bUartTx[0]);
      telemetry.statoTX = telemetry_wait;
      telemetry.time_send = HAL_GetTick();     
    break; 
  }
  
  
  
  //-----------------------RX TELEMETRY------------------------
  
  //Data Read in buffer ? , quantitizza dati da leggere
  datatoRead = 0;
  leftToTransfer = 100 -  __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);  
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
        //Find pattern 'R' o 'F'
        if(rxvalue=='F'){
          telemetry.statoRX = 1;
          telemetry.counterBufferRXTrama = 0;
            telemetry.TelemetryRX_TramaX[0] = 0;
            telemetry.TelemetryRX_TramaY[0] = 0;          
            telemetry.dataLast_Command = 'F';
        }else if(rxvalue=='R'){
          telemetry.statoRX = 1;      
          telemetry.counterBufferRXTrama = 0;
          telemetry.TelemetryRX_TramaX[0] = 0;
          telemetry.TelemetryRX_TramaY[0] = 0;               
            telemetry.dataLast_Command = 'R';          
        }
      break;
      case 1:
        if(rxvalue != ';'){
          temp11 = telemetry.counterBufferRXTrama;
          telemetry.TelemetryRX_TramaX[temp11] = rxvalue;
          telemetry.counterBufferRXTrama++;
          if(telemetry.counterBufferRXTrama>=5){
            //Error reset
            telemetry.statoRX = 0;
            telemetry.dataLast_Command = 0;
          }                
        }else{
            telemetry.TelemetryRX_TramaX[telemetry.counterBufferRXTrama] = 0;            
            telemetry.counterBufferRXTrama = 0;
            telemetry.statoRX = 2;        
        }
      break;
      case 2:
        if(rxvalue != '\n'){
          temp11 = telemetry.counterBufferRXTrama;
          telemetry.TelemetryRX_TramaY[temp11] = rxvalue;
          telemetry.counterBufferRXTrama++;
          if(telemetry.counterBufferRXTrama>=5){
            //Error reset
            telemetry.statoRX = 0;
            telemetry.dataLast_Command = 0;            
          }                
        }else{
            telemetry.TelemetryRX_TramaY[telemetry.counterBufferRXTrama] = 0;            
            telemetry.counterBufferRXTrama = 0;
            telemetry.statoRX = 0;        
            //Data ...ok !!!!!!!!!!!!!!!!!!!!!!
            telemetry.dataREADY_JOYSTICK = 1;           //Attiva FLAG --> Data redy!!!!!!!!!!
            telemetry.joyx = atoi((char *)&telemetry.TelemetryRX_TramaX[0]);
            telemetry.joyy = atoi((char *)&telemetry.TelemetryRX_TramaY[0]);    
        }
      break;    
    }
  }

}
