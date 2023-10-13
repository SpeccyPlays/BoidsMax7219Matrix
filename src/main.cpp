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
uint8_t amountOfBoids = 0;
void boidSetup(boidSingle *array);
void showBoids(boidSingle *array);
void firstRule(boidSingle *array);
void secondRule(byte &x, byte &y, int16_t &angle, byte &velocity, boidSingle *array);

void setup() {
  // put your setup code here, to run once:
  display.init();
  SCREENWIDTH = display.getModulesWide() * 8;
  SCREENHEIGHT = display.getModulesHigh() * 8;
  amountOfBoids = sizeof(boidArray) / sizeof(boidArray[0]);//size of boid array
  boidSetup(boidArray);
  firstRule(boidArray);
}

void loop() {
  // put your main code here, to run repeatedly:
  display.wipeScreenBuffer();
  showBoids(boidArray);
  display.sendScreenBuffer();
  delay(10);
  firstRule(boidArray);
}

void findAngleBetweenPoints(byte &x, byte &y, int16_t &angle){
  /*
  Find the angle between the boid we're at and the center mass
  Maths from https://www.gamedev.net/forums/topic/318640-how-to-get-angle-between-two-points/
  because I hate maths
  */
  double n1 = sqrt(x*x + y*y), n2 = sqrt(globalAverageX*globalAverageX + globalAverageY * globalAverageY);
  angle = acos((x * globalAverageX + y*globalAverageY)/(n1*n2)) * 180 / PI;
}
void firstRule(boidSingle *array){
  /*
  First boid rule is to establish the center mass of all boids
  */
  for (byte i = 0; i < amountOfBoids; i++){
    globalAverageX += array[i].x;
    globalAverageY += array[i].y;
  }
  globalAverageX = (globalAverageX / (amountOfBoids - 1) / 2);
  globalAverageY = (globalAverageY / (amountOfBoids - 1) / 2);
}
void secondRule(byte &x, byte &y, int16_t &angle, byte &velocity, boidSingle *array){
  /*
  Second rule of boid is to avoid other boids
  Third rule is to match velocity of close boids
  */
  byte avoidence = 1;
  byte avoidenceAngle = 15;
  byte closeBoidCount = 0;
  byte closeBoidVelocities = 0;
  for (byte i = 0; i < amountOfBoids; i++){
    if ((y + avoidence == array[i].y)){
      closeBoidCount ++;
      closeBoidVelocities += array[i].velocity;
      angle -= avoidenceAngle;
    }
    if ((y - avoidence == array[i].y)){
      closeBoidCount ++;
      closeBoidVelocities += array[i].velocity;
      angle -= avoidenceAngle;
    }
    if (x + avoidence == array[i].x){
      closeBoidCount ++;
      closeBoidVelocities += array[i].velocity;
      angle -= avoidenceAngle;
    }
    if (x - avoidence == array[i].x){
      closeBoidCount ++;
      closeBoidVelocities += array[i].velocity;
      angle += avoidenceAngle;
    }
  }
  byte temp = closeBoidVelocities / closeBoidCount;
  if (temp > 0){
    velocity = closeBoidVelocities / closeBoidCount;
  }
  else {
    velocity = 1;
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
		findAngleBetweenPoints(array[i].x, array[i].y, array[i].angle);
    //not sure what the below is actually doing
    //moving toward the center mass I think
		array[i].y += array[i].velocity * sin(array[i].angle);
		array[i].x += array[i].velocity * cos(array[i].angle);	
		if (array[i].x <= 0){
			array[i].x = SCREENWIDTH -1;
			array[i].velocity = 1;
		}
		else if (array[i].x >= SCREENWIDTH){
			array[i].x = 1;
			array[i].velocity = 1;
		}
		if (array[i].y <= 0){
			array[i].y = SCREENHEIGHT -1;
			array[i].velocity = 1; 
		}
		else if (array[i].y > SCREENHEIGHT){
			array[i].y = 1;
			array[i].velocity = 1;
		}
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