#include <stdio.h>
#include "APIWrapper.h"
#include <math.h>

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
            turnRobot(-45, 20);
        }
        else {
            turnRobot(45, 20);
        }
    }
}

int currentFrontSideAngle;
void updateFrontSensorPositions(int side, int frontSideAngle, int stopAndSearchDistance)
{
    currentFrontSideAngle = frontSideAngle;
    
    int wallDetectAngle = round(atan((double)stopAndSearchDistance/12.5)*180.0/M_PI)-45.0;
    
    int frontLeftAngle = (side == RIGHT ? 45+wallDetectAngle : -45+frontSideAngle);
    int frontRightAngle = (side == RIGHT ? 45-frontSideAngle : -45-wallDetectAngle);
    
    char buf[80];
    sprintf(buf, "I LR %d %d", frontLeftAngle, frontRightAngle);
    sendCommand(buf);
}

/*
 track2b
 int speed = 40;
 int stabilisationDistanceToWall = 10;
 int frontSideAngle = -15;
 int stopAndSearchDistance = 30;
 */

int abs(int a)
{
    if (a < 0) {
        return a * -1;
    }
    return a;
}

void followWall(int side, int degrees)
{
    int speed = 20;
    int stabilisationDistanceToWall = 20;
    int frontSideAngle = 0;
    int stopAndSearchDistance = 35;
    int stopAndSearchHypotenuse = sqrt(12.5*12.5+stopAndSearchDistance*stopAndSearchDistance);
    
    updateFrontSensorPositions(side, frontSideAngle, stopAndSearchDistance);
    
    SensorValue frontInfrareds, sideInfrareds;
    
    SensorValue *list = NULL;
    
    sendCommand("C RME");
    addSensorValue(&list, createSensorValueAndRecord(SensorTypeMELR));
    
    double wallInFrontWheelTurnCount = 0.0;
    int wallInFrontFlag = 0;
    double length = 0.0;
    while (abs(list->values[0] - list->values[1]) < (1690.0/360.0)*degrees)
    {
        bumperCheck(side);
        
        sensorRead(SensorTypeIFLR, &frontInfrareds);
        sensorRead(SensorTypeISLR, &sideInfrareds);
        
        infraredsToDist(&frontInfrareds, SensorTypeIFLR);
        infraredsToDist(&sideInfrareds, SensorTypeISLR);
        
        int frontDistanceToWall = cos(currentFrontSideAngle*M_PI/ 180.0)*frontInfrareds.values[side];
        int wallInFront = frontInfrareds.values[!side] < stopAndSearchHypotenuse;
        int frontSideTooClose = frontDistanceToWall < stabilisationDistanceToWall/1.5;
        int headedTowardsTheWall = frontDistanceToWall < sideInfrareds.values[side];
        int wallToTheSide = frontDistanceToWall < 40 || sideInfrareds.values[side] < 40;
        
        // Too close! Do something!
        if (wallInFront || (frontSideTooClose &&  headedTowardsTheWall)) {
            /*if (wallInFront) {
                if (!wallInFrontFlag) {
                    updateFrontSensorPositions(side, -2*frontSideAngle, stopAndSearchDistance);
                }
                wallInFrontFlag = 1;
                wallInFrontWheelTurnCount = 0.0;
            }*/
            
            if (wallToTheSide) {
                turnAndRecord(35 * (side == RIGHT ? -1 : 1), speed/2, &list);
            }
            else {
                turnAndRecord(35 * (side == RIGHT ? 1 : -1), speed/2, &list);
            }
            
        }
        // Stabilise along wall with ratio
        else
        {
            // Should the sensor position be reset yet?
            if (wallInFrontFlag && wallInFrontWheelTurnCount > 0.05) {
                updateFrontSensorPositions(side, frontSideAngle, stopAndSearchDistance);
                wallInFrontFlag = 0;
            }
            
            int sideDistanceToWall = sideInfrareds.values[side];
            
            int minDistance = MIN(frontDistanceToWall, sideDistanceToWall);
            int outOfRange = minDistance >= 41;
            
            double ratio;
            if (side == RIGHT) {
                ratio = (double)frontDistanceToWall/(double)sideDistanceToWall;
            }
            else {
                ratio = (double)sideDistanceToWall/(double)frontDistanceToWall;
            }
            
            // Stabilise the distance to the wall around the specified value (e.g. 20 cm)
            ratio += ((minDistance-stabilisationDistanceToWall)/((double)stabilisationDistanceToWall-5.0)) * (side == LEFT ? -1 : 1);
            
            ratio = MAX(0.1, ratio);
            ratio = MIN(6.0, ratio);
            
            if (outOfRange) {
                ratio = 1.0;
            }
            
            wallInFrontWheelTurnCount += 0.01;
            length += 0.01;
            driveRobotAndRecord(0.01, speed, ratio, &list);
        }
    }
}

int main()
{
    //setIPAndPort("128.16.79.10", 55443);
	connectAndGetSocket();
    sleep(2);
    
    followWall(RIGHT, 270);
    while (1) {
        followWall(LEFT, 360);
        followWall(RIGHT, 360);
    }
    
    
}