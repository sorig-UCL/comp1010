#include <stdio.h>
#include "APIWrapper.h"
#include <math.h>

#define PI 3.14159265358979323846
#define edge1 3
#define edge2 4

void drawRATriangle(double a,double b)
{
    double longEdge = sqrt(a*a + b*b);
    double angle = asin(a/longEdge) * 180.0 / PI;
    
    printf("%f\n%f",angle,longEdge);
    
    driveRobot(a, 60, 1.0);
    stopMotorsAndWait(1);
    turnRobot(90);
    stopMotorsAndWait(1);
    driveRobot(b, 60, 1.0);
    stopMotorsAndWait(1);
    turnRobot(180 - angle);
    driveRobot((int)longEdge, 60, 1.0);
    

    
}

int main() {
	connectAndGetSocket();
    drawRATriangle(edge1,edge2);
}