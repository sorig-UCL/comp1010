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

int robotAligned()
{
    return 0;
}

void alignAtLine()
{
    sendCommand("C RME");
    SensorValue frontInfrareds;
    
    int direction = 1;
    
    double distance = 0.0;
    int alignedCount = 0;
    
    while (alignedCount < 100 || distance > -0.08)
    {
        sensorRead(SensorTypeIFLR, &frontInfrareds);
        
        infraredsToDist(&frontInfrareds, SensorTypeIFLR);
        
        alignedCount = (frontInfrareds.values[RIGHT] == frontInfrareds.values[LEFT] ? alignedCount + 1 : 0);
        printf("alignmentCount: %i\n", alignedCount);
        
        double ratio = (double)frontInfrareds.values[RIGHT] / (double)frontInfrareds.values[LEFT];
        
        ratio = (direction < 0 ? 1/ratio : ratio);
        
        driveRobot(0.01, 40*direction, ratio);
        
        distance += 0.01*direction;
                
        if (distance > 0.1 || distance < -0.1) {
            direction *= -1;
        }
    }
    
    // Drive to the line
    SensorValue underside = {0};    
    while (detectLineAndUpdateValue(&underside))
    {        
        driveRobot(0.01, 20, 1.0);
    }
    
    // A bit further to position the robot in the starting position
    driveRobot(0.3, 20, 1.0);
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
    
    alignAtLine();
    
    reverseList(&pathList);
    printList(pathList);
    
    stopMotorsAndWait(1);
    playBackRecording(&pathList, 20);
    
    listDelete(pathList);
}

int main()
{
    //setIPAndPort("128.16.79.5", 55443);
	connectAndGetSocket();
    
    sleep(2);
    
    loop();
}