//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////
// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__HARDSERIAL

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 9600

// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 230 bytes
  { 255,1,0,35,0,223,0,16,242,1,4,0,42,29,7,47,5,14,67,4,
  5,47,20,5,2,26,11,67,4,5,63,20,5,2,26,11,70,16,5,26,
  8,8,26,2,0,72,44,34,19,24,24,2,26,134,31,0,0,32,64,0,
  0,200,66,0,0,0,0,37,0,0,160,66,0,0,200,66,129,0,14,27,
  8,3,17,83,104,111,114,116,0,129,0,14,30,9,3,17,99,105,114,99,
  117,105,116,0,129,0,5,44,14,2,17,79,117,116,112,117,116,32,118,111,
  108,116,97,103,101,0,129,0,5,60,20,2,17,65,118,101,114,97,103,101,
  32,108,111,97,100,32,99,117,114,114,101,110,116,0,129,0,39,78,14,3,
  17,86,79,76,84,65,71,69,0,129,0,26,48,4,3,31,40,86,41,0,
  129,0,26,64,6,3,31,40,109,65,41,0,67,4,5,76,20,5,2,26,
  11,129,0,5,74,10,2,17,68,117,116,121,32,82,97,116,105,111,0,129,
  0,26,77,4,3,31,40,37,41,0 };
  
// this structure defines all the variables and events of your control interface 
struct {
    // input variables
  int8_t slider_1; // =0..100 slider position 

    // output variables
  char voltReading[11];  // string UTF8 end zero 
  char curReading[11];  // string UTF8 end zero 
  uint8_t shortCircuit; // led state 0 .. 1 
  int8_t circularbar_1;  // from 2.5 to 100 
  char dutyText[11];  // string UTF8 end zero 
  
    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 
  
} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

#include <TimerOne.h> 
#define pwmPin 9                                  //Pwm pin
#define vReadPin A0                               //Reading pin
float current =0.0 , vOnShunt= 0.0 , DutyRatio=0.0 , vReading =0.0;
float loadRes=120.0;
float ShuntRes= 10.0;
void setup() 
{
  RemoteXY_Init ();  
  Serial.begin(9600);
  pinMode(pwmPin,OUTPUT);
  pinMode(vReadPin,INPUT);
  Timer1.initialize(50);                                                        //number in uS
}
void loop() 
{ 
  RemoteXY_Handler ();
  RemoteXY.slider_1=constrain(RemoteXY.slider_1,19.4,100.0);
  DutyRatio = ((RemoteXY.slider_1)/100.0);
  Timer1.pwm(pwmPin,(DutyRatio)*1023);
  RemoteXY.circularbar_1=RemoteXY.slider_1;                                  
  dtostrf (DutyRatio*100.0,3,0,RemoteXY.dutyText);                              
  vReading=(analogRead(vReadPin));                                              //Reading voltage on shunt res   
  vOnShunt=2*vReading *(5.0/1023.0);                                          ///////////////// delete 2* ya 7ag /////////////////////////////////
  dtostrf((vOnShunt*(loadRes/ShuntRes)),5,2,RemoteXY.voltReading);              //use voltage divider to calculate voltage on load
  current=(vOnShunt/ShuntRes)*1000.0;                                           // in mAmps
  dtostrf(current,5,2,RemoteXY.curReading);                                     //Multiplied by 1000 to convert it to mAmp 
  RemoteXY_delay(50);
  if(current >100 )                                                             
  {
   RemoteXY.shortCircuit=1;
   RemoteXY_delay(50);
   DutyRatio=0;
   RemoteXY.circularbar_1=0;
   RemoteXY.slider_1=0; 
   RemoteXY.shortCircuit=0;
  }
  
}