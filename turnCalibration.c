#include <stdio.h>
#include <stdlib.h>
#include "APIWrapper.h"


void trailDrive(double wheelTurns)
{
    double distance = 0.0;
    double loopDistance = 0.1;
    
    while (distance < wheelTurns)
    {
        driveRobot(loopDistance, 60, 1.0);
        sendCommand("C TRAIL");
        distance += loopDistance;
    }
    sendCommand("M LR 0 0\n");
}

int main()
{
    //setIPAndPort("128.16.79.9", 55443);
	connectAndGetSocket();
    
    sleep(2);
    
    turnRobot(90);
    sleep(1);
    trailDrive(5.0);
    sleep(1);
    turnRobot(180);
    sleep(1);
    trailDrive(5.0);
}