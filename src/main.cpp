#include <Arduino.h>
#include "LedMatrix.h"

LedMatrix display(4, 3);
byte dWidthPixel = 0;
byte dHeightPixel = 0;

void setup() {
  // put your setup code here, to run once:
  display.init();
  dWidthPixel = display.getModulesWide() * 8;
  


}

void loop() {
  // put your main code here, to run repeatedly:
}