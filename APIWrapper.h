// Sensor type definitions
typedef struct SensorValue {
    int values[3];
    int length;
    
    struct SensorValue *next;
} SensorValue;

typedef enum {
    SensorTypeIFL = 0,  // Infrared rangefinder, Front Left
    SensorTypeIFR,      // Infrared rangefinder, Front Right
    SensorTypeISL,      // Infrared rangefinder, Side Left
    SensorTypeISR,      // Infrared rangefinder, Side Right
    SensorTypeUS,       // Ultrasound, front
    SensorTypeBFL,      // Bumper, Front Left
    SensorTypeBFR,      // Bumper, Front Right
    SensorTypeV,        // Battery Voltage
    SensorTypeMEL,      // Motor Encoder, Left
    SensorTypeMER,      // Motor Encoder, Right
    SensorTypeMCL,      // Motor Current, Left
    SensorTypeMCR,      // Motor Current, Right
    SensorTypeIBL,      // Infrared reflectometer, Bottom Left
    SensorTypeIBC,      // Infrared reflectometer, Bottom Centre
    SensorTypeIBR,      // Infrared reflectometer, Bottom Right
    SensorTypeIFLR,     // Infrared rangefinders, Front Left and Front Right
    SensorTypeISLR,     // Infrared rangefinder, Side Left and Side Right
    SensorTypeBFLR,     // Bumper, Front Left and Right
    SensorTypeMELR,     // Motor Encoders, Left and Right
    SensorTypeMCLR,     // Motor Current, Left and Right
    SensorTypeIBLC,     // Infrared reflectometer, Bottom left and Centre
    SensorTypeIBCR,     // Infrared reflectometer, Bottom Centre and Right
    SensorTypeIBLR,     // Infrared reflectometer, Bottom Left and and Right
    SensorTypeIBLCR     // Infrared reflectometer, Bottom Left, Centre and Right
} SensorType;

#define LEFT 0
#define RIGHT 1

int connectAndGetSocket();
void setIPAndPort(char *ip, int port);
int getVoltage();
int sensorRead(SensorType type, SensorValue *value);
int sendCommand(char *command);
void infraredsToDist(SensorValue *sensorValue, SensorType type); // Currently only supports SensorTypeIFLR and SensorTypeISLR

void turnRobot(int degrees, int speed);
void turnAndRecord(int degrees, int speed, SensorValue **list);
void driveRobot(double wheelTurns, int speed, double turnRatio);
void driveRobotAndRecord(double wheelTurns, int speed, double turnRatio, SensorValue **list);
void playBackRecording(SensorValue **list, int speed);
void stopMotorsAndWait(int seconds);

// SensorValue linked list data structure
SensorValue* createSensorValue();
SensorValue* createSensorValueAndRecord(SensorType type);
void addSensorValue(SensorValue **list, SensorValue *newValue);
void deleteSensorValue(SensorValue *sensorValue);
void listDelete(SensorValue *list);
void reverseList(SensorValue **list);
void printList(SensorValue *list);