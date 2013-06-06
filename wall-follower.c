#include <stdio.h>
#include "APIWrapper.h"

#define MIN(A, B) (A < B ? A : B)

void bumperCheck(int side)
{
    SensorValue bumpers;
    sensorRead(SensorTypeBFLR, &bumpers);
    
    int bumpedIntoSomething = bumpers.values[0] || bumpers.values[1];
    if (bumpedIntoSomething) {
        driveRobot(-1.0, 40.0, 1.0);
        
        if (side == RIGHT) {
            turnRobot(-45, 10);
        }
        else {
            turnRobot(45, 10);
        }
    }
}

int abs(int a)
{
    if (a < 0) {
        return a * -1;
    }
    return a;
}

void followWall(int side, int degrees)
{    
    int stopAndSearchDistance = 30;
    
    if (side == RIGHT) {
        sendCommand("I LR 45 45");
    }
    else {
        sendCommand("I LR -45 -45");
    }
    
    SensorValue frontInfrareds, sideInfrareds;
    int distanceToWall = 20;
    SensorValue *list = NULL;
    sendCommand("C RME");
    addSensorValue(&list, createSensorValueAndRecord(SensorTypeMELR));
    
    while (abs(list->values[0] - list->values[1]) < (1690.0/360.0)*degrees)
    {
        bumperCheck(side);
        
        sensorRead(SensorTypeIFLR, &frontInfrareds);
        sensorRead(SensorTypeISLR, &sideInfrareds);
        
        infraredsToDist(&frontInfrareds, SensorTypeIFLR);
        infraredsToDist(&sideInfrareds, SensorTypeISLR);
        
        if (frontInfrareds.values[!side] < stopAndSearchDistance) {
            turnAndRecord(10 * (side == RIGHT ? -1 : 1), 1, &list);
        }
        else
        {
            int frontDistance = frontInfrareds.values[side];
            int sideDistance = sideInfrareds.values[side];
            
            int minDistance = MIN(frontDistance, sideDistance);
            int outOfRange = minDistance >= 41;
            
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
            }
            
            driveRobotAndRecord(0.01, 30, ratio, &list);
        }
    }
}

int main()
{
    //setIPAndPort("128.16.79.9", 55443);
	connectAndGetSocket();
    sleep(2);
    followWall(RIGHT, 270);
    while (1) {
        followWall(LEFT, 360);
        followWall(RIGHT, 360);
    }
    //sendCommand("I LR -45 -45");
    //sendCommand("I R -45");
}