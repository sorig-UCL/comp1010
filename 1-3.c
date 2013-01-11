#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct {
    int leftME;
    int rightME;
} MEValue;

MEValue readME(int *sock, char *buf)
{
    // Send message
    sprintf(buf, "S MELR\n");
    write(*sock, buf, strlen(buf));
    memset(buf, 0, 80);
    read(*sock, buf, 80);
    
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
    return (leftDifference + rightDifference)/2;
}

void turnRobot(int *sock, char *buf, int degrees)
{
    MEValue initialME = readME(sock, buf);
    MEValue currentME = initialME;
    
    while (averageMEDifference(initialME, currentME) <= degrees*(2.25))
    {
        int speed = 1 * (degrees/abs(degrees));
        sprintf(buf, "M LR %i %i\n", speed, -speed);
        write(*sock, buf, strlen(buf));
        memset(buf, 0, 80);
        read(*sock, buf, 80);
        
        currentME = readME(sock, buf);
    }    
}

void driveRobot(int *sock, char *buf, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        sprintf(buf, "M LR 50 50\n");
        write(*sock, buf, strlen(buf));
        memset(buf, 0, 80);
        read(*sock, buf, 80);
        
        sprintf(buf, "C TRAIL\n");
        write(*sock, buf, strlen(buf));
        memset(buf, 0, 80);
        read(*sock, buf, 80);
    }
}

void drawSquare(int *sock, char *buf)
{
    driveRobot(sock, buf, 50);
    turnRobot(sock, buf, 90);
    driveRobot(sock, buf, 50);
    turnRobot(sock, buf, 90);
    driveRobot(sock, buf, 50);
    turnRobot(sock, buf, 90);
    driveRobot(sock, buf, 50);
    
}

void drawTriangle(int *sock, char *buf)
{
    driveRobot(sock, buf, 50);
    turnRobot(sock, buf, 90);
    driveRobot(sock, buf, 50);
    turnRobot(sock, buf, 90);
    driveRobot(sock, buf, 50);
    
}

int main() {
	char buf[80];
	struct sockaddr_in s_addr;
	int i, sock;
    
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
    
    
    
    //printf("MEL: %i\n", readME(&sock, buf, 0));
    //turnRobot(&sock, buf, 90);
    //driveRobot(&sock, buf, 10);
    drawSquare(&sock, buf);
    //readME(&sock, buf);
}