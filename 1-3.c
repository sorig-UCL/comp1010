#include <stdio.h>
#include "APIWrapper.h"

#define PI 3.14159265358979323846

void drawSquare(int sock, double size)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        driveRobot(sock, size, 60, 1.0);
        if (i < 3) {
            stopMotorsAndWait(sock, 1);
            turnRobot(sock, 90);
        }
    }
}

void drawTriangle(int sock, double size)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        driveRobot(sock, size, 60, 1.0);
        if (i < 2) {
            stopMotorsAndWait(sock, 1);
            turnRobot(sock, 120);
        }        
    }    
}

void drawStar(int sock, double size)
{
    int i;
    for (i = 0; i<5; i++)
    {
        driveRobot(sock, size, 60, 1.0);
        if (i < 4) {
            stopMotorsAndWait(sock, 1);
            turnRobot(sock, 144);
        }        
    }    
}

void drawCircle(int sock, double radius)
{    
    double circumference = 2 * radius * PI;
    
    double ratio = 1 + (1/radius);
    double wheelTurns = circumference*(3.0/4.0);
    
    driveRobot(sock, wheelTurns, 60, ratio);
}

int main() {
	int sock = connectAndGetSocket();
    
    //turnRobot(sock, 720);
    //driveRobot(sock, 10);
    //drawSquare(sock);
    drawCircle(sock, 1.0);
    //drawTriangle(sock);
    //drawStar(sock, 2.0);
    //readME(sock);
}