#include <TVout.h>
#include <fontALL.h>
#include <avr/pgmspace.h>
#include <Wire.h>

#define I2C_SLAVE
#define SLAVE_ADDR 9
//#define ANSWERSIZE 1
//bool response = 0;


const char PROGMEM ph[] = "Phase Clan\n";
const char PROGMEM pr[] = "Presents\n";
const char PROGMEM th[] = "THE SIMON SAYS GAME";
const char PROGMEM men[] = "Enter 1 for timed mode.\nEnter 2 for infinite mode.";
const char PROGMEM timed[] = "Timed Mode. Get as \nmany correct entries\nas you can in the \nallotted time";
const char PROGMEM infinite[] = "Infinite Mode. Play \nas long as you want.\nPress home to \ngo back";
const char PROGMEM aButton[] = "\n\n\n\n\n\n              A";
const char PROGMEM bButton[] = "\n\n\n\n\n\n              B";
const char PROGMEM hButton[] = "\n\n\n\n\n\n              H";
const char PROGMEM mButton[] = "\n\n\n\n\n\n              -";
const char PROGMEM pButton[] = "\n\n\n\n\n\n              +";
const char PROGMEM oneButton[] = "\n\n\n\n\n\n              1";
const char PROGMEM twoButton[] = "\n\n\n\n\n\n              2";
const char PROGMEM sup[] = "\n\n\n\n\n\n          Swing up!";
const char PROGMEM sdo[] = "\n\n\n\n\n\n          Swing down!";
const char PROGMEM sle[] = "\n\n\n\n\n\n          Swing left!";
const char PROGMEM sri[] = "\n\n\n\n\n\n          Swing right!";
const char PROGMEM sfo[] = "\n\n\n\n\n\n          Swing forward!";
const char PROGMEM sba[] = "\n\n\n\n\n\n          Swing backward!";
const char PROGMEM rdy[] = "\n\n\n\n\n\n              READY";
const char PROGMEM set[] = "\n\n\n\n\n\n              SET";
const char PROGMEM go[] = "\n\n\n\n\n\n              GO!!!";
const char PROGMEM lost[] = "\n\n\n\n\n\n              YOU LOST!!!";
const char PROGMEM won[] = "\n\n\n\n\n\n              YOU WON!!!";

TVout TV;

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
  uint8_t x = 40, y = 30, w = 35, h = 15;  //rectangle starting point and W/H
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
      lx0 = x + w; ly0 = y - lineOff; lx1 = x + w; ly1 = y + h + lineOff;
      mx0 = lx0; my0 = ly0; mx1 = lx0 + pointOff; my1 = yCen;
      nx0 = lx1; ny0 = ly1; nx1 = mx1; ny1 = my1;
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

  TV.draw_rect(x, y, w, h, WHITE, WHITE);  // Draw rotated rectangle
  TV.draw_line(lx0, ly0, lx1, ly1, WHITE);
  TV.draw_line(mx0, my0, mx1, my1, WHITE);
  TV.draw_line(nx0, ny0, nx1, ny1, WHITE);
}

void setup() {
  



  //Serial.begin(9600);
  TV.begin(NTSC, 120, 96);
  TV.select_font(font4x6);
 // TV.println("TEST");

}
int count = -1;
void receiveEvent(){

  while(0<Wire.available()){
     int b = Wire.read();
     

    count = b;
    //Serial.println(count);
    
  }
  //Serial.println(count);
  
}

enum EX_States { preReady, ready, pre, pre1, pre2,
                 menu, timedMode,infiniteMode, r, s, 
                 g, home, a, b, m, 
                 p, one, two, up, down, 
                 left, right, su, sd, sl, sr, sf, sb,
                 gameWon,gameOver } EX_State;


 int transition(int count){
  //count = 3;
  switch(count){
      case -1:
        return preReady;
      case 0:
        return up;
      case 1:
        return down;
      case 2:
        return left;
      case 3:
        return right;
      case 4:
        return p;
      case 5:
        return m;
      case 6:
        return one;
      case 7:
        return two;
      case 8:
        return a;
      case 9:
        return b;
      case 10:
        return home;
      case 11:
        return su;
      case 12:
        return sd;
      case 13:
        return sf;
      case 14:
        return sb;
      case 15: 
        return sl;
      case 16:
        return sr;
      case 17:
        return menu;
      case 18:
        return timedMode;
      case 19:
        return infiniteMode;
      
      
  }
    return preReady;  


  
}

void EX_Tick() {
  char buffer[20];

  switch (EX_State) {
    case preReady:
        if (count != -1){
          EX_State = transition(count);
          TV.clear_screen();
        }
        
      break;
    case ready:

        

        //EX_State = transition(count); 

      break;
    
    case pre:
        EX_State = preReady; 
        count = -1;    
      break;

    case pre1:
        EX_State = preReady;
        count = -1;  
      break;
    
    case pre2:
        EX_State = preReady;
        count = -1;  
      break;
    
    case menu:
        EX_State = preReady;
        count = -1;  
      break;
    
    case timedMode:
        EX_State = preReady;
        count = -1;  
      break;
    
    case infiniteMode:
        EX_State = preReady;
        count = -1;  
      break;
    
    case r:
        EX_State = preReady;
        count = -1;  
      break;
    
    case s:
        EX_State = preReady;
        count = -1;  
      break;
    
    case g:
        EX_State = preReady;
        count = -1;  
      break;
    
    case a:
        EX_State = preReady;
        count = -1;  
      break;
    case b:
        EX_State = preReady;
        count = -1;  
      break;
     
    case home:
        EX_State = preReady;
        count = -1;  
      break;   
    case m:
        EX_State = preReady;
        count = -1;  
      break;
    
    case p:
        EX_State = preReady;
        count = -1;  
      break;
    
    case one:
        EX_State = preReady;
        count = -1;  
      break;
    
    case two:
        EX_State = preReady;
        count = -1;  
      break;
    
    case up:
        EX_State = preReady;
        count = -1;  
      break;
    
    case down:
        EX_State = preReady;
        count = -1;  
      break;
    
    case left:
        EX_State = preReady;
        count = -1;  
      break;
    
    case right:
        EX_State = preReady;
        count = -1;  
      break;
    
    case su:
        EX_State = preReady;
        count = -1;  
      break;
    
    case sd:
        EX_State = preReady;
        count = -1;  
      break;
    
    case sl:
        EX_State = preReady;
        count = -1;  
      break;
    
    case sr:
        EX_State = preReady;
        count = -1;  
      break;
    
    case sf:
         EX_State = preReady;
         count = -1;  
      break;
    
    case sb:
        EX_State = preReady;
        count = -1;  
      break;
    
    case gameWon:
        EX_State = preReady;
        count = -1;  
      break;
    
    case gameOver:
        EX_State = preReady;
        count = -1;  
      break;
    
    default:

      break;
  }

  switch (EX_State) {
    
    case preReady:
        //does nothing to screen, transitions only
      break;

    case ready:
        //does nothing to screen, transitions only
      break;
    
    case pre:
      
      strcpy_P(buffer, ph);
      TV.println(buffer);
      break;

    case pre1:

      
      strcpy_P(buffer, pr);
      TV.println(buffer);
      break;
    
    case pre2:

    
        strcpy_P(buffer, th);
        TV.println(buffer);
      break;
    
    case menu:
       
        strcpy_P(buffer, men);
        TV.println(buffer);
      break;
    
    case timedMode:
       
        strcpy_P(buffer, timed);
        TV.println(buffer);
      break;
    
    case infiniteMode:
        
        strcpy_P(buffer, infinite);
        TV.println(buffer);
      break;
    
    case r:
        
        strcpy_P(buffer, rdy);
        TV.println(buffer);
      break;
    
    case s:
       
        strcpy_P(buffer, set);
        TV.println(buffer);
      break;
    
    case g:
        
        strcpy_P(buffer, go);
        TV.println(buffer);
      break;
    
    case a:
        drawButton(0);
      break;
          
    case b:
        drawButton(5);
      break;
          
    case home:
        drawButton(6);
      break;
    
    case m:
        drawButton(1);
      break;
    
    case p:
        drawButton(2);
      break;
    
    case one:
        drawButton(3);
      break;
    
    case two:
        drawButton(4);
      break;
    
    case up:
        drawArrow(5);
      break;
    
    case down:
        drawArrow(7);
      break;
    
    case left:
        drawArrow(8);
      break;
    
    case right:
        drawArrow(6);
      break;
    
    case su:
       
        strcpy_P(buffer, sup);
        TV.println(buffer);
      break;
    
    case sd:
       
        strcpy_P(buffer, sdo);
        TV.println(buffer);
      break;
    
    case sl:
       
        strcpy_P(buffer, sle);
        TV.println(buffer);
      break;
    
    case sr:
       
        strcpy_P(buffer, sri);
        TV.println(buffer);
      break;
    case sf:
        
        strcpy_P(buffer, sfo);
        TV.println(buffer);      
      break;

    case sb:
      
        strcpy_P(buffer, sba);
        TV.println(buffer);
        break;

    case gameWon:
    
        strcpy_P(buffer, won);
        TV.println(buffer);        
      break;
    
    case gameOver:
        
        strcpy_P(buffer, lost);
        TV.println(buffer);
      break;
    
    default:

      break;
  }
}




void loop() {
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  
  EX_State = preReady;
  TV.clear_screen();
  
  //int t = 0;
  while (1) {
    //TV.println(count);
    //TV.print(t);
    //delay(10000);
    EX_Tick();
        //if(count == -1){
          //count = t;
          //t++;
        //}
    
    //if (t == 20){
     // t = -1;
    //}

  }
  
}
