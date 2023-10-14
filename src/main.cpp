#include <Arduino.h>
#include "LedMatrix.h"
/*
An old project to make a boids simulation for MAX7219 matrixes
It's not exactly as it should be but looks nice
Looks more like midges
*/
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
uint8_t amountOfBoids = 0;
void boidSetup(boidSingle *array);
void showBoids(boidSingle *array);
void firstRule(boidSingle *array);
void secondRule(byte &x, byte &y, int16_t &angle, byte &velocity, boidSingle *array);
void findAngleBetweenPoints(byte &x, byte &y, int16_t &angle, uint8_t targetX, uint8_t targetY);

void setup() {
  // put your setup code here, to run once:
  display.init();
  SCREENWIDTH = display.getModulesWide() * 8;
  SCREENHEIGHT = display.getModulesHigh() * 8;
  amountOfBoids = sizeof(boidArray) / sizeof(boidArray[0]);//size of boid array
  randomSeed(analogRead(0));
  boidSetup(boidArray);
}
void loop() {
  // put your main code here, to run repeatedly:
  firstRule(boidArray);
  display.wipeScreenBuffer();
  showBoids(boidArray);
  display.sendScreenBuffer();
  delay(20);
}
void findAngleBetweenPoints(byte &x, byte &y, int16_t &angle, uint8_t targetX, uint8_t targetY){
  /*
  Find the angle between the boid we're at and the center mass
  Maths from https://www.gamedev.net/forums/topic/318640-how-to-get-angle-between-two-points/
  because I hate maths
  */
  double n1 = sqrt(x*x + y*y);
  double n2 = sqrt(targetX*targetX + targetY * targetY);
  angle = acos((x * targetX + y*targetY)/(n1*n2)) * 180 / PI;
}
void firstRule(boidSingle *array){
  /*
  First boid rule is to establish the center mass of all boids
  */
  for (byte i = 0; i < amountOfBoids; i++){
    globalAverageX += array[i].x;
    globalAverageY += array[i].y;
  }
  globalAverageX = (globalAverageX / (amountOfBoids - 1));
  globalAverageY = (globalAverageY / (amountOfBoids - 1));
}
void secondRule(byte &x, byte &y, int16_t &angle, byte &velocity, boidSingle *array){
  /*
  Second rule of boid is to avoid other boids
  */
  byte avoidence = 1;
  byte avoidenceAngle = 45;
  for (byte i = 0; i < amountOfBoids; i++){

    if ((y + avoidence == array[i].y)){
      velocity = 1;
      angle -= avoidenceAngle;
    }
    else if ((y - avoidence == array[i].y)){
      velocity = 1;
      angle += avoidenceAngle;
    }
    if (x + avoidence == array[i].x){
      velocity = 1;
      angle -= avoidenceAngle;
    }
    else if (x - avoidence == array[i].x){
      velocity = 1;
      angle += avoidenceAngle;
    }
  }
}
void showBoids(boidSingle *array){
  /*
  Do the stuff.
  Check if we're inside or outside the 'screen' boundaries and adjust where needed
  */

	for (byte i = 0; i < amountOfBoids; i++){
		display.drawPixel(array[i].x, array[i].y);
		array[i].velocity ++;
    if (array[i].x <= 0){
			array[i].x = SCREENWIDTH -1;
			//array[i].velocity = 1;
		}
		else if (array[i].x >= SCREENWIDTH){
			array[i].x = 1;
			//array[i].velocity = 1;
		}
		if (array[i].y <= 0){
			array[i].y = SCREENHEIGHT -1;
			//array[i].velocity = 1; 
		}
		else if (array[i].y > SCREENHEIGHT){
			array[i].y = 1;
			//array[i].velocity = 1;
		}
    findAngleBetweenPoints(array[i].x, array[i].y, array[i].angle, globalAverageX, globalAverageY);
    //not sure what the below is actually doing
    //moving toward the center mass I think
    array[i].y += array[i].velocity * sin(array[i].angle);
		array[i].x += array[i].velocity * cos(array[i].angle);
    secondRule(array[i].x, array[i].y, array[i].angle, array[i].velocity, boidArray);
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