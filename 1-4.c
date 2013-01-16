#include <stdio.h>
#include "APIWrapper.h"

#define PI 3.14159265358979323846

void drawSquare(int sock, double size)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        driveRobot(sock, size, 60, 1.0);
        stopMotorsAndWait(sock, 1);
        if (i < 3) {            
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
        stopMotorsAndWait(sock, 1);
        if (i < 2) {            
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
        stopMotorsAndWait(sock, 1);
        if (i < 4) {            
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
    stopMotorsAndWait(sock, 0);
}

void presentMenu(int sock)
{
    printf("What would you like to draw? (type in an integer)\n");
    printf("1. Square\n");
    printf("2. Triangle\n");
    printf("3. Star\n");
    printf("4. Circle\n");
    
    int input;
    scanf("%i", &input);
    
    if (input >= 1 && input <= 3) {
        printf("Please enter the size of the shape (in wheel turns): \n");
    }
    else if(input == 4)
    {
        printf("Please enter the radius (in wheel turns): \n");
    }
    
    double size;
    scanf("%lf", &size);
    
    printf("Drawing..\n");
    
    switch (input) {
        case 1:
            drawSquare(sock, size);
            break;
        case 2:
            drawTriangle(sock, size);
            break;
        case 3:
            drawStar(sock, size);
            break;
        case 4:
            drawCircle(sock, size);
            break;
        default:
            break;
    }
    
    printf("Done!\n\n");
}

int main() {
	int sock = connectAndGetSocket();
    
    while (1) {
        presentMenu(sock);
    }
}