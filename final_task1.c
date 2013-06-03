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
        driveRobot(-0.5, 40.0, 1.0);
        
        if (side == RIGHT) {
            turnRobot(-30);
        }
        else {
            turnRobot(30);
        }
    }
}

void followWall(int side,int turn)
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
    
    while (1)
    {
        bumperCheck(side);
        
        sensorRead(SensorTypeIFLR, &frontInfrareds);
        sensorRead(SensorTypeISLR, &sideInfrareds);
        
        infraredsToDist(&frontInfrareds, SensorTypeIFLR);
        infraredsToDist(&sideInfrareds, SensorTypeISLR);
        //int turn = -1;

        if(frontInfrareds.values[side] >= 50 /*sideInfrareds.values[side] >= 20*/){
            switch (turn++){
                case 0:{
                    driveRobot(1.5, 60, 1 + 0.5*(side == LEFT ? -1 : 1)); 
                    do{
                        bumperCheck(side);
                        driveRobot(0.1, 70, 1 + 0.5*(side == LEFT ? -1 : 1)); 
                        sensorRead(SensorTypeISLR, &sideInfrareds);
                        infraredsToDist(&sideInfrareds, SensorTypeISLR);
                    }
                    while(sideInfrareds.values[!side] >= 39);
                    followWall(!side,turn);
                    return;
                }
                break;
                case 1:{
                    driveRobot(2, 50, 1 + 0.8*(side == LEFT ? -1 : 1)); 
                    turnRobot(60*(side == LEFT ? -1 : 1));
                    driveRobot(1.0, 30, 1); 
                }
                break;
                default:{
                    driveRobot(2, 50, 1 + 0.5*(side == LEFT ? 1 : -1)); 
                    do{
                        bumperCheck(side);
                        driveRobot(0.1, 50, 1 + 0.5*(side == LEFT ? 1 : -1)); 
                        sensorRead(SensorTypeISLR, &sideInfrareds);
                        infraredsToDist(&sideInfrareds, SensorTypeISLR);
                    }
                    while(sideInfrareds.values[side] >= 39);
                    followWall(side,0);
                    return;
                }
                break;
            }
        }
        
        if (frontInfrareds.values[!side] < stopAndSearchDistance) {
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
            
            // Stabilise the distance to the wall around the specified value (e.g. 20 cm)
            ratio += ((minDistance-distanceToWall)/25.0) * (side == LEFT ? -1 : 1);
                        
            if (outOfRange) {
                ratio = 1.0;
            }
            
            driveRobot(0.01, 50, ratio);
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
    followWall(LEFT,0);
    //sendCommand("I LR -45 -45");
    //sendCommand("I R -45");
}