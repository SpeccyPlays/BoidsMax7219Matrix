#include <Arduino.h>
#include "LedMatrix.h"
/*
An old project to make a boids simulation for MAX7219 matrixes
Fixed all the maths so they actually flock
About 25 boids seems to give the best behaviour
Note to self - all the maths functions taken radians so no need to convert to degrees
*/
LedMatrix display(4, 3);
byte SCREENWIDTH = 0;
byte SCREENHEIGHT= 0;

struct boidSingle{
  int8_t x;
  int8_t y;
  byte velocity;
  float angle;
};
boidSingle boidArray[20];
uint32_t globalAverageX = 0;
uint32_t globalAverageY = 0;
uint8_t amountOfBoids = 0;
//these two change the boid behaviour
byte maxSpeed = 5;
float avoidenceAngle = 0.8;
//just for ease of changing the time between screen refresh
byte loopDelay = 100;
void boidSetup(boidSingle *array);
void showBoids(boidSingle *array);
void firstRule(boidSingle *array);
void secondRule(int8_t &x, int8_t &y, float &angle, byte &velocity, boidSingle *array);
void findAngleBetweenPoints(int8_t &x, int8_t &y, float &angle, uint8_t targetX, uint8_t targetY);

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
  display.wipeScreenBuffer();
  showBoids(boidArray);
  display.sendScreenBuffer();
  delay(loopDelay);
}
void findAngleBetweenPoints(int8_t &x, int8_t &y, float &angle, uint8_t targetX, uint8_t targetY){
  /*
  Find the angle between the boid we're at and the center mass
  */
  angle = atan2(targetY - y, targetX - x);// * 180 / PI;
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
void secondRule(int8_t &x, int8_t &y, float &angle, byte &velocity, boidSingle *array){
  /*
  Second rule of boid is to avoid other boids
  */
  byte avoidence = 1;
  byte neighbourCount = 0;
  byte avgNeighbourVeloctity = 0;
  for (byte i = 0; i < amountOfBoids; i++){
    if ((y + avoidence == array[i].y) && (x + avoidence == array[i].x)){
      neighbourCount ++;
      avgNeighbourVeloctity += array[i].velocity;
      angle -= avoidenceAngle;
    }
    if ((y - avoidence == array[i].y) && (x + avoidence == array[i].x)){
      angle += avoidenceAngle;
      neighbourCount ++;
      avgNeighbourVeloctity += array[i].velocity;
    }
    if((y - avoidence == array[i].y) && (x + avoidence == array[i].x)){
      angle += avoidenceAngle;
      neighbourCount ++;
      avgNeighbourVeloctity += array[i].velocity;
    }
    if ((y + avoidence == array[i].y) && (x - avoidence == array[i].x)){
      angle -= avoidenceAngle;
      neighbourCount ++;
      avgNeighbourVeloctity += array[i].velocity;
    }
  }
  if (neighbourCount > 0){
    velocity = avgNeighbourVeloctity / neighbourCount;
  } 
}
void showBoids(boidSingle *array){
  /*
  Do the stuff.
  Check if we're inside or outside the 'screen' boundaries and adjust where needed
  */
	for (byte i = 0; i < amountOfBoids; i++){
    if (array[i].velocity < maxSpeed){
      array[i].velocity ++;
    }
    if (array[i].x <= 0){
			array[i].x = SCREENWIDTH -2;
			array[i].velocity = 1;
		}
		else if (array[i].x >= SCREENWIDTH){
			array[i].x = 1;
			array[i].velocity = 1;
		}
		if (array[i].y <= 0){
			array[i].y = SCREENHEIGHT -2;
			array[i].velocity = 1; 
		}
		else if (array[i].y > SCREENHEIGHT){
			array[i].y = 1;
			array[i].velocity = 1;
		}
    firstRule(boidArray);
    /*
    Find the angle between boid current position and centre mass first
    The second rule will adjust the angle to avoid others
    */
    findAngleBetweenPoints(array[i].x, array[i].y, array[i].angle, globalAverageX, globalAverageY);
    secondRule(array[i].x, array[i].y, array[i].angle, array[i].velocity, boidArray);

    //moving toward the center mass
    array[i].y += array[i].velocity * sin(array[i].angle);
		array[i].x += array[i].velocity * cos(array[i].angle);
    display.drawPixel(array[i].x, array[i].y);
	}
}
void boidReset(int8_t &x, int8_t &y, byte &velocity, float &angle){
  /*
  Generate random values
  */
  y = random(0, SCREENHEIGHT);
  x = random(0, SCREENWIDTH);
  velocity = random(1, maxSpeed);
  angle = random(0.0, 4.7);
}
void boidSetup(boidSingle *array){
  /*
  Go through all boids and set with random values
  */
  for (byte i = 0; i < amountOfBoids; i++){
    boidReset(array[i].x, array[i].y, array[i].velocity, array[i].angle);
  }
}