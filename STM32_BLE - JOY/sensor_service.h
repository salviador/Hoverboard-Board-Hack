#ifndef _SENSOR_SERVICE_H_
#define _SENSOR_SERVICE_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include "cube_hal.h"
#include "hal_types.h"
#include "bluenrg_gatt_server.h"
#include "bluenrg_gap.h"
#include "string.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_gatt_aci.h"
#include "hci_const.h"
#include "gp_timer.h"
#include "bluenrg_hal_aci.h"
#include "bluenrg_aci_const.h"   
#include "hci.h"
#include "hci_le.h"
#include "hal.h"
#include "sm.h"
#include "debug.h"

#include <stdlib.h>

#define IDB04A1 0
#define IDB05A1 1


   typedef int i32_t;
   

typedef union
{
  struct{
    float Battery;
    float Current_m_L;
    float Current_m_R;
  };
  uint8_t bytes[12];
}TELEMETRY_VALUE_t;



tBleStatus Add_WHEEL_Service(void);
tBleStatus Telemetry_Update(TELEMETRY_VALUE_t *data);




void       setConnectable(void);
void       enableNotification(void);
void       GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle);
void       GAP_DisconnectionComplete_CB(void);
void       HCI_Event_CB(void *pckt);
void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data);


#ifdef __cplusplus
}
#endif

#endif /* _SENSOR_SERVICE_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

