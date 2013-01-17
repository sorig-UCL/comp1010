// Sensor type definitions
typedef struct {
    int values[3];
    int length;
} SensorValue;

typedef enum {
    SensorTypeIFL = 0,
    SensorTypeIFR,
    SensorTypeISL,
    SensorTypeISR,
    SensorTypeUS,
    SensorTypeBFL,
    SensorTypeBFR,
    SensorTypeV,
    SensorTypeMEL,
    SensorTypeMER,
    SensorTypeMCL,
    SensorTypeMCR,
    SensorTypeIBL,
    SensorTypeIBC,
    SensorTypeIBR,
    SensorTypeIFLR,
    SensorTypeISLR,
    SensorTypeBFLR,
    SensorTypeMELR,
    SensorTypeMCLR,
    SensorTypeIBLC,
    SensorTypeIBCR,
    SensorTypeIBLR,
    SensorTypeIBLCR
} SensorType;

int connectAndGetSocket();
int getVoltage();
int sensorRead(SensorType type, SensorValue *value);
int sendCommand(char *command);

void turnRobot(int degrees);
void driveRobot(double wheelTurns, int speed, double turnRatio);
void stopMotorsAndWait(int seconds);