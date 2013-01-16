#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define PI 3.14159265358979323846

typedef struct {
    int leftME;
    int rightME;
} MEValue;

MEValue readME(int sock)
{
    char buf[80];
    
    // Send message
    sprintf(buf, "S MELR\n");
    write(sock, buf, strlen(buf));
    memset(buf, 0, 80);
    read(sock, buf, 80);
    
    // Parse the response
    char *secondInt = buf + strlen(buf);
    while (*secondInt != ' ') {
        secondInt--;
    }
    char *firstInt = secondInt-1;
    *secondInt = '\0';
    secondInt++;
    
    while (*firstInt != ' ') {
        firstInt--;
    }
    firstInt++;
    
    // Construct the return value
    MEValue returnValue;
    returnValue.leftME = atoi(firstInt);
    returnValue.rightME = atoi(secondInt);
    return returnValue;
}

double averageMEDifference(MEValue a, MEValue b)
{
    int leftDifference = abs(a.leftME - b.leftME);
    int rightDifference = abs(a.rightME - b.rightME);
    return (leftDifference + rightDifference)/2.0;
}

double minimumMEDifference(MEValue a, MEValue b)
{
    int leftDifference = (abs(a.leftME - b.leftME));
    int rightDifference = (abs(a.rightME - b.rightME));
    
    return (leftDifference < rightDifference ? leftDifference : rightDifference);
}

void turnRobot(int sock, int degrees)
{
    char buf[80];
    
    MEValue initialME = readME(sock);
    MEValue currentME = initialME;
    
    // 6.79/3.0 - Works well without delay after driving forwards
    // 7.1/3.0  - Works well with delay after driving forwards
    while (averageMEDifference(initialME, currentME) <= degrees * (7.0/3.0))
    {
        int speed = 1 * (degrees/abs(degrees));
        sprintf(buf, "M LR %i %i\n", speed, -speed);
        write(sock, buf, strlen(buf));
        memset(buf, 0, 80);
        read(sock, buf, 80);
        
        currentME = readME(sock);
    }    
}


void driveRobot(int sock, double wheelTurns, int speed, double turnRatio)
{
    char buf[80];
    MEValue initialME = readME(sock);
    int i;
    
    int leftSpeed = (turnRatio < 1.0 ? speed/turnRatio : speed);
    int rightSpeed = (turnRatio > 1.0 ? speed/turnRatio : speed);
    
    while (minimumMEDifference(initialME, readME(sock)) <= wheelTurns*360)
    {
        sprintf(buf, "M LR %i %i\n", leftSpeed, rightSpeed);
        write(sock, buf, strlen(buf));
        memset(buf, 0, 80);
        read(sock, buf, 80);
        
        sprintf(buf, "C TRAIL\n");
        write(sock, buf, strlen(buf));
        memset(buf, 0, 80);
        read(sock, buf, 80);
    }
}

void stopMotorsAndWait(int sock, int seconds)
{
    char buf[80];
    
    time_t startTime = time(NULL);
    while (time(NULL) - startTime <= seconds)
    {
        sprintf(buf, "M LR 0 0\n");
        write(sock, buf, strlen(buf));
        memset(buf, 0, 80);
        read(sock, buf, 80);
    }    
}

void drawSquare(int sock, double size)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        driveRobot(sock, size, 50, 1.0);
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
        driveRobot(sock, size, 50, 1.0);
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
        driveRobot(sock, size, 10, 1.0);
        if (i < 4) {
            stopMotorsAndWait(sock, 1);
            turnRobot(sock, 144);
        }        
    }    
}

void drawCircle(int sock, double radius)
{    
    double innerCircumference = 2 * radius * PI;
    
    double ratio = 1 + (1/radius);
    double wheelTurns = innerCircumference*(3.0/4.0);
    
    driveRobot(sock, wheelTurns, 60, ratio);
}

int connectAndGetSocket()
{
    struct sockaddr_in s_addr;
	int sock;
    
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr, "Failed to create socket\n");
		exit(1);
	}
    
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	s_addr.sin_port = htons(55443);
    
	if (connect(sock, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) {
		fprintf(stderr, "Failed to connect socket\n");
		exit(1);
	}
    
    return sock;
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