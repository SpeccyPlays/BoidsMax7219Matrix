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
boidSingle boidArray[50];
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
  delay(20);
  firstRule(boidArray);
}

void findAngleBetweenPoints(byte &x, byte &y, int16_t &angle){
// find dot product
//i hate maths
  double magnitudeOld = sqrt(sq(x) + sq(y));
  double magnitudeNew = sqrt(sq(globalAverageX) + sq(globalAverageY));
  double a = /*tan*/tan((x * globalAverageX + y * globalAverageY) / (magnitudeOld  - magnitudeNew));
  angle = (a * 180) / 3.141;//this should be cos(a) but what's there works better
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
    if ((y + 1 == array[i].y) | (x + 1 == array[i].x)){
      array[i].velocity = 1;
      angle = -angle;
    }
    else if ((y - 1 == array[i].y) | (x - 1 == array[i].x)){
      array[i].velocity = 1;
      angle = -angle;
    }
  }
}
void showBoids(boidSingle *array){
	for (byte i = 0; i < amountOfBoids; i++){
		display.drawPixel(array[i].x, array[i].y);
		array[i].velocity ++;
		findAngleBetweenPoints(array[i].x, array[i].y, array[i].angle);
		array[i].y += array[i].velocity * sin(array[i].angle);
		array[i].x += array[i].velocity * cos(array[i].angle);	
		if (array[i].x <= 0){
			array[i].x = SCREENWIDTH -1;
			array[i].velocity = 1;
		}
		if (array[i].x >= SCREENWIDTH){
			array[i].x = 1;
			array[i].velocity = 1;
		}
		if (array[i].y <= 0){
			array[i].y = SCREENHEIGHT -1;
			array[i].velocity = 1; 
		}
		if (array[i].y > SCREENHEIGHT){
			array[i].y = 1;
			array[i].velocity = 1;
		}
		secondRule(array[i].x, array[i].y, array[i].angle, boidArray);
//		delay(500);
	}
}
void boidReset(byte &x, byte &y, byte &velocity, int16_t &angle){
  y = random(0, SCREENHEIGHT);
  x = random(0, SCREENWIDTH);
  velocity = random(1, 6);
  angle = random(0, 359);
}
void boidSetup(boidSingle *array){
  for (byte i = 0; i < amountOfBoids; i++){
    boidReset(array[i].x, array[i].y, array[i].velocity, array[i].angle);
  }
}