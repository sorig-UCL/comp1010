// Sensor type definitions
typedef struct {
    int values[3];
    int length;
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
    SensorTypeIFLR,     // Infrared rangefinders, Front Left and Right
    SensorTypeISLR,     // Infrared rangefinder, Side Left and Right
    SensorTypeBFLR,     // Bumper, Front Left and Right
    SensorTypeMELR,     // Motor Encoders, Left and Right
    SensorTypeMCLR,     // Motor Current, Left and Right
    SensorTypeIBLC,     // Infrared reflectometer, Bottom left and Centre
    SensorTypeIBCR,     // Infrared reflectometer, Bottom Centre and Right
    SensorTypeIBLR,     // Infrared reflectometer, Bottom Left and and Right
    SensorTypeIBLCR     // Infrared reflectometer, Bottom Left, Centre and Right
} SensorType;

int connectAndGetSocket();
void setIPAndPort(char *ip, int port);
int getVoltage();
int sensorRead(SensorType type, SensorValue *value);
int sendCommand(char *command);
int gp2d12_to_dist(int ir); // Front infrared rangefinders
int gp2d120_to_dist(int ir); // Side infrared rangefinders

void turnRobot(int degrees);
void driveRobot(double wheelTurns, int speed, double turnRatio);
void stopMotorsAndWait(int seconds);