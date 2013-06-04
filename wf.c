#include <stdio.h>
#include "APIWrapper.h"

#define MIN(A, B) (A < B ? A : B)

typedef enum {
    WorldStateWallLeft = 1 << 0,
    WorldStateWallRight = 1 << 1,
    WorldStateWallFront = 1 << 2
} WorldState;

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
    int stopAndSearchDistance = 30;
    
    if (side == RIGHT) {
        sendCommand("I LR 55 45");
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
        
        if (frontInfrareds.values[!side] < stopAndSearchDistance || frontInfrareds.values[side] < 15) {
            turnRobot(35 * (side == RIGHT ? -1 : 1));
            driveRobot(0.05, 30, 1.0);
        }
        else
        {
            int frontDistance = frontInfrareds.values[side];
            int sideDistance = sideInfrareds.values[side];
            
            int minDistance = MIN(frontDistance, sideDistance);
            int outOfRange = minDistance >= 60;
            
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
            
            driveRobot(0.01, 30, ratio);
        }
    }
}

WorldState getWorldState()
{
    SensorValue frontIR, sideIR, ultraSound;
    
    sensorRead(SensorTypeIFLR, &frontIR);
    sensorRead(SensorTypeISLR, &sideIR);
    sensorRead(SensorTypeUS, &ultraSound);
    
    
}

void reactiveRobotProgram()
{
    // prevState, state
    WorldState currentState, previousState;
    
    while (1) {
        // update the states
        // If wall on side -> no wall: turn in direction of wall untill it is found
        // If passage -> no wall: turn in either direction untill a wall is found
        // no wall: go straight
        // wall on side: follow wall
        // Wall
    }
}

int main()
{
    //setIPAndPort("128.16.79.9", 55443);
	connectAndGetSocket();
    sleep(2);
    followWall(RIGHT);
    //sendCommand("I LR -45 -45");
    //sendCommand("I R -45");
}