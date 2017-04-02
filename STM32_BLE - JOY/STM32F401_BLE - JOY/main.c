#include "cube_hal.h"
#include "osal.h"
#include "sensor_service.h"
#include "debug.h"
#include "stm32_bluenrg_ble.h"
#include "bluenrg_utils.h"
#include "debug.h"

#include <stdlib.h> //only for random!

//X-NUCLEO-IDB05A1

//My Application setting
uint32_t time_Update_Telemetry;
extern TELEMETRY_VALUE_t telemetry_value;


//BLE Setting
#define BDADDR_SIZE 6
extern volatile uint8_t set_connectable;
extern volatile int connected;
uint8_t bnrg_expansion_board = IDB05A1; // IDB04A1; /* at startup, suppose the X-NUCLEO-IDB04A1 is used */
extern volatile uint8_t bond_ok;


int main(void)
{
  const char *name = "Wheel";
  uint8_t SERVER_BDADDR[] = {0x1f, 0x34, 0x00, 0xE1, 0x80, 0x03};       //13
  uint8_t bdaddr[BDADDR_SIZE];
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  
  uint8_t  hwVersion;
  uint16_t fwVersion;
  
  int ret;  

  HAL_Init();
  

  
  BSP_LED_Init(LED2); 
  
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  SystemClock_Config();

  BNRG_SPI_Init();
  
  HCI_Init();

  BlueNRG_RST();
    
  getBlueNRGVersion(&hwVersion, &fwVersion);

  BlueNRG_RST();

  DebugUart_init();
    
  PRINTF("HWver %d, FWver %d\r\n", hwVersion, fwVersion);
  
  if (hwVersion > 0x30) { /* X-NUCLEO-IDB05A1 expansion board is used */
    bnrg_expansion_board = IDB05A1; //Change the MAC address to avoid issues with Android cache:
    SERVER_BDADDR[5] = 0x02;
  }
  Osal_MemCpy(bdaddr, SERVER_BDADDR, sizeof(SERVER_BDADDR));
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                                  CONFIG_DATA_PUBADDR_LEN,
                                  bdaddr);
  if(ret){
    PRINTF("Setting BD_ADDR failed.\r\n");
  }
 
  ret = aci_gatt_init();    
  if(ret){
    PRINTF("GATT_Init failed.\r\n");
  }

    ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
   //ret = aci_gap_init_IDB04A1(GAP_PERIPHERAL_ROLE_IDB04A1, &service_handle, &dev_name_char_handle, &appearance_char_handle);

  if(ret != BLE_STATUS_SUCCESS){
    PRINTF("GAP_Init failed.\r\n");
  }

  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
                                   strlen(name), (uint8_t *)name);

  if(ret){
    PRINTF("aci_gatt_update_char_value failed.\r\n");            
    while(1);
  }
  
  ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED, //MITM_PROTECTION_NOT_REQUIRED,
                                     OOB_AUTH_DATA_ABSENT,
                                     NULL,
                                     7,
                                     16,
                                     USE_FIXED_PIN_FOR_PAIRING, // DONOT_USE_FIXED_PIN_FOR_PAIRING,
                                     123456,
                                     BONDING); //NO_BONDING);
  if (ret == BLE_STATUS_SUCCESS) {
    PRINTF("BLE Stack Initialized.\r\n");
  }  
  PRINTF("SERVER: BLE Stack Initialized\r\n");
  
 //aci_gap_set_author_requirement(service_handle,1);


  //-------------SERVICE--------------
  
  ret = Add_WHEEL_Service();
  
  if(ret == BLE_STATUS_SUCCESS){
    PRINTF("wheel service added successfully.\r\n");
  }
  else{
    PRINTF("Error while adding Acc service.\r\n");
  }
  
  /* Set output power level */
  ret = aci_hal_set_tx_power_level(1,4);

  bond_ok = FALSE;      //Se si è scelto il paired BOND, aspettare prima di trasmettere i dati x prevenire errore
  
  while(1)
  {
      HCI_Process();
      
      if(set_connectable){
        setConnectable();
        set_connectable = FALSE;
      }  
    
      if(connected && bond_ok)  //aspetta connessione e password giusta prima di trasmettere, x evitare errore
      {
          //Update Telemetry data
          if((HAL_GetTick() - time_Update_Telemetry)> 100){       //Ogni 100mS se connesso invia telemetria
            time_Update_Telemetry = HAL_GetTick();
            
            telemetry_value.Battery = 12.5;       //(float)rand();
            telemetry_value.Current_m_L = 1.5;
            telemetry_value.Current_m_R = 0.3;
          
            Telemetry_Update(&telemetry_value);
         }
      }
    
  }  
}


void JoyStick_notify_Callback(int x, int y){
  PRINTF("[ joystick char read] , x: %d , y: %d\r\n",x,y);
  
}

