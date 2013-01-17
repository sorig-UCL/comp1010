#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <time.h>
#include "APIWrapper.h"

// String literals
static const char sensorNames[][24] = {"IFL", "IFR", "ISL", "ISR", "US", "BFL", "BFR", "V", "MEL", "MER", "MCL", "MCR", "IBL", "IBC", "IBR", "IFLR", "ISLR", "BFLR", "MELR", "MCLR", "IBLC", "IBCR", "IBLR", "IBLCR"};

// Private function declarations
int sendMsg(char* msg);
int recvMsg(char *buf);
void clearInputStream();
SensorValue parseValueString(char *str);
double avarageSensorDifference(SensorValue a, SensorValue b);
int minimumSensorDifference(SensorValue a, SensorValue b);


#define BUF_SIZE 80
int sock = -1;

int connectAndGetSocket()
{
    int volts;
    printf("connecting...");
    struct sockaddr_in s_addr;
    if (sock != -1) {
        close(sock);
        sock = -1;
    }
    
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        fprintf(stderr, "Failed to create socket\n");
        exit(1);
    }
    
    while (1) {
        s_addr.sin_family = AF_INET;
        s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        s_addr.sin_port = htons(55443);
        
        if (connect(sock, (struct sockaddr *) &s_addr, sizeof(s_addr)) >= 0) {
            /* connection succeeded */
            printf("done\n");
            volts = getVoltage(sock);
            printf("Battery state %2.1f volts\n", volts/10.0);
            return sock;
        }
        sleep(1);
        printf(".");
        fflush(stdout);
    }
}

int getVoltage() {
    int result;
    SensorValue value;
    result = sensorRead(SensorTypeV, &value);
    if (result < 0)
        return result;
    return value.values[0];
}

void turnRobot(int degrees)
{
    char buf[BUF_SIZE];
    
    SensorValue initialME, currentME;
    
    sendCommand("C RME");
    sensorRead(SensorTypeMELR, &initialME);
    
    currentME = initialME;
    
    // 6.79/3.0 - Works well without delay after driving forwards
    // 7.1/3.0  - Works well with delay after driving forwards
    while (avarageSensorDifference(initialME, currentME) <= degrees * (7.0/3.0))
    {
        int speed = 1 * (degrees/abs(degrees));
        sprintf(buf, "M LR %i %i\n", speed, -speed);
        write(sock, buf, strlen(buf));
        memset(buf, 0, BUF_SIZE);
        read(sock, buf, BUF_SIZE);
        
        sensorRead(SensorTypeMELR, &currentME);
    }
}


void driveRobot(double wheelTurns, int speed, double turnRatio)
{
    char buf[BUF_SIZE];
    SensorValue initialME, currentME;
    
    sendCommand("C RME");
    sensorRead(SensorTypeMELR, &initialME);
    
    currentME = initialME;
    
    int leftSpeed = (turnRatio < 1.0 ? speed/turnRatio : speed);
    int rightSpeed = (turnRatio > 1.0 ? speed/turnRatio : speed);
    
    while (minimumSensorDifference(initialME, currentME) <= wheelTurns*360.0)
    {
        sprintf(buf, "M LR %i %i\n", leftSpeed, rightSpeed);
        write(sock, buf, strlen(buf));
        memset(buf, 0, BUF_SIZE);
        read(sock, buf, BUF_SIZE);
        
        sprintf(buf, "C TRAIL\n");
        write(sock, buf, strlen(buf));
        memset(buf, 0, BUF_SIZE);
        read(sock, buf, BUF_SIZE);
        
        sensorRead(SensorTypeMELR, &currentME);
    }
}

void stopMotorsAndWait(int seconds)
{
    char buf[BUF_SIZE];
    
    time_t startTime = time(NULL);
    while (time(NULL) - startTime <= seconds)
    {
        sprintf(buf, "M LR 0 0\n");
        write(sock, buf, strlen(buf));
        memset(buf, 0, BUF_SIZE);
        read(sock, buf, BUF_SIZE);
    }
}

int sensorRead(SensorType type, SensorValue *value) {
    char  sendbuf[BUF_SIZE];
    char  recvbuf[BUF_SIZE];
    char *arg;
    
    sprintf(sendbuf, "S %s\n", sensorNames[type]);
    if (sendMsg(sendbuf)) {
        /* now we loop, reading until we get the answer to the request we just asked. */
        while (1)
        {
            memset(recvbuf, 0, BUF_SIZE);
            recvMsg(recvbuf);
            
            /* remove the trailing newline from the request string so we can
             compare it to the response */
            sendbuf[strlen(sendbuf)-1]='\0';
            if (strncmp(recvbuf, sendbuf, strlen(sendbuf))==0) {
                
                /* skip over the fixed part of the response to get to the
                 returned value */
                arg = recvbuf + strlen(sendbuf) + 1;
                *value = parseValueString(arg);
                /* all done */
                return 0;
            } else if (recvbuf[0] == 'W') {
                /* response is an asynchronous warning - we'll print it and ignore it. */
                printf("Warning: %s\n", recvbuf+2);
                /* now go round the loop again and re-read */
            } else {
                /* what we got back wasn't the response we expected or a warning. */
                printf("got an error >>%s<<\n", recvbuf);
                /* we don't really know what happened - just clear any
                 remaining input in the hope we can get back in sync. */
                clearInputStream();
                return -1;
            }
        }
    } else {
        /* the send failed - retry the whole request; it should auto-reconnect */
        return sensorRead(type, value);
    }
}

int sendCommand(char *command)
{
    char  sendbuf[BUF_SIZE];
    char  recvbuf[BUF_SIZE];
    
    sprintf(sendbuf, "%s\n", command);
    if (sendMsg(sendbuf)) {
        /* now we loop, reading until we get the answer to the request we just asked. */
        while (1)
        {
            memset(recvbuf, 0, BUF_SIZE);
            recvMsg(recvbuf);
            
            /* remove the trailing newline from the request string so we can
             compare it to the response */
            sendbuf[strlen(sendbuf)-1]='\0';
            if (strncmp(recvbuf, ".", 1)==0) {
                /* all done */
                return 0;
            } else if (recvbuf[0] == 'W') {
                /* response is an asynchronous warning - we'll print it and ignore it. */
                printf("Warning: %s\n", recvbuf+2);
                /* now go round the loop again and re-read */
            } else {
                /* what we got back wasn't the response we expected or a warning. */
                printf("got an error >>%s<<\n", recvbuf);
                /* we don't really know what happened - just clear any
                 remaining input in the hope we can get back in sync. */
                clearInputStream();
                return -1;
            }
        }
    } else {
        /* the send failed - retry the whole request; it should auto-reconnect */
        return sendCommand(command);
    }
}

// Private functions
// ----------------------------------------------------

int sendMsg(char* msg) {
    if (write(sock, msg, strlen(msg)) <= 0) {
        /* the write failed - likely the robot was switched off - attempt
         to reconnect and reinitialize */
        connectAndGetSocket();
        return 0;
    } else {
        return 1;
    }
}

int recvMsg(char *buf) {
    int val;
    fd_set read_fdset;
    fd_set except_fdset;
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    FD_ZERO(&read_fdset);
    FD_ZERO(&except_fdset);
    FD_SET(sock, &read_fdset);
    FD_SET(sock, &except_fdset);
    if (select(sock+1, &read_fdset, NULL, &except_fdset, &tv) == 0) {
        /* we've waited 2 seconds and got no response - too long - conclude
         the socket is dead */
        printf("timed out waiting response\n");
        connectAndGetSocket();
        return 0;
    }
    if (FD_ISSET(sock, &except_fdset)) {
        connectAndGetSocket();
        return 0;
    }
    
    assert(FD_ISSET(sock, &read_fdset));
    val = (int)read(sock, buf, sizeof(char)*BUF_SIZE);
    
    if (val > 0) {
    } else {
        /* the write failed - likely the robot was switched off - attempt
         to reconnect and reinitialize */
        connectAndGetSocket();
    }
    return val;
}

void clearInputStream() {
    // If we're out of sync, we read until there's no more input
    int val;
    fd_set read_fdset;
    struct timeval tv;
    char buf[1024];
    usleep(500000);
    while (1) {
        /* check if there's any data to read */
        /* we want to return immediately if there's nothing to read */
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        FD_ZERO(&read_fdset);
        FD_SET(sock, &read_fdset);
        if (select(sock+1, &read_fdset, NULL, NULL, &tv) == 0) {
            /* nothing to read - we're done */
            return;
        }
        assert(FD_ISSET(sock, &read_fdset));
        val = (int)read(sock, buf, 1024);
        if (val < 0) {
            /* we got an error; leave cleaning up to elsewhere */
            return;
        }
    }
}

SensorValue parseValueString(char *str)
{
    SensorValue result;
    result.length = 0;
    
    char *temp = str + strlen(str);
    while (temp > str)
    {
        while (temp >= str && *temp != ' ') {
            temp--;
        }
        
        *temp = '\0';
        temp++;
        
        result.values[result.length] = atoi(temp);
        result.length++;
    }
    
    return result;
}

double avarageSensorDifference(SensorValue a, SensorValue b)
{
    int i, length = a.length < b.length ? a.length : b.length;
    
    int sum = 0.0;
    for (i = 0; i < length; i++) {
        sum += abs(a.values[i] - b.values[i]);
    }
    
    return (double)sum / (double)length;
}

int minimumSensorDifference(SensorValue a, SensorValue b)
{
    int i, minDifference = INT32_MAX;
    int length = a.length < b.length ? a.length : b.length;
    
    for (i = 0; i < length; i++) {
        int difference = abs(a.values[i] - b.values[i]);
        minDifference = (difference < minDifference ? difference : minDifference);
    }
    
    return minDifference;
}