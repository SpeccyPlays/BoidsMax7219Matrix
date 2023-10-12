#include <Arduino.h>
#include "LedMatrix.h"

LedMatrix display(4, 3);
byte SCREENWIDTH = 0;
byte SCREENHEIGHT= 0;

struct boidSingle{
  byte x;
  byte y;
  byte oldX;
  byte oldY;
  byte velocity;
  int16_t angle;
};
boidSingle boidArray[25];
uint32_t globalAverageX = 0;
uint32_t globalAverageY = 0;
uint8_t amountOfBoids = sizeof(boidArray) / sizeof(boidArray[0]);
void boidSetup(boidSingle *array);
void showBoids(boidSingle *array);
void firstRule(boidSingle *array);
void secondRule(byte &x, byte &y, int16_t &angle, boidSingle *array);

void setup() {
  // put your setup code here, to run once:
  display.init();
  SCREENWIDTH = display.getModulesWide() * 8;
  SCREENHEIGHT = display.getModulesHigh() * 8;
  boidSetup(boidArray);
  firstRule(boidArray);
}

void loop() {
  // put your main code here, to run repeatedly:
  display.wipeScreenBuffer();
  showBoids(boidArray);
  display.sendScreenBuffer();
  delay(50);
  firstRule(boidArray);
}

void findAngleBetweenPoints(byte &x, byte &y, int16_t &angle){
// find dot product
//i hate maths
//  uint32_t dotProduct = (x * globalAverageX) + (y * globalAverageY);
  double magnitudeOld = sqrt(sq(x) + sq(y));
  double magnitudeNew = sqrt(sq(globalAverageX) + sq(globalAverageY));
//  double a = dotProduct / (magnitudeOld * magnitudeNew);
  double a = /*tan*/tan((x * globalAverageX + y * globalAverageY) / (magnitudeOld  - magnitudeNew));
  angle = (a * 180) / 3.141;//this should be cos(a) but what's there works better
//  if (angle > 0){ 
//    angle += 180;
//  }
//  Serial.println(angle);
}
void firstRule(boidSingle *array){
  for (byte i = 0; i < amountOfBoids; i++){
    globalAverageX += array[i].x;
    globalAverageY += array[i].y;
  }
  globalAverageX = (globalAverageX / (amountOfBoids - 1));
  globalAverageY = (globalAverageY / (amountOfBoids - 1));
}
void secondRule(byte &x, byte &y, int16_t &angle, boidSingle *array){
  for (byte i = 0; i < amountOfBoids; i++){
    if ((y + 3 == array[i].y) | (x + 3 == array[i].x)){
      y -= 1;
      x -= 1;
      array[i].velocity = 10;
//      angle = array[i].angle;
    }
    if ((y - 3 == array[i].y) | (x - 3 == array[i].x)){
      y += 1;
      x += 1;
      array[i].velocity = 10;
//      angle = array[i].angle;
    }
  }
}
void showBoids(boidSingle *array){
  for (byte i = 0; i < amountOfBoids; i++){
    display.drawPixel(array[i].x, array[i].y);
    array[i].oldX = array[i].x;
    array[i].oldY = array[i].y;
    array[i].velocity ++;
    findAngleBetweenPoints(array[i].x, array[i].y, array[i].angle);
    int16_t rotation = 360 - array[i].angle;
    int16_t angleToRad = (rotation * 3.141) / 180;
    array[i].y += array[i].velocity * sin(angleToRad);
    array[i].x += array[i].velocity * cos(angleToRad);
    if (array[i].x <= 0){
      array[i].x =+ SCREENWIDTH;
      array[i].velocity = 10;
    }
    if (array[i].x > SCREENWIDTH){
      array[i].x = 0;//=- SCREENWIDTH;
      array[i].velocity = 10;
    }
    if (array[i].y <= 0){
      array[i].y =+ SCREENHEIGHT -2;
      array[i].velocity = 10;
    }
    if (array[i].y >= SCREENHEIGHT){
      array[i].y =- SCREENHEIGHT - 2;
      array[i].velocity = 10;
    }
    secondRule(array[i].x, array[i].y, array[i].angle, boidArray);
  }
}
void boidReset(byte &x, byte &y, byte &velocity, int16_t &angle){
  y = random(0, SCREENHEIGHT - 1);
  x = random(0, SCREENWIDTH - 1);
  velocity = random(10, 20);
  angle = random(0, 359);
}
void boidSetup(boidSingle *array){
  for (byte i = 0; i < amountOfBoids; i++){
    boidReset(array[i].x, array[i].y, array[i].velocity, array[i].angle);
  }
}