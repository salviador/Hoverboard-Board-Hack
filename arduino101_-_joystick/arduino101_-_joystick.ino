/**************************************************************
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************
 * You can receive x and y coords for joystick movement within App.
 *
 * App project setup:
 *   Two Axis Joystick on V1 in MERGE output mode.
 *   MERGE mode means device will receive both x and y within 1 message
 *
 **************************************************************/
 #include <SoftwareSerial.h>
#define BLYNK_USE_DIRECT_CONNECT

//#define BLYNK_PRINT Serial

#include <BlynkSimpleCurieBLE.h>
#include <CurieBLE.h>

SoftwareSerial mySerial(5, 3); // RX, TX
 
   
unsigned long timeTrasmitt;
int x;
int y;
int counterTXfree;
int icountrx;
unsigned int i;
int stato_rx;
int incomingByte = 0;
String MOTOR_LEFT_STRINGtemp;
String MOTOR_RIGHT_STRINGtemp;
String BATT_STRINGtemp;
char MOTOR_LEFT_STRING[20];
char MOTOR_RIGHT_STRING[20];
char BATT_STRING[20];
char tMOTOR_LEFT_STRING[20];
char tMOTOR_RIGHT_STRING[20];
char tBATT_STRING[20];

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "ae2a670c290f44aba3ae06913c483226";


BLEPeripheral  blePeripheral;

void setup() {

  mySerial.begin(9600);
   
  Serial.begin(250000);
  delay(1000);

  blePeripheral.setLocalName("Wheel");
  blePeripheral.setDeviceName("Wheel");
  blePeripheral.setAppearance(384);

  Blynk.begin(blePeripheral, auth);

  blePeripheral.begin();

  timeTrasmitt = millis();
  x=0;
  y=0;
  stato_rx=0;
  BATT_STRING[0] = 0x00;
  MOTOR_LEFT_STRING[0] = 0x00;
  MOTOR_RIGHT_STRING[0] = 0x00;

  counterTXfree=0;
  icountrx = 0;

 Serial.println("RUN");
 }

BLYNK_WRITE(V1) {
  x = param[0].asInt();
  y = param[1].asInt();
}

BLYNK_READ(V5)
{
  Blynk.virtualWrite(V5, BATT_STRING);
}
BLYNK_READ(V6)
{
  Blynk.virtualWrite(V6, MOTOR_LEFT_STRING);
}
BLYNK_READ(V7)
{
  Blynk.virtualWrite(V7, MOTOR_RIGHT_STRING);
}

void loop() {
  Blynk.run();
  blePeripheral.poll();

  if((blePeripheral.connected())&&((millis()-timeTrasmitt)>50)){
    timeTrasmitt = millis();
    mySerial.print("R");
    mySerial.print(x);
    mySerial.print(";");
    mySerial.print(y);
    mySerial.print('\n');
    counterTXfree=0;
  }
  if((blePeripheral.connected()==0)&&((millis()-timeTrasmitt)>50)){
    if(counterTXfree<3){
      timeTrasmitt = millis();
      x=75;
      y=75;
      mySerial.print("F");
      mySerial.print(x);
      mySerial.print(";");
      mySerial.print(y);
      mySerial.print('\n');
      counterTXfree++;

    }
  }







  //Recive Telemetry
  if (mySerial.available() > 0) {
    incomingByte = mySerial.read();

   // Serial.println(incomingByte,HEX);
    
      switch(stato_rx){
        case 0:
            if(incomingByte == 'T'){
              stato_rx = 1; 
              icountrx = 0;
             }
        break;
         case 1:
            if(incomingByte != ';'){
              //BATT_STRINGtemp += incomingByte;
              tBATT_STRING[icountrx] = incomingByte;
              icountrx++;
              tBATT_STRING[icountrx] = 0x00;              
              if(icountrx>4){
                //Error
                tBATT_STRING[0] = '0';              
                tBATT_STRING[1] = 0;                              
                stato_rx = 0; 
              }
             }else{
                stato_rx = 2; 
                icountrx = 0;
              }
        break;   
        case 2:
            if(incomingByte != ';'){
              tMOTOR_LEFT_STRING[icountrx] = incomingByte;
              icountrx++;
              tMOTOR_LEFT_STRING[icountrx] = 0x00;              
              if(icountrx>4){
                //Error
                tMOTOR_LEFT_STRING[0] = '0';              
                tMOTOR_LEFT_STRING[1] = 0;                              
                stato_rx = 0; 
              }
             }else{
                stato_rx = 3; 
                icountrx = 0;                
             }
        break;
         case 3:
            if(incomingByte != '\n'){
              tMOTOR_RIGHT_STRING[icountrx] = incomingByte;
              icountrx++;
              tMOTOR_RIGHT_STRING[icountrx] = 0x00;              
              if(icountrx>4){
                //Error
                tMOTOR_RIGHT_STRING[0] = '0';              
                tMOTOR_RIGHT_STRING[1] = 0;                              
                stato_rx = 0; 
              }
             }else{
                icountrx = 0;              
                stato_rx = 0; 
/*
                Serial.print(tBATT_STRING);
                Serial.print(" ; ");
                Serial.print(tMOTOR_LEFT_STRING);
                Serial.print(" ; ");
                Serial.println(tMOTOR_RIGHT_STRING);
*/

              for(i=0;i<sizeof(tBATT_STRING);i++){
                BATT_STRING[i] = tBATT_STRING[i];
              }
              for(i=0;i<sizeof(tMOTOR_LEFT_STRING);i++){
                MOTOR_LEFT_STRING[i] = tMOTOR_LEFT_STRING[i];
              }
              for(i=0;i<sizeof(tMOTOR_RIGHT_STRING);i++){
                MOTOR_RIGHT_STRING[i] = tMOTOR_RIGHT_STRING[i];
              }


             }
        break;   
      }



    
  }








  
}

