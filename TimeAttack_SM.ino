bool gameTimeAttack = 1;
unsigned char timeAttackPlayTime = 2;
unsigned char timeAttackPlayTimeCnt = 0;

String inputStrings[] = {"buttonUp", "buttonDown", "buttonLeft", "buttonRight", "buttonPlus", "buttonMinus", "buttonOne", "buttonTwo", "buttonA", "buttonB", "buttonHome", 
"motionUp", "motionDown", "motionForward", "motionBack", "motionLeft", "motionRight"};

enum inputPosition {buttonUp, buttonDown, buttonLeft, buttonRight, buttonPlus, buttonMinus, buttonOne, buttonTwo, buttonA, buttonB, buttonHome, 
motionUp, motionDown, motionForward, motionBack, motionLeft, motionRight};

const int displayCommandSize = 17;
const int inputCommandSize = 17;
bool displayCommands[displayCommandSize];
bool inputCommands[inputCommandSize];
// Maps 1:1 with inputStrings

const int commandSizeDifficulty = 5;
unsigned char timeAttackDisplayCnt = 0;
unsigned char timeAttackPoints = 0;
unsigned char timeAttackScore = 0;

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

void setInput() { // REMOVE AFTER TESTING
  while (Serial.available() == 0) {
  }
  int integerVariable = Serial.parseInt();
  inputCommands[integerVariable] = 1;
}

void clearInputs() { // REMOVE AFTER TESTING
  for (int i = 0; i < inputCommandSize; i++) {
    inputCommands[i] = 0;
  }
}

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

const unsigned short tasksNum = 1;
task tasks[tasksNum];

//Command* commands = new Command[commandSizeDifficulty];
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
        timeAttackPlayTimeCnt = 0;
        timeAttackDisplayCnt = 0;
        timeAttackState = TimeAttack_Check;
      } else {
        timeAttackState = TimeAttack_Show;
      }
      break;
    case TimeAttack_Check:
      Serial.print(F("\r\nChecking..."));
      setInput(); // REMOVE AFTER TESTING
      if (timeAttackPlayTimeCnt < timeAttackPlayTime) { // still time left
        timeAttackPlayTimeCnt++;
        if (commands[timeAttackDisplayCnt].checkType() && (timeAttackDisplayCnt < commandSizeDifficulty)) { // correct and not end of sequence, move to next input (maybe jingle/image for correct?)
          timeAttackPoints++;
          timeAttackDisplayCnt++;
          Serial.print(F("\r\nCorrect!"));
          timeAttackState = TimeAttack_Check;
        } else if (!commands[timeAttackDisplayCnt].checkType() && (timeAttackDisplayCnt < commandSizeDifficulty)) { // incorrect and not end of sequence, show sequence again (maybe rumble for incorrect?)
          timeAttackDisplayCnt = 0;
          Serial.print(F("\r\nIncorrect, Try Again!"));
          timeAttackState = TimeAttack_Show;
        } else { // end of sequence, load new sequence
          timeAttackScore++;
          Serial.print(F("\r\nGreat! Loading Next Sequence"));
          timeAttackState = TimeAttack_Load;
        }
      } else if (timeAttackPlayTimeCnt >= timeAttackPlayTime) { // out of time
        Serial.print(F("\r\nSorry, Outta Time!"));
        Serial.print(F("\r\nScore: ")); Serial.print(timeAttackScore);
        Serial.print(F("\r\nPoints: ")); Serial.print(timeAttackPoints);
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
        // commands[i].inputType = inputStrings[i];
      }
      break;
    case TimeAttack_Show:
      commands[timeAttackDisplayCnt].displayImage();
      Serial.print("\r\n"); Serial.print(commands[timeAttackDisplayCnt].inputType); 
      break;
    case TimeAttack_Check:
      clearInputs(); // REMOVE AFTER TESTING
      break;
    case TimeAttack_End:
      // display image of score with varable
      setInput(); // REMOVE AFTER TESTING
      break;
  }
  return timeAttackState;
}

void setup() {
  Serial.begin(115200);
  
  unsigned char i = 0;
  tasks[i].state = TimeAttack_INIT;
  tasks[i].period = 1000; 
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &TimeAttack_Tick;
  i++;

}

void loop() {
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) {
    if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsedTime = millis(); // Last time this task was ran
    }
  }
}