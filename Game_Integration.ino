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

const unsigned short tasksNum = 3;
task tasks[tasksNum];

bool gameInfinite = 0;
bool gameTimeAttack = 0;
bool timeAttackDone = 0;
bool infiniteDone = 0;

enum Game_State {Game_INIT, Game_Wait, Game_TimeAttack, Game_Infinite};
int Game_Tick(int gameState) {
  switch(gameState) {
    case Game_INIT:
      gameState = Game_Wait;
      break;
    case Game_Wait:
      Serial.print(F("\r\nChoose Game Mode"));
      setInput();
      if (inputCommands[buttonPlus]) {
        Serial.print(F("\r\nStarting TimeAttack..."));
        gameTimeAttack = 1;
        gameState = Game_TimeAttack;
      } else if ((inputCommands[buttonMinus])) {
        Serial.print(F("\r\nStarting Infinite..."));
        gameInfinite = 1;
        gameState = Game_Infinite;
      } else {
        gameState = Game_Wait;
      }
      break;
    case Game_TimeAttack:
      gameTimeAttack = 0;
      gameState = (timeAttackDone) ? Game_Wait : Game_TimeAttack;
      break;
    case Game_Infinite:
      gameInfinite = 0;
      gameState = (infiniteDone) ? Game_Wait : Game_Infinite;
      break;
  }
  switch(gameState) {
    case Game_INIT:
      break;
    case Game_Wait:
      break;
    case Game_TimeAttack:
      break;
    case Game_Infinite:
      break;
  }
  return gameState;
}

unsigned char timeAttackPlayTime = 2;
unsigned char timeAttackTimeCnt = 0;

const int commandSizeDifficulty = 5;
unsigned char timeAttackDisplayCnt = 0;
unsigned char timeAttackPoints = 0;
unsigned char timeAttackScore = 0;

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
        timeAttackDone = 0;
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
        // readMotionInput = 1;
        Serial.print("\r\nGO!!!");
        timeAttackState = TimeAttack_Check;
      } else {
        timeAttackState = TimeAttack_Show;
      }
      break;
    case TimeAttack_Check:
      Serial.print(F("\r\nChecking..."));
      setInput(); // REMOVE AFTER TESTING
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
        // readMotionInput = 0;
        timeAttackState = TimeAttack_End;
      } else {
        timeAttackState = TimeAttack_Check;
      }
      break;
    case TimeAttack_End:
      setInput(); // REMOVE AFTER TESTING
      if (inputCommands[buttonA]) {
        timeAttackDone = 1;
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
      if (timeAttackDisplayCnt < commandSizeDifficulty) {
        Serial.print("\r\n"); Serial.print(commands[timeAttackDisplayCnt].inputType); 
      }
      break;
    case TimeAttack_Check:
      if (commands[timeAttackDisplayCnt].checkType()) { // Now no Fail Check
        Serial.print(F("\r\nCorrect!"));
        timeAttackDisplayCnt++;
        timeAttackPoints++;
      }
      clearInputs(); // REMOVE AFTER TESTING
      break;
    case TimeAttack_End:
      // display image of score with variable
      break;
  }
  return timeAttackState;
}

unsigned char infiniteBuffer = 2;
unsigned char infiniteBufferCnt = 0;

unsigned char infiniteSizeDifficulty = 5;
const int infinteMaxSize = 50;
unsigned char infiniteDisplayCnt = 0;
unsigned char infinitePoints = 0;
unsigned char infiniteScore = 0;

//Command* infiniteCommands = new Command[infiniteSizeDifficulty];
Command infiniteCommands[infinteMaxSize];

enum Infinite_State {Infinite_INIT, Infinite_Off, Infinite_Load, Infinite_Show, Infinite_Check, Infinite_End};
int Infinite_Tick(int infiniteState) {
  switch(infiniteState) {
    case Infinite_INIT:
      infiniteState = Infinite_Off;
      break;
    case Infinite_Off:
      if (gameInfinite) {
        infiniteDone = 0;
        infinitePoints = 0;
        infiniteScore = 0;
        infiniteState = Infinite_Load;
      } else {
        infiniteState = Infinite_Off;
      }
      break;
    case Infinite_Load:
      infiniteDisplayCnt = 0;
      Serial.print(F("\r\nShowing Sequence"));
      infiniteState = Infinite_Show;
      break;
    case Infinite_Show:
      if (infiniteDisplayCnt < infiniteSizeDifficulty) {
        infiniteCommands[infiniteDisplayCnt].clearImage();
        infiniteDisplayCnt++;
        infiniteState = Infinite_Show;
      } else if (infiniteDisplayCnt >= infiniteSizeDifficulty) {
        infiniteBufferCnt = 0;
        infiniteDisplayCnt = 0;
        Serial.print("\r\nGO!!!");
        infiniteState = Infinite_Check;
      } else {
        infiniteState = Infinite_Show;
      }
      break;
    case Infinite_Check:
      Serial.print(F("\r\nChecking..."));
      setInput(); // REMOVE AFTER TESTING
      if (infiniteDisplayCnt >= infiniteSizeDifficulty) { // finish + increase difficulty
        Serial.print(F("\r\nGreat! Increasing Difficulty"));
        infiniteScore++;
        infiniteSizeDifficulty++;
        infiniteState = Infinite_Load;
      } else if (!infiniteCommands[infiniteDisplayCnt].checkType()) { // incorrect
        if (infiniteBufferCnt >= infiniteBuffer) { // timed buffer for correct input
          Serial.print(F("\r\nIncorrect, Game Over!"));
          Serial.print(F("\r\nScore: ")); Serial.print(infiniteScore);
          Serial.print(F("\r\nPoints: ")); Serial.print(infinitePoints);
          infiniteState = Infinite_End;
        } else {
          infiniteBufferCnt++;
          infiniteState = Infinite_Check;
        }
      } else if (infiniteCommands[infiniteDisplayCnt].checkType()) { // correct
        Serial.print(F("\r\nCorrect!"));
        infinitePoints++;
        infiniteDisplayCnt++;
        infiniteState = Infinite_Check;
      } else {
        infiniteState = Infinite_Check;
      }
      break;
    case Infinite_End:
      setInput(); // REMOVE AFTER TESTING
      if (inputCommands[buttonA]) {
        infiniteDone = 1;        
        infiniteState = Infinite_Off;
      } else {
        infiniteState = Infinite_End;
      }
      break;
  }
  switch(infiniteState) {
    case Infinite_INIT:
      break;
    case Infinite_Off:
      break;
    case Infinite_Load:
      // Makes new array to display from all possible inputs, randomly chosen
      for (int i = 0; i < infiniteSizeDifficulty; i++) {
        int newCommand = rand() % inputCommandSize;
        infiniteCommands[i].inputType = (newCommand != buttonHome) ? inputStrings[newCommand] : inputStrings[newCommand + 1];
      }
      break;
    case Infinite_Show:
      infiniteCommands[infiniteDisplayCnt].displayImage();
      if (infiniteDisplayCnt < infiniteSizeDifficulty) {
        Serial.print("\r\n"); Serial.print(infiniteCommands[infiniteDisplayCnt].inputType); 
      }
      break;
    case Infinite_Check:
      clearInputs(); // REMOVE AFTER TESTING
      break;
    case Infinite_End:
      // display image of score with varable
      break;
  }
  return infiniteState;
}


void setup() {
  Serial.begin(115200);
  
  unsigned char i = 0;
  tasks[i].state = Game_INIT;
  tasks[i].period = 100; 
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Game_Tick;
  i++;
  tasks[i].state = TimeAttack_INIT;
  tasks[i].period = 100; 
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &TimeAttack_Tick;
  i++;
  tasks[i].state = Infinite_INIT;
  tasks[i].period = 100; 
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &Infinite_Tick;
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