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

String inputStrings[] = {"buttonUp", "buttonDown", "buttonLeft", "buttonRight", "buttonPlus", "buttonMinus", "buttonOne", "buttonTwo", "buttonA", "buttonB", "buttonHome", 
"motionUp", "motionDown", "motionForward", "motionBack", "motionLeft", "motionRight"};

enum inputPosition {buttonUp, buttonDown, buttonLeft, buttonRight, buttonPlus, buttonMinus, buttonOne, buttonTwo, buttonA, buttonB, buttonHome, 
motionUp, motionDown, motionForward, motionBack, motionLeft, motionRight};

const int displayCommandSize = 17;
const int inputCommandSize = 17;
bool displayCommands[displayCommandSize];
bool inputCommands[inputCommandSize];

class Command {
  public:
    String inputType;
    void displayImage();
    void clearImage();
    bool checkType();
};

void Command::displayImage() {
  for (int i = 0; i < inputCommandSize; i++) {
    if (this->inputType == inputStrings[i]) {
      displayCommands[i] = 1;
      break;
    }
  }
}

// Might have too much power to overwrite every display
void Command::clearImage() { 
  for (int i = 0; i < displayCommandSize; i++) {
    displayCommands[i] = 0;
  }
}

bool Command::checkType() {
  for (int i = 0; i < inputCommandSize; i++) {
    if (this->inputType == inputStrings[i]) {
      return inputCommands[i];
    }
  }
}

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

const unsigned short tasksNum = 3;
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
      inputCommands[buttonUp] = (Wii.getButtonClick(UP)) ? 1 : 0;
      inputCommands[buttonDown] = (Wii.getButtonClick(DOWN)) ? 1 : 0;
      inputCommands[buttonLeft] = (Wii.getButtonClick(LEFT)) ? 1 : 0;
      inputCommands[buttonRight] = (Wii.getButtonClick(RIGHT)) ? 1 : 0;
      inputCommands[buttonPlus] = (Wii.getButtonClick(PLUS)) ? 1 : 0;
      inputCommands[buttonMinus] = (Wii.getButtonClick(MINUS)) ? 1 : 0;
      inputCommands[buttonOne] = (Wii.getButtonClick(ONE)) ? 1 : 0;
      inputCommands[buttonTwo] = (Wii.getButtonClick(TWO)) ? 1 : 0;
      inputCommands[buttonA] = (Wii.getButtonClick(A)) ? 1 : 0;
      inputCommands[buttonB] = (Wii.getButtonClick(B)) ? 1 : 0;
      inputCommands[buttonHome] = (Wii.getButtonClick(HOME)) ? 1 : 0;
      
      if (inputCommands[buttonUp]) {Serial.print(F("\r\nbuttonUp: ")); Serial.print(inputCommands[buttonUp]); }
      if (inputCommands[buttonDown]) {Serial.print(F("\r\nbuttonDown: ")); Serial.print(inputCommands[buttonDown]); }
      if (inputCommands[buttonLeft]) {Serial.print(F("\r\nbuttonLeft: ")); Serial.print(inputCommands[buttonLeft]); }
      if (inputCommands[buttonRight]) {Serial.print(F("\r\nbuttonRight: ")); Serial.print(inputCommands[buttonRight]); }
      if (inputCommands[buttonPlus]) {Serial.print(F("\r\nbuttonPlus: ")); Serial.print(inputCommands[buttonPlus]); }
      if (inputCommands[buttonMinus]) {Serial.print(F("\r\nbuttonMinus: ")); Serial.print(inputCommands[buttonMinus]); }
      if (inputCommands[buttonOne]) {Serial.print(F("\r\nbuttonOne: ")); Serial.print(inputCommands[buttonOne]); }
      if (inputCommands[buttonTwo]) {Serial.print(F("\r\nbuttonTwo: ")); Serial.print(inputCommands[buttonTwo]); }
      if (inputCommands[buttonA]) {Serial.print(F("\r\nbuttonA: ")); Serial.print(inputCommands[buttonA]); }
      if (inputCommands[buttonB]) {Serial.print(F("\r\nbuttonB: ")); Serial.print(inputCommands[buttonB]); }
      if (inputCommands[buttonHome]) {Serial.print(F("\r\nbuttonHome: ")); Serial.print(inputCommands[buttonHome]); }
      
      break;
  }
  return buttonState;
}

bool readMotionInput = 0;
const int motionReadTime = 10;
unsigned char motionReadCnt;

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

enum Motion_State { Motion_INIT, Motion_Wait, Motion_Read, Motion_Check };
int Motion_Tick(int motionState) {
  switch(motionState) {
    case Motion_INIT:
      motionState = Motion_Wait;
      break;
    case Motion_Wait:
      if (readMotionInput) {
        motionReadCnt = 0;
        motionState = Motion_Read;
      } else {
        motionState = Motion_Wait;
      }
      break;
    case Motion_Read:
      if (motionReadCnt < motionReadTime) {
        motionState = Motion_Read;
      } else if (motionReadCnt >= motionReadTime) {
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
      motionPitchArr[motionReadCnt] = Wii.getPitch();
      motionRollArr[motionReadCnt] = Wii.getRoll();
      motionYawArr[motionReadCnt] = Wii.getYaw();
      // Serial.print(F("\r\nmotionPitch[")); Serial.print(motionReadCnt); Serial.print(F("]: ")); Serial.print(motionPitchArr[motionReadCnt]);
      // Serial.print(F("\r motionRoll[")); Serial.print(motionReadCnt); Serial.print(F("]: ")); Serial.print(motionRollArr[motionReadCnt]);
      // Serial.print(F("\r motionYaw[")); Serial.print(motionReadCnt); Serial.print(F("]: ")); Serial.print(motionYawArr[motionReadCnt]);
      motionReadCnt++;
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
        inputCommands[motionUp] = 1;
        inputCommands[motionDown] = 0;
      } else if ((motionPitchAvg <= DOWN_THRESH_UPPER) && (motionPitchAvg >= DOWN_THRESH_LOWER)){
        inputCommands[motionUp] = 0;
        inputCommands[motionDown] = 1;
      } else {
        inputCommands[motionUp] = 0;
        inputCommands[motionDown] = 0;
      }

      if ((motionPitchAvg <= FORWARD_THRESH_UPPER) && (motionPitchAvg >= FORWARD_THRESH_LOWER)) {
        inputCommands[motionForward] = 1;
        inputCommands[motionBack] = 0;
      } else if ((motionPitchAvg <= BACK_THRESH_LOWER) || (motionPitchAvg >= BACK_THRESH_UPPER)){
        inputCommands[motionForward] = 0;
        inputCommands[motionBack] = 1;
      } else {
        inputCommands[motionForward] = 0;
        inputCommands[motionBack] = 0;
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
        inputCommands[motionLeft] = 0;
        inputCommands[motionRight] = 1;
      } else if ((leftCount > (motionReadTime / 2)) && (rightCount <= 1)) {
        inputCommands[motionLeft] = 1;
        inputCommands[motionRight] = 0;
      } else {
        inputCommands[motionLeft] = 0;
        inputCommands[motionRight] = 0;
      }
      
      // Serial.print(F("\r\nmotionUp: ")); Serial.print(inputCommands[motionUp]); Serial.print(F(" motionDown: ")); Serial.print(inputCommands[motionDown]);
      // Serial.print(F("\r\nmotionForward: ")); Serial.print(inputCommands[motionForward]); Serial.print(F(" motionBack: ")); Serial.print(inputCommands[motionBack]);
      // Serial.print(F("\r\nmotionLeft: ")); Serial.print(inputCommands[motionLeft]); Serial.print(F(" motionRight: ")); Serial.print(inputCommands[motionRight]);
      
      break;
  }
  return motionState;
}

bool gameTimeAttack = 1;
unsigned char timeAttackPlayTime = 100;
unsigned char timeAttackTimeCnt = 0;

const int commandSizeDifficulty = 5;
unsigned char timeAttackDisplayCnt = 0;
unsigned char timeAttackPoints = 0;
unsigned char timeAttackScore = 0;

Command commands[commandSizeDifficulty];

enum TimeAttack_State {TimeAttack_INIT, TimeAttack_Off, TimeAttack_Load, TimeAttack_Show, TimeAttack_Check, TimeAttack_End};
int TimeAttack_Tick(int timeAttackState) {
  switch(timeAttackState) {
    case TimeAttack_INIT:
      timeAttackState = TimeAttack_Off;
      break;
    case TimeAttack_Off:
      if (gameTimeAttack) {
        timeAttackPoints = 0;
        timeAttackScore = 0;
        timeAttackState = TimeAttack_Load;
      } else {
        timeAttackState = TimeAttack_Off;
      }
      break;
    case TimeAttack_Load:
      timeAttackDisplayCnt = 0;
      Serial.print(F("\r\nShowing Sequence"));
      timeAttackState = TimeAttack_Show;
      break;
    case TimeAttack_Show:
      if (timeAttackDisplayCnt < commandSizeDifficulty) {
        commands[timeAttackDisplayCnt].clearImage();
        timeAttackDisplayCnt++;
        timeAttackState = TimeAttack_Show;
      } else if (timeAttackDisplayCnt >= commandSizeDifficulty) {
        timeAttackTimeCnt = 0;
        timeAttackDisplayCnt = 0;
        readMotionInput = 1;
        Serial.print("\r\nGO!!!");
        timeAttackState = TimeAttack_Check;
      } else {
        timeAttackState = TimeAttack_Show;
      }
      break;
    case TimeAttack_Check:
      if (timeAttackTimeCnt < timeAttackPlayTime) { // still time left
        timeAttackTimeCnt++;
        if (timeAttackDisplayCnt >= commandSizeDifficulty) { 
          timeAttackScore++;
          Serial.print(F("\r\nGreat! Loading Next Sequence"));
          timeAttackState = TimeAttack_Load;
        } else { 
          timeAttackState = TimeAttack_Check;
        }
      } else if (timeAttackTimeCnt >= timeAttackPlayTime) {
        Serial.print(F("\r\nSorry, Outta Time!"));
        Serial.print(F("\r\nScore: ")); Serial.print(timeAttackScore);
        Serial.print(F("\r\nPoints: ")); Serial.print(timeAttackPoints);
        readMotionInput = 0;
        timeAttackState = TimeAttack_End;
      } else {
        timeAttackState = TimeAttack_Check;
      }
      break;
    case TimeAttack_End:
      if (inputCommands[buttonA]) {
        timeAttackState = TimeAttack_Off;
      } else {
        timeAttackState = TimeAttack_End;
      }
      break;
  }
  switch(timeAttackState) {
    case TimeAttack_INIT:
      break;
    case TimeAttack_Off:
      break;
    case TimeAttack_Load:
      // Makes new array to display from all possible inputs, randomly chosen
      for (int i = 0; i < commandSizeDifficulty; i++) {
        int newCommand = rand() % inputCommandSize;
        commands[i].inputType = (newCommand != buttonHome) ? inputStrings[newCommand] : inputStrings[newCommand + 1];
      }
      break;
    case TimeAttack_Show:
      commands[timeAttackDisplayCnt].displayImage();
      Serial.print("\r\n"); Serial.print(commands[timeAttackDisplayCnt].inputType); 
      break;
    case TimeAttack_Check:
      Serial.print(F("\r\nChecking..."));
      if (commands[timeAttackDisplayCnt].checkType()) { // Now no Fail Check
        Serial.print(F("\r\nCorrect!"));
        timeAttackDisplayCnt++;
        timeAttackPoints++;
      }
      break;
    case TimeAttack_End:
      // display image of score with varable
      break;
  }
  return timeAttackState;
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
  tasks[i].period = 100; 
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Button_Tick;
  i++;
  tasks[i].state = Motion_INIT;
  tasks[i].period = 100; 
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Motion_Tick;
  i++;
  tasks[i].state = TimeAttack_INIT;
  tasks[i].period = 100; 
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &TimeAttack_Tick;
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