#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include "APIWrapper.h"


// String literals
static const char sensorNames[][24] = {"IFL", "IFR", "ISL", "ISR", "US", "BFL", "BFR", "V", "MEL", "MER", "MCL", "MCR", "IBL", "IBC", "IBR", "IFLR", "ISLR", "BFLR", "MELR", "MCLR", "IBLC", "IBCR", "IBLR", "IBLCR"};

// Private function declarations
int sendMsg(char* msg);
int recvMsg(char *buf);
void clearInputStream();
SensorValue parseValueString(char *str);
SensorValue reverseValueOrder(SensorValue someValue);
double avarageSensorDifference(SensorValue a, SensorValue b);
int minimumSensorDifference(SensorValue a, SensorValue b);
int maximumSensorDifference(SensorValue a, SensorValue b);
int gp2d12_to_dist(int ir); // Front infrared rangefinders
int gp2d120_to_dist(int ir); // Side infrared rangefinders
int getUSDist(); // Ultrasound

#define BUF_SIZE 80
int sock = -1;

char *global_ip = "127.0.0.1";
int global_port = 55443;

int connectAndGetSocket()
{
    printf("connecting...");
    struct sockaddr_in s_addr;
    if (sock != -1) {void setIPAndPort(char *ip, int port);
        close(sock);
        sock = -1;
    }
    
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        fprintf(stderr, "Failed to create socket\n");
        exit(1);
    }
    
    while (1) {
        s_addr.sin_family = AF_INET;
        s_addr.sin_addr.s_addr = inet_addr(global_ip);
        s_addr.sin_port = htons(global_port);
        
        if (connect(sock, (struct sockaddr *) &s_addr, sizeof(s_addr)) >= 0) {
            /* connection succeeded */
            printf("done\n");
            return sock;
        }
        sleep(1);
        printf(".");
        fflush(stdout);
    }
}

void setIPAndPort(char *ip, int port)
{
    global_ip = ip;
    global_port = port;
}

int getVoltage() {
    int result;
    SensorValue value;
    result = sensorRead(SensorTypeV, &value);
    if (result < 0)
        return result;
    return value.values[0];
}

void turnRobot(int degrees, int speed)
{
    char buf[BUF_SIZE];
    
    SensorValue initialME, currentME;
    
    sendCommand("C RME");
    sensorRead(SensorTypeMELR, &initialME);
    
    currentME = initialME;
    
    // 6.79/3.0 - Works well without delay after driving forwards
    // 7.1/3.0  - Works well with delay after driving forwards
    while (avarageSensorDifference(initialME, currentME) < fabs(degrees * (2.36)))
    {        
        speed *= (degrees/abs(degrees));
        sprintf(buf, "M LR %i %i", speed, -speed);
        sendCommand(buf);        
        
        sensorRead(SensorTypeMELR, &currentME);
    }
    
    // Stop the robot - important!
    sendCommand("M LR 0 0");
}


void driveRobot(double wheelTurns, int speed, double turnRatio)
{
    char buf[BUF_SIZE];
    SensorValue initialME, currentME;
    
    sendCommand("C RME");
    sensorRead(SensorTypeMELR, &initialME);
    
    currentME = initialME;
    
    int leftSpeed = (int)(turnRatio < 1.0 ? (double)speed*turnRatio : speed) * (wheelTurns < 0.0 ? -1 : 1);
    int rightSpeed = (int)(turnRatio > 1.0 ? (double)speed/turnRatio : speed) * (wheelTurns < 0.0 ? -1 : 1);
    
    while (maximumSensorDifference(initialME, currentME) <= fabs(wheelTurns)*360.0)
    {
        sprintf(buf, "M LR %i %i", leftSpeed, rightSpeed);
        sendCommand(buf);
        
        sensorRead(SensorTypeMELR, &currentME);
    }
}

void driveRobotAndRecord(double wheelTurns, int speed, double turnRatio, SensorValue **list)
{
    char buf[BUF_SIZE];
    SensorValue initialME, currentME;
    
    addSensorValue(list, createSensorValueAndRecord(SensorTypeMELR));
    initialME = **list;
    
    currentME = initialME;
    
    int leftSpeed = (int)(turnRatio < 1.0 ? (double)speed*turnRatio : speed) * (wheelTurns < 0.0 ? -1 : 1);
    int rightSpeed = (int)(turnRatio > 1.0 ? (double)speed/turnRatio : speed) * (wheelTurns < 0.0 ? -1 : 1);
    
    while (maximumSensorDifference(initialME, currentME) <= fabs(wheelTurns)*360.0)
    {
        sprintf(buf, "M LR %i %i", leftSpeed, rightSpeed);
        sendCommand(buf);
        
        addSensorValue(list, createSensorValueAndRecord(SensorTypeMELR));
        currentME = **list;
    }
}

void turnAndRecord(int degrees, int speed, SensorValue **list)
{
    char buf[BUF_SIZE];
    
    SensorValue initialME, currentME;
    
    addSensorValue(list, createSensorValueAndRecord(SensorTypeMELR));
    initialME = **list;
    
    currentME = initialME;
    
    // 6.79/3.0 - Works well without delay after driving forwards
    // 7.1/3.0  - Works well with delay after driving forwards
    while (avarageSensorDifference(initialME, currentME) < fabs(degrees * (2.36)))
    {
        int aspeed = speed*((double)degrees/(double)abs(degrees));
        sprintf(buf, "M LR %i %i", aspeed, -aspeed);
        sendCommand(buf);
        
        addSensorValue(list, createSensorValueAndRecord(SensorTypeMELR));
        currentME = **list;
    }
    
    // Stop the robot - important!
    sendCommand("M LR 0 0");
}

/*void playBackRecording(SensorValue **list, int speed)
{
    SensorValue currentME, initialME;
    sensorRead(SensorTypeMELR, &initialME);
    
    turnRobot(180);
    stopMotorsAndWait(1);
    sendCommand("C RME");
    
    sensorRead(SensorTypeMELR, &currentME);
    
    while (*list)
    {
        int rightUnfinished, leftUnfinished;
        int leftFinishingME = (*list)->values[RIGHT];
        int rightFinishingME = (*list)->values[LEFT];
        
        do {
            int leftAdjustedME = initialME.values[RIGHT] - currentME.values[LEFT];
            int rightAdjustedME = initialME.values[LEFT] - currentME.values[RIGHT];
            
            double ratio = ((double)leftAdjustedME - (double)leftFinishingME) / ((double)rightAdjustedME - (double)rightFinishingME);
            
            int leftSpeed = (ratio > 1.0 ? speed : ratio * speed);
            int rightSpeed = (ratio > 1.0 ? speed / ratio : speed);
            
            leftUnfinished = leftAdjustedME > leftFinishingME;
            rightUnfinished = rightAdjustedME > rightFinishingME;
            
            if (!leftUnfinished || !rightUnfinished) {
                leftSpeed = speed;
                rightSpeed = speed;
            }
            
            if (leftUnfinished || rightUnfinished)
            {
                leftSpeed = leftSpeed * (leftUnfinished ? 1 : 0);
                rightSpeed = rightSpeed * (rightUnfinished ? 1 : 0);
                
                char command[BUF_SIZE];
                sprintf(command, "M LR %i %i", leftSpeed, rightSpeed);
                sendCommand(command);
            }            
            
            sensorRead(SensorTypeMELR, &currentME);
            
        } while (rightUnfinished || leftUnfinished);
        
        *list = (*list)->next;
    }
}*/

void playBackRecording(SensorValue **list, int speed)
{
    sendCommand("C RME");
    
    SensorValue currentME;    
    sensorRead(SensorTypeMELR, &currentME);
    
    while (*list)
    {
        int rightUnfinished, leftUnfinished;
        int leftFinishingME = (*list)->values[LEFT];
        int rightFinishingME = (*list)->values[RIGHT];
        
        do
        {
            double ratio = ((double)leftFinishingME - (double)currentME.values[LEFT]) / ((double)rightFinishingME - (double)currentME.values[RIGHT]);
            
            // Division by zero
            if (((double)rightFinishingME - (double)currentME.values[RIGHT]) == 0) {
                ratio = 1.0;
            }
            
            int leftSpeed = (ratio > 1.0 ? speed : ratio * speed);
            int rightSpeed = (ratio > 1.0 ? speed / ratio : speed);
            
            leftUnfinished = currentME.values[LEFT] < leftFinishingME;
            rightUnfinished = currentME.values[RIGHT] < rightFinishingME;
            
            if (!leftUnfinished || !rightUnfinished) {
                leftSpeed = speed;
                rightSpeed = speed;
            }
            
            if (leftUnfinished || rightUnfinished)
            {
                leftSpeed = leftSpeed * (leftUnfinished ? 1 : 0);
                rightSpeed = rightSpeed * (rightUnfinished ? 1 : 0);
                
                char command[BUF_SIZE];
                sprintf(command, "M LR %i %i", leftSpeed, rightSpeed);
                sendCommand(command);
            }
            
            sensorRead(SensorTypeMELR, &currentME);
            
        } while (rightUnfinished || leftUnfinished);
        
        *list = (*list)->next;
    }
}

void stopMotorsAndWait(int seconds)
{    
    time_t startTime = time(NULL);
    while (time(NULL) - startTime <= seconds)
    {
        sendCommand("M LR 0 0");
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
            if (sendbuf[strlen(sendbuf)-1] == '\n') {
                sendbuf[strlen(sendbuf)-1]='\0';
            }
            
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
            if (sendbuf[strlen(sendbuf)-1] == '\n') {
                sendbuf[strlen(sendbuf)-1]='\0';
            }
            
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

void infraredsToDist(SensorValue *sensorValue, SensorType type)
{
    int (*converterFunction)(int);
    if (type == SensorTypeIFLR)
    {
        converterFunction = &gp2d12_to_dist;
    }
    else if (type == SensorTypeISLR)
    {
        converterFunction = &gp2d120_to_dist;
    }
    else
    {
        return;
    }
    
    int i;
    for (i = 0; i < sensorValue->length; i++) {
        sensorValue->values[i] = (*converterFunction)(sensorValue->values[i]);
    }
}

// SensorValue data structure
SensorValue* createSensorValue()
{
    SensorValue *sensorValue = (SensorValue *)malloc(sizeof(SensorValue));
    
    return sensorValue;
}

SensorValue* createSensorValueAndRecord(SensorType type)
{
    SensorValue *sensorValue = createSensorValue();
    
    sensorRead(type, sensorValue);
    
    return sensorValue;
}

void addSensorValue(SensorValue **list, SensorValue *newValue)
{
    newValue->next = *list;
    *list = newValue;
}

void deleteSensorValue(SensorValue *sensorValue)
{
    free(sensorValue->values);
    free(sensorValue);
}

void listDelete(SensorValue *list)
{
    SensorValue *sensorValue = list;
    SensorValue *next;
    while(sensorValue)
    {
        next = sensorValue->next;
        deleteSensorValue(sensorValue);
        sensorValue = next;
    }
}
//  a -> b -> c -> d
void reverseList(SensorValue **list)
{
    SensorValue *prev = NULL;
    
    while (*list)
    {
        SensorValue *next = (*list)->next;
        
        (*list)->next = prev;
        
        prev = *list;
        if (next) {
            *list = next;
        }
        else {
            break;
        }        
    }
}

void printList(SensorValue *list)
{
    SensorValue *current = list;
    while (current)
    {
        printf("MER: %i, MEL: %i\n", current->values[0], current->values[1]);
        current = current->next;
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
    
    return reverseValueOrder(result);
}

SensorValue reverseValueOrder(SensorValue someValue)
{
    SensorValue reversedValue;
    reversedValue.length = someValue.length;
    
    int i;
    for (i = someValue.length-1; i >= 0; i--) {
        reversedValue.values[someValue.length-i-1] = someValue.values[i];
    }
    
    return reversedValue;
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

int maximumSensorDifference(SensorValue a, SensorValue b)
{
    int i, maxDifference = 0;
    int length = a.length < b.length ? a.length : b.length;
    
    for (i = 0; i < length; i++) {
        int difference = abs(a.values[i] - b.values[i]);
        maxDifference = (difference > maxDifference ? difference : maxDifference);
    }
    
    return maxDifference;
}

int gp2d12_to_dist(int ir) {
    int dist;
    if (ir > 35)
        dist = (6787 / (ir - 3)) - 4;
    else
        dist=200;
    return dist;
}

int gp2d120_to_dist(int ir) {
    int dist;
    if (ir > 80)
        dist = (2914 / (ir + 5)) - 1;
    else
        dist = 40;
    return dist;
}