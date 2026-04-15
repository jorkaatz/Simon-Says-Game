#include <Wire.h>
#define SLAVE_ADDR 9

int x = 0;
void setup() {
  // put your setup code here, to run once:
  Wire.begin();

  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(x);
  Wire.endTransmission();
  x = x + 1;
  if (x == 20){
    x = -1;
  }

}
