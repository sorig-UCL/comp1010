#include <stdio.h>
#include <stdlib.h>
#include "APIWrapper.h"

#define MIN(A, B) (A < B ? A : B)
#define MAX(A, B) (A > B ? A : B)

void goThroughPassage()
{
    sendCommand("C RME");
    sendCommand("I LR -25 25");
    
    SensorValue frontInfrareds, ultraSound;
    
    sensorRead(SensorTypeUS, &ultraSound);
    
    int noWall = 100;
    
    while (ultraSound.values[0] > 27 && (frontInfrareds.values[RIGHT] < noWall || frontInfrareds.values[LEFT] < noWall))
    {
        sensorRead(SensorTypeIFLR, &frontInfrareds);
        sensorRead(SensorTypeUS, &ultraSound);
        
        infraredsToDist(&frontInfrareds, SensorTypeIFLR);
        
        double ratio = (double)frontInfrareds.values[RIGHT] / (double)frontInfrareds.values[LEFT];
        
        driveRobot(0.001, 60, ratio);
    }
}

int main()
{
    //setIPAndPort("128.16.79.9", 55443);
	connectAndGetSocket();
    
    sleep(2);
    
    //driveRobot(1.0, 20, 1.0);
    goThroughPassage();
    turnRobot(180);
    goThroughPassage();
}