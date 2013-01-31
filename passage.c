#include <stdio.h>
#include "APIWrapper.h"

#define LEFT 1
#define RIGHT 0
#define MIN(A, B) (A < B ? A : B)
#define MAX(A, B) (A > B ? A : B)

void bumperCheck(int side)
{
    SensorValue bumpers;
    sensorRead(SensorTypeBFLR, &bumpers);
    
    int bumpedIntoSomething = bumpers.values[0] || bumpers.values[1];
    if (bumpedIntoSomething) {
        driveRobot(-1.0, 40.0, 1.0);
        
        if (side == RIGHT) {
            turnRobot(-45);
        }
        else {
            turnRobot(45);
        }
    }
}

void goThroughPassage()
{    
    sendCommand("I LR -20 20");
    
    SensorValue frontInfrareds, sideInfrareds, ultraSound;
    
    
    while (ultraSound.values[0] > 10)
    {
        //bumperCheck(side);
        
        sensorRead(SensorTypeIFLR, &frontInfrareds);
        sensorRead(SensorTypeISLR, &sideInfrareds);
        
        infraredsToDist(&frontInfrareds, SensorTypeIFLR);
        infraredsToDist(&sideInfrareds, SensorTypeISLR);
        
        double rightAverageDist = (frontInfrareds.values[RIGHT] + sideInfrareds.values[RIGHT]) / 2.0;
        double leftAverageDist = (frontInfrareds.values[LEFT] + sideInfrareds.values[LEFT]) / 2.0;
        
        double ratio = rightAverageDist / leftAverageDist;
                
        /*int frontDistance = frontInfrareds.values[side];
        int sideDistance = sideInfrareds.values[side];
        
        int minDistance = MIN(frontDistance, sideDistance);
        int outOfRange = minDistance >= 40;
        
        double ratio;
        if (side == RIGHT) {
            ratio = (double)frontDistance/(double)sideDistance;
        }
        else {
            ratio = (double)sideDistance/(double)frontDistance;
        }
        
        // Stabilise the distance to the wall around the specified value (e.g. 20 cm)
        ratio += ((minDistance-distanceToWall)/25.0) * (side == LEFT ? -1 : 1);
        
        if (outOfRange) {
            ratio = 1.0;
        }*/
        
        driveRobot(0.01, 10, ratio);
        
    }
}

int main()
{
    //setIPAndPort("128.16.80.185", 55443);
	connectAndGetSocket();
    
    goThroughPassage();
}