#include <stdio.h>
#include <stdlib.h>
#include "APIWrapper.h"

#define MIN(A, B) (A < B ? A : B)
#define MAX(A, B) (A > B ? A : B)

void goThroughPassage()
{    
    sendCommand("I LR -25 25");
    
    SensorValue frontInfrareds, ultraSound;
    SensorValue *list = NULL;
    
    sensorRead(SensorTypeUS, &ultraSound);
    
    while (ultraSound.values[0] > 30)
    {
        sensorRead(SensorTypeIFLR, &frontInfrareds);
        sensorRead(SensorTypeUS, &ultraSound);
        
        infraredsToDist(&frontInfrareds, SensorTypeIFLR);
        
        double ratio = (double)frontInfrareds.values[RIGHT] / (double)frontInfrareds.values[LEFT];
        
        driveRobotAndRecord(0.001, 20, ratio, &list);
    }
    
    playBackRecording(&list, 10);
}

int main()
{
    //setIPAndPort("128.16.79.9", 55443);
	connectAndGetSocket();
    
    sleep(2);
    
    //driveRobot(1.0, 20, 1.0);
    goThroughPassage();
}