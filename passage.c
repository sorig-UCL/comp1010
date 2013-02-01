#include <stdio.h>
#include "APIWrapper.h"

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
    sendCommand("I LR -25 25");
    
    SensorValue frontInfrareds, ultraSound;
    SensorValue *list = NULL;
    
    sensorRead(SensorTypeUS, &ultraSound);
    
    while (ultraSound.values[0] > 20)
    {        
        sensorRead(SensorTypeIFLR, &frontInfrareds);
        sensorRead(SensorTypeUS, &ultraSound);
        
        infraredsToDist(&frontInfrareds, SensorTypeIFLR);
        
        double ratio = (double)frontInfrareds.values[RIGHT] / (double)frontInfrareds.values[LEFT];
        
        driveRobotAndRecord(0.001, 40, ratio, &list);
    }
    
    playBackRecording(&list, 40);
}

int main()
{
    //setIPAndPort("128.16.80.185", 55443);
	connectAndGetSocket();
    
    goThroughPassage();
}