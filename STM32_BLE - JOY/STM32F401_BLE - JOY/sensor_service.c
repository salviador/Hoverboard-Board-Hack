#include "sensor_service.h"
#include "main.h"

uint16_t WHEELServHandle;
uint16_t JoystickCharHandle;
uint16_t TelemetryCharHandle;

volatile uint8_t bond_ok = FALSE;
volatile int connected = FALSE;
volatile uint8_t set_connectable = 1;
volatile uint16_t connection_handle = 0;
volatile uint8_t notification_enabled = FALSE;


#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
        uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
            uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
                uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)



#define COPY_WHEEL_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x19,0xB1,0x00,0x10, 0xE8,0xF2, 0x53,0x7E, 0x4F,0x6C, 0xD1,0x14,0x76,0xEA,0x12,0x18)
#define COPY_JOYSTICK_UUID(uuid_struct)    COPY_UUID_128(uuid_struct,0x19, 0xB1, 0x1E, 0x01, 0xE8, 0xF2, 0x53, 0x7E, 0x4F, 0x6C, 0xD1, 0x04, 0x76, 0x8A, 0x12, 0x14)
#define COPY_TELEMETRY_UUID(uuid_struct)   COPY_UUID_128(uuid_struct,0x19, 0xB1, 0x1E, 0x02, 0xE8, 0xF2, 0x53, 0x7E, 0x4F, 0x6C, 0xD1, 0x04, 0x76, 0x8A, 0x12, 0x14)


volatile __IO TELEMETRY_VALUE_t telemetry_value;


tBleStatus Add_WHEEL_Service(void)
{
  tBleStatus ret;
  uint8_t uuid[16];

  COPY_WHEEL_SERVICE_UUID(uuid);
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 7, &WHEELServHandle); //? 7
  if (ret != BLE_STATUS_SUCCESS) goto fail;    
  
  //Characteristic   
  
  COPY_JOYSTICK_UUID(uuid);
  ret =  aci_gatt_add_char(WHEELServHandle, UUID_TYPE_128, uuid, 2,
                           //CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                           CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_AUTHEN_WRITE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                           16, 1, &JoystickCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  COPY_TELEMETRY_UUID(uuid);
  ret =  aci_gatt_add_char(WHEELServHandle, UUID_TYPE_128, uuid, 12,
                           CHAR_PROP_READ | CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                           //CHAR_PROP_READ | CHAR_PROP_NOTIFY, ATTR_PERMISSION_AUTHEN_WRITE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                           16, 1, &TelemetryCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
  
  PRINTF("Service WHEEL added. Handle 0x%04X, Joystick Charac handle: 0x%04X, Telemetry Charac handle: 0x%04X\r\n",WHEELServHandle, JoystickCharHandle, TelemetryCharHandle);	
  return BLE_STATUS_SUCCESS; 
  
fail:
  PRINTF("Error while adding ACC service.\r\n");
  return BLE_STATUS_ERROR ;
    

}

tBleStatus Telemetry_Update(TELEMETRY_VALUE_t *data)
{  
  tBleStatus ret;  
  ret = aci_gatt_update_char_value(WHEELServHandle, TelemetryCharHandle, 0, 12, data->bytes);
  	
  if (ret != BLE_STATUS_SUCCESS){
    PRINTF("Error while updating TELEMETRY characteristic.\r\n") ;
    return BLE_STATUS_ERROR ;
  }
  return BLE_STATUS_SUCCESS;	
}







//***************************Low Level callback BLE***************************

void setConnectable(void)
{  
  tBleStatus ret;
  
  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'W','h','e','e','l'};
  
  /* disable scan response */
  hci_le_set_scan_resp_data(0,NULL);
  PRINTF("General Discoverable Mode.\r\n");
  
  ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 sizeof(local_name), local_name, 0, NULL, 0, 0);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error while setting discoverable mode (%d)\r\n", ret);    
  }  
}
void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle)
{  
  connected = TRUE;
  connection_handle = handle;
  
  PRINTF("Connected to device:");
  for(int i = 5; i > 0; i--){
    PRINTF("%02X-", addr[i]);
  }
  PRINTF("%02X\r\n", addr[0]);
}
void GAP_DisconnectionComplete_CB(void)
{
  bond_ok = FALSE;
  connected = FALSE;
  PRINTF("Disconnected\r\n");
  /* Make the device connectable again. */
  set_connectable = TRUE;
  notification_enabled = FALSE;
}
void Read_Request_CB(uint16_t handle)
{  
  if(handle == JoystickCharHandle + 1){
    //Acc_Update((AxesRaw_t*)&axes_data);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
  }  
  
  //EXIT:
  if(connection_handle != 0)
    aci_gatt_allow_read(connection_handle);
}

void HCI_Event_CB(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  /* obtain event packet */
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  if(hci_pckt->type != HCI_EVENT_PKT)
    return;
  
  switch(event_pckt->evt){
    
  case EVT_ENCRYPT_CHANGE:
    bond_ok = TRUE;
    break;
    
  case EVT_DISCONN_COMPLETE:
    {
      GAP_DisconnectionComplete_CB();
    }
    break;
    
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      switch(evt->subevent){
      case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
          GAP_ConnectionComplete_CB(cc->peer_bdaddr, cc->handle);
        }
        break;
      }
    }
    break;
    
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      switch(blue_evt->ecode){
        case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:         
          {
            /* this callback is invoked when a GATT attribute is modified
            extract callback data and pass to suitable handler function */

            evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
            Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data); 
          }
          break; 
        case EVT_BLUE_GATT_READ_PERMIT_REQ:
          {
            evt_gatt_read_permit_req *pr = (void*)blue_evt->data;                    
            Read_Request_CB(pr->attr_handle);                    
          }
          break;
         case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
            __NOP();
            __NOP();
            __NOP();
            __NOP();
          break;
          
          
      }
    }
    break;
  }    
}


void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  int x ,y;
  /* If GATT client has modified 'LED button characteristic' value, toggle LED2 */
  if(handle == JoystickCharHandle + 1){      
      __NOP();
      __NOP();
      __NOP();
      if(data_length==2){
        x = att_data[0];
        y = att_data[1];
        
        //PRINTF("[ joystick char read] , data_len: %d , data: %d , %d\r\n",data_length,att_data[0],att_data[1]);
        
        JoyStick_notify_Callback(x,y);
      }
      __NOP();
      __NOP();
      __NOP();
      __NOP();
      __NOP();
      __NOP();
      __NOP();
      __NOP();
      __NOP();
      __NOP();
      __NOP();
      __NOP();
      __NOP();
      __NOP();
      __NOP();
  }
}
