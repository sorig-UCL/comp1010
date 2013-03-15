#include <stdio.h>
#include <stdlib.h>
#include "APIWrapper.h"

void populateList(SensorValue **list)
{
    int i;
    for (i = 49; i >= 0; i--) {
        SensorValue *nextPoint = createSensorValue();
        nextPoint->length = 2;
        nextPoint->values[0] = i * 10;
        nextPoint->values[1] = i * 10;
        
        addSensorValue(list, nextPoint);
    }
}

void testListPlayback()
{
    SensorValue *list = NULL;
    populateList(&list);
    
    playBackRecording(&list, 20);
    
    listDelete(list);
}

int main()
{
    //setIPAndPort("128.16.79.5", 55443);
	connectAndGetSocket();
    
    sleep(2);
    
    testListPlayback();
}