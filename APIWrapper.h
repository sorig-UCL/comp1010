typedef struct {
    int leftME;
    int rightME;
} MEValue;


int connectAndGetSocket();
void turnRobot(int sock, int degrees);
void driveRobot(int sock, double wheelTurns, int speed, double turnRatio);
void stopMotorsAndWait(int sock, int seconds);
MEValue readME(int sock);