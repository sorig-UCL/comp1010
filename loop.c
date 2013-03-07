#include <stdio.h>
#include <stdlib.h>
#include "APIWrapper.h"

/*
 Takes a SensorValue* argument and updates with new data from the robot
 Return values:
 1: line is not detected
 0: line is detected
 */
int detectLineAndUpdateValue(SensorValue *value)
{
    SensorValue newUnderside;
    sensorRead(SensorTypeIBLCR, &newUnderside);
    
    int i, threshold = 75, result = 1;
    for (i = 0; i < value->length; i++)
    {
        if (abs(value->values[i] - newUnderside.values[i]) > threshold)
        {
            result = 0;
            break;
        }
    }
    
    *value = newUnderside;
    
    return result;
}

void loop()
{
    sendCommand("C RME");
    sendCommand("I LR -25 25");
    
    SensorValue frontInfrareds, underside = {0};
    SensorValue *pathList = NULL;
    
    while (detectLineAndUpdateValue(&underside))
    {
        sensorRead(SensorTypeIFLR, &frontInfrareds);
        
        infraredsToDist(&frontInfrareds, SensorTypeIFLR);
        
        double ratio = (double)frontInfrareds.values[RIGHT] / (double)frontInfrareds.values[LEFT];
        
        driveRobotAndRecord(0.001, 40, ratio, &pathList);
    }
    
    printList(pathList);
    
    stopMotorsAndWait(1);
    playBackRecording(&pathList, 20);
}

int main()
{
    //setIPAndPort("128.16.79.5", 55443);
	connectAndGetSocket();
    
    sleep(2);
    
    loop();
}