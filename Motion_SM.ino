#include <Wii.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
WII Wii(&Btd, PAIR); // This will start an inquiry and then pair with your Wiimote - you only have to do this once
//WII Wii(&Btd); // After that you can simply create the instance like so and then press any button on the Wiimote

bool readMotionInput = 1;
const int motionReadTime = 5;

float motionPitchArr[motionReadTime]; 
float motionRollArr[motionReadTime]; 
float motionYawArr[motionReadTime]; 
float motionPitchAvg;
float motionRollAvg;
float motionYawAvg;

float UP_THRESH_UPPER = 179;
float UP_THRESH_LOWER = 0;
float DOWN_THRESH_UPPER = 360;
float DOWN_THRESH_LOWER = 180;
float FORWARD_THRESH_UPPER = 270;
float FORWARD_THRESH_LOWER = 90;
float BACK_THRESH_UPPER = 271;
float BACK_THRESH_LOWER = 89;

float LEFT_THRESH_UPPER = 360;
float LEFT_THRESH_LOWER = 180;
float RIGHT_THRESH_UPPER = 179;
float RIGHT_THRESH_LOWER = 0;


bool motionUp = 0;
bool motionDown = 0;
bool motionForward = 0;
bool motionBack = 0;
bool motionLeft = 0;
bool motionRight = 0;

int j;

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

const unsigned short tasksNum = 1;
task tasks[tasksNum];

enum Motion_State { Motion_INIT, Motion_Wait, Motion_Read, Motion_Check };
int Motion_Tick(int motionState) {
  switch(motionState) {
    case Motion_INIT:
      motionState = Motion_Wait;
      break;
    case Motion_Wait:
      if (readMotionInput) {
        j = 0;
        motionState = Motion_Read;
      } else {
        motionState = Motion_Wait;
      }
      break;
    case Motion_Read:
      if (j < motionReadTime) {
        motionState = Motion_Read;
      } else if (j >= motionReadTime) {
        motionPitchAvg = 0;
        motionRollAvg = 0;
        motionState = Motion_Check;
      } else {
        motionState = Motion_Read;
      }
      break;
    case Motion_Check:
      motionState = Motion_Wait;
      break;
  }
  switch(motionState) {
    case Motion_INIT:
      break;
    case Motion_Wait:
      break;
    case Motion_Read:
      motionPitchArr[j] = Wii.getPitch();
      motionRollArr[j] = Wii.getRoll();
      motionYawArr[j] = Wii.getYaw();
      Serial.print(F("\r\nmotionPitch[")); Serial.print(j); Serial.print(F("]: ")); Serial.print(motionPitchArr[j]);
      Serial.print(F("\r motionRoll[")); Serial.print(j); Serial.print(F("]: ")); Serial.print(motionRollArr[j]);
      Serial.print(F("\r motionYaw[")); Serial.print(j); Serial.print(F("]: ")); Serial.print(motionYawArr[j]);
      j++;
      break;
    case Motion_Check:
      for (int k = 0; k < motionReadTime; k++) {
        motionPitchAvg += motionPitchArr[k];
        motionRollAvg += motionRollArr[k];
        motionYawAvg += motionYawArr[k];
      }
      motionPitchAvg = motionPitchAvg / motionReadTime;
      motionRollAvg = motionRollAvg / motionReadTime;
      motionYawAvg = motionYawAvg / motionReadTime;

      if ((motionPitchAvg <= UP_THRESH_UPPER) && (motionPitchAvg >= UP_THRESH_LOWER)) {
        motionUp = 1;
        motionDown = 0;
      } else if ((motionPitchAvg <= DOWN_THRESH_UPPER) && (motionPitchAvg >= DOWN_THRESH_LOWER)){
        motionUp = 0;
        motionDown = 1;
      } else {
        motionUp = 0;
        motionDown = 0;
      }

      if ((motionPitchAvg <= FORWARD_THRESH_UPPER) && (motionPitchAvg >= FORWARD_THRESH_LOWER)) {
        motionForward = 1;
        motionBack = 0;
      } else if ((motionPitchAvg <= BACK_THRESH_LOWER) || (motionPitchAvg >= BACK_THRESH_UPPER)){
        motionForward = 0;
        motionBack = 1;
      } else {
        motionForward = 0;
        motionBack = 0;
      }

      int rightCount = 0;
      int leftCount = 0;
      for (int k = 0; k < motionReadTime; k++){
        if ((motionYawArr[k+1] - motionYawArr[0]) < -5) {
          rightCount++;
        } else if ((motionYawArr[k+1] - motionYawArr[0]) > 5) {
          leftCount++;
        } else {
          // do nothing
        }
      }
      
      if ((rightCount > (motionReadTime / 2)) && (leftCount <= 1)) {
        motionLeft = 0;
        motionRight = 1;
      } else if ((leftCount > (motionReadTime / 2)) && (rightCount <= 1)) {
        motionLeft = 1;
        motionRight = 0;
      } else {
        motionLeft = 0;
        motionRight = 0;
      }
      
      Serial.print(F("\r\nmotionUp: ")); Serial.print(motionUp);
      Serial.print(F("\r\nmotionDown: ")); Serial.print(motionDown);
      Serial.print(F("\r\nmotionForward: ")); Serial.print(motionForward);
      Serial.print(F("\r\nmotionBack: ")); Serial.print(motionBack);
      Serial.print(F("\r\nmotionLeft: ")); Serial.print(motionLeft);
      Serial.print(F("\r\nmotionRight: ")); Serial.print(motionRight);
      
      break;
  }
  return motionState;
}

void setup() {
  Serial.begin(115200);
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nWiimote Bluetooth Library Started"));
  
  unsigned char i = 0;
  tasks[i].state = Motion_INIT;
  tasks[i].period = 500; 
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Motion_Tick;
  i++;

}

void loop() {
  Usb.Task();
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) {
    if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsedTime = millis(); // Last time this task was ran
    }
  }
}