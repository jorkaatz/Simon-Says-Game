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


bool buttonUp = 0;
bool buttonDown = 0;
bool buttonLeft = 0;
bool buttonRight = 0;
bool buttonPlus = 0;
bool buttonMinus = 0;
bool buttonOne = 0;
bool buttonTwo = 0;
bool buttonA = 0;
bool buttonB = 0;
bool buttonHome = 0;

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

const unsigned short tasksNum = 1;
task tasks[tasksNum];

enum Button_State { Button_INIT, Button_Read };
int Button_Tick(int buttonState) {
  switch(buttonState) {
    case Button_INIT:
      buttonState = Button_Read;
      break;
    case Button_Read:
      buttonState = Button_Read;
      break;

  }
  switch(buttonState) {
    case Button_INIT:
      break;
    case Button_Read:
      buttonUp = (Wii.getButtonClick(UP)) ? 1 : 0;
      buttonDown = (Wii.getButtonClick(DOWN)) ? 1 : 0;
      buttonLeft = (Wii.getButtonClick(LEFT)) ? 1 : 0;
      buttonRight = (Wii.getButtonClick(RIGHT)) ? 1 : 0;
      buttonPlus = (Wii.getButtonClick(PLUS)) ? 1 : 0;
      buttonMinus = (Wii.getButtonClick(MINUS)) ? 1 : 0;
      buttonOne = (Wii.getButtonClick(ONE)) ? 1 : 0;
      buttonTwo = (Wii.getButtonClick(TWO)) ? 1 : 0;
      buttonA = (Wii.getButtonClick(A)) ? 1 : 0;
      buttonB = (Wii.getButtonClick(B)) ? 1 : 0;
      buttonHome = (Wii.getButtonClick(HOME)) ? 1 : 0;
      
      if (buttonUp) {Serial.print(F("\r\nbuttonUp: ")); Serial.print(buttonUp); }
      if (buttonDown) {Serial.print(F("\r\nbuttonDown: ")); Serial.print(buttonDown); }
      if (buttonLeft) {Serial.print(F("\r\nbuttonLeft: ")); Serial.print(buttonLeft); }
      if (buttonRight) {Serial.print(F("\r\nbuttonRight: ")); Serial.print(buttonRight); }
      if (buttonPlus) {Serial.print(F("\r\nbuttonPlus: ")); Serial.print(buttonPlus); }
      if (buttonMinus) {Serial.print(F("\r\nbuttonMinus: ")); Serial.print(buttonMinus); }
      if (buttonOne) {Serial.print(F("\r\nbuttonOne: ")); Serial.print(buttonOne); }
      if (buttonTwo) {Serial.print(F("\r\nbuttonTwo: ")); Serial.print(buttonTwo); }
      if (buttonA) {Serial.print(F("\r\nbuttonA: ")); Serial.print(buttonA); }
      if (buttonB) {Serial.print(F("\r\nbuttonB: ")); Serial.print(buttonB); }
      if (buttonHome) {Serial.print(F("\r\nbuttonHome: ")); Serial.print(buttonHome); }
      
      break;
  }
  return buttonState;
}

void setup() {
  Serial.begin(115200);
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nWiimote Bluetooth Library Started"));
  
  unsigned char i = 0;
  tasks[i].state = Button_INIT;
  tasks[i].period = 50; 
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Button_Tick;
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