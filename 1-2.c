#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int readME(int *sock, char *buf, int side)
{
    const char left[] = "S MEL\n";
    const char right[] = "S MER\n";
    sprintf(buf, (side ? right : left));
    write(*sock, buf, strlen(buf));
    memset(buf, 0, 80);
    read(*sock, buf, 80);
    
    char *bufEnd = buf + strlen(buf);
    while (*bufEnd != ' ') {
        bufEnd--;
    }
    bufEnd++;
    
    return atoi(bufEnd);
}

void turnRobot(int *sock, char *buf, int degrees)
{
    double calibration = 2.333;
    int finished = 0;
    int initialLeftME = readME(sock, buf, 1);
    while (!finished)
    {
        int speed = 30;
        int left = (degrees > 0 ? speed : -speed);
        int right = -left;
        sprintf(buf, "M LR %i %i\n", left, right);
        write(*sock, buf, strlen(buf));
        memset(buf, 0, 80);
        read(*sock, buf, 80);
                
        int melSensor = initialLeftME - readME(sock, buf, 1);
        if (melSensor >= degrees * calibration) {
            finished = 1;
        }
        
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
}