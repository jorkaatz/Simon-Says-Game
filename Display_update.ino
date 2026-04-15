#include <TVout.h>
#include <fontALL.h>
#include <avr/pgmspace.h>
#include <Wire.h>

#define SLAVE_ADDR 9

TVout TV;

const char PROGMEM ph[] = "Phase Clan\n";
const char PROGMEM pr[] = "Presents\n";
const char PROGMEM th[] = "THE SIMON SAYS GAME";
const char PROGMEM men[] = "Enter 0 for timed mode.\nEnter 1 for infinite mode.";
const char PROGMEM timed[] = "Timed Mode. Get as \nmany correct entries\nas you can in the \nallotted time";
const char PROGMEM infinite[] = "Infinite Mode. Get as \nmany correct motions\nas you can in the \nallotted time";
const char PROGMEM aButton[] = "\n\n\n\n\n\n              A";
const char PROGMEM bButton[] = "\n\n\n\n\n\n              B";
const char PROGMEM hButton[] = "\n\n\n\n\n\n              H";
const char PROGMEM mButton[] = "\n\n\n\n\n\n              -";
const char PROGMEM pButton[] = "\n\n\n\n\n\n              +";
const char PROGMEM oneButton[] = "\n\n\n\n\n\n              1";
const char PROGMEM twoButton[] = "\n\n\n\n\n\n              2";
const char PROGMEM sup[] = "\n\n\n\n\n\n              Swing up!";
const char PROGMEM sdo[] = "\n\n\n\n\n\n              Swing down!";
const char PROGMEM sle[] = "\n\n\n\n\n\n              Swing left!";
const char PROGMEM sri[] = "\n\n\n\n\n\n              Swing right!";
const char PROGMEM sfo[] = "\n\n\n\n\n\n              Swing forward!";
const char PROGMEM sba[] = "\n\n\n\n\n\n              Swing backward!";
const char PROGMEM rdy[] = "\n\n\n\n\n\n              READY";
const char PROGMEM set[] = "\n\n\n\n\n\n              SET";
const char PROGMEM go[] = "\n\n\n\n\n\n              GO!!!";
const char PROGMEM lost[] = "\n\n\n\n\n\n              YOU LOST!!!";
const char PROGMEM won[] = "\n\n\n\n\n\n              YOU WON!!!";
void drawButton(byte but) {
   char buffer[20]; 
  const char* label;

  switch (but) {
    case 0:  //A button
      label = aButton;
      break;
    case 1:  // - button
      label = mButton;
      break;
    case 2:  // + button
      label = pButton;
      break;
    case 3:  // 1 button
      label = oneButton;
      break;
    case 4:  // 2 button
      label = twoButton;
      break;
    case 5: //B button
      label = bButton;
      break;
    case 6: //Home button
      label = hButton;
      break;
    default:
      return; // Invalid button
  }

  strcpy_P(buffer, (char*)label); // Copy label from flash memory
  TV.println(buffer);
  TV.draw_circle(57, 39, 6, WHITE);
}

void drawArrow(byte dir) {
  uint8_t x = 40, y = 30, w = 35, h = 15;
  uint8_t xCen = x + w / 2, yCen = y + h / 2, temp;
  uint8_t lineOff = 7, pointOff = 13;
  uint8_t lx0, ly0, lx1, ly1, mx0, my0, mx1, my1, nx0, ny0, nx1, ny1;

  switch (dir) {
    case 5:  //up arrow
      y = yCen - w / 2;
      x = xCen - h / 2;
      temp = w;
      w = h;
      h = temp;
      lx0 = x - lineOff; ly0 = y; lx1 = x + w + lineOff; ly1 = y;
      mx0 = lx0; my0 = ly0; mx1 = xCen; my1 = my0 - pointOff;
      nx0 = lx1; ny0 = ly1; nx1 = mx1; ny1 = my1;
      break;
    case 6:  //right arrow
      break;
    case 7:  //down arrow
      y = yCen - w / 2;
      x = xCen - h / 2;
      temp = w;
      w = h;
      h = temp;
      lx0 = x - lineOff; ly0 = y + h; lx1 = x + lineOff + w; ly1 = y + h;
      mx0 = lx0; my0 = ly0; mx1 = xCen; my1 = ly1 + pointOff;
      nx0 = lx1; ny0 = ly1; nx1 = mx1; ny1 = my1;
      break;
    case 8:  //left arrow
      lx0 = x; ly0 = y - lineOff; lx1 = x; ly1 = y + h + lineOff;
      mx0 = lx0; my0 = ly0; mx1 = lx0 - pointOff; my1 = yCen;
      nx0 = lx1; ny0 = ly1; nx1 = mx1; ny1 = my1;
      break;
  }
}
void receiveEvent(int numBytes) {
  if (numBytes > 0) {
    int b = Wire.read();
    handleInput(b);
    Serial.println(b);
  }
}
void handleInput(int input) {
  TV.clear_screen();
  
  char buffer[25];
  switch(input){

      case 0:
        drawArrow(5);
        break;
      case 1:
        drawArrow(7);
        break;
      case 2:
        drawArrow(8);
        break;
      case 3:
        drawArrow(6);
        break;
      case 4:
        drawButton(2);
        break;
      case 5:
        drawButton(1);
        break;
      case 6:
        drawButton(3);
        break;
      case 7:
        drawButton(4);
        break;
      case 8:
        drawButton(0);
        break;
      case 9:
        drawButton(5);
        break;
      case 10:
        drawButton(6);
        break;
      case 11:
        strcpy_P(buffer, sup);
        TV.println(buffer);
        break;
      case 12:
        strcpy_P(buffer, sdo);
        TV.println(buffer);
        break;
      case 13:
        strcpy_P(buffer, sfo);
        TV.println(buffer);
        break; 
      case 14:
        strcpy_P(buffer, sba);
        TV.println(buffer);
        break;
      case 15: 
        strcpy_P(buffer, sle);
        TV.println(buffer);
        break;
      case 16:
        strcpy_P(buffer, sri);
        TV.println(buffer);
        break;
      case 17:
        strcpy_P(buffer, men);
        TV.println(buffer);
        break;
      case 18:
        strcpy_P(buffer, timed);
        TV.println(buffer);
        break;
      case 19:
        strcpy_P(buffer, infinite);
        TV.println(buffer);
        break;
      
      
  }
  
}
void setup() {
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  TV.begin(NTSC, 120, 96);
  TV.select_font(font4x6);
}

void loop() {
  // Empty loop
}





