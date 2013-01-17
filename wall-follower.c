#include <stdio.h>
#include "APIWrapper.h"

int main() {
    // "128.16.80.185"
    setIPAndPort("128.16.80.185", 55443);
	connectAndGetSocket();
    
    SensorValue infraredRangeFinders;
    sensorRead(SensorTypeIFLR, &infraredRangeFinders);
    
    printf("rangefinders: %i %i\n", infraredRangeFinders.values[0], infraredRangeFinders.values[1]);
}