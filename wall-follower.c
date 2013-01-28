#include <stdio.h>
#include "APIWrapper.h"

#define LEFT 1
#define RIGHT 0
#define MIN(A, B) (A < B ? A : B)

int infraredsToDist(SensorValue *sensorValue, SensorType type)
{
    int (*converterFunction)(int);
    if (type == SensorTypeIFLR)
    {
        converterFunction = &gp2d12_to_dist;
    }
    else if (type == SensorTypeISLR)
    {
        converterFunction = &gp2d120_to_dist;
    }
    else
    {
        return -1;
    }
    
    int i;
    for (i = 0; i < sensorValue->length; i++) {
        sensorValue->values[i] = (*converterFunction)(sensorValue->values[i]);
    }
    
    return 0;
}

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

void followWall(int side)
{
    if (side == RIGHT) {
        sendCommand("I LR 45 45");
    }
    else {
        sendCommand("I LR -45 -45");
    }
    
    SensorValue frontInfrareds, sideInfrareds;
    int distanceToWall = 20;
    
    while (1)
    {
        bumperCheck(side);
        
        sensorRead(SensorTypeIFLR, &frontInfrareds);
        sensorRead(SensorTypeISLR, &sideInfrareds);
        
        infraredsToDist(&frontInfrareds, SensorTypeIFLR);
        infraredsToDist(&sideInfrareds, SensorTypeISLR);
        
        if (frontInfrareds.values[!side] < 40) {
            turnRobot(10 * (side == RIGHT ? -1 : 1));
        }
        else
        {
            int frontDistance = frontInfrareds.values[side];
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
            
            ratio += ((minDistance-distanceToWall)/25.0) * (side == LEFT ? -1 : 1);
            
            if (outOfRange) {
                ratio = 1.0;
            }
            
            driveRobot(0.1, 40, ratio);
        }
    }
}

int main()
{
    //setIPAndPort("128.16.80.185", 55443);
	connectAndGetSocket();
    
    followWall(LEFT);
}