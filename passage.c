#include <stdio.h>
#include <stdlib.h>
#include "APIWrapper.h"

#define MIN(A, B) (A < B ? A : B)
#define MAX(A, B) (A > B ? A : B)

void goThroughPassage(int speed)
{    
    sendCommand("C RME");
    sendCommand("I LR -25 25");
    
    SensorValue frontInfrareds, ultraSound;
    SensorValue *list = NULL;
    
    sensorRead(SensorTypeUS, &ultraSound);
    
    while (ultraSound.values[0] > 30 || 1)
    {
        sensorRead(SensorTypeIFLR, &frontInfrareds);
        sensorRead(SensorTypeUS, &ultraSound);
        
        infraredsToDist(&frontInfrareds, SensorTypeIFLR);
        
        double ratio = (double)frontInfrareds.values[RIGHT] / (double)frontInfrareds.values[LEFT];
        
        driveRobotAndRecord(0.01, speed, ratio, &list);
    }
    
    printList(list);
    
    stopMotorsAndWait(1);
    playBackRecording(&list, speed);
}

int main()
{
    //setIPAndPort("128.16.79.5", 55443);
	connectAndGetSocket();
    
    sleep(2);
    
    
    goThroughPassage(120);
}