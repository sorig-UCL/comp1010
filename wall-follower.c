#include <stdio.h>
#include "APIWrapper.h"

void followWall()
{
    int minimumDistance = 300;
    int maximumDistance = 500;
    
    sendCommand("I R 0");
    SensorValue infraredRangeFinders;
    
    while (1) {
        sensorRead(SensorTypeIFR, &infraredRangeFinders);
        printf("rangefinders: %i\n", infraredRangeFinders.values[0]);
        
        if (infraredRangeFinders.values[0] > minimumDistance) {
            // Turn left
            turnRobot(-5);
        }
        else if (infraredRangeFinders.values[0] < maximumDistance) {
            // Turn right
            turnRobot(5);
        }
        
        driveRobot(0.25, 20, 1.0);
        
    }    
}

int main()
{
    //setIPAndPort("128.16.80.185", 55443);
	connectAndGetSocket();
    
    /*turnRobot(90);
    driveRobot(9.0, 127, 1.0);
    turnRobot(225);
    driveRobot(1.0, 127, 1.0);
    */
    //followWall();
    
    turnRobot(-90);
}