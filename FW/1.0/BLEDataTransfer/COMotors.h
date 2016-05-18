

#ifndef COMotors_h
#define COMotors_h


#include <AccelStepper.h>
#include <MultiStepper.h>


#define MOTOR_COMMAND_QUEUE_CAPACITY        200


struct COMotorsCommand {
  int steps;
  float speed;
};


enum COMotorsMotor {
  COMotorsMotorRotate = 0,
  COMotorsMotorPan = 1
};

enum COMotorsDirection {
  COMotorsDirectionLeft = 0,
  COMotorsDirectionRight = 1
};


class COMotors {
  
  public:
    COMotors();
    
    float currentPanPosition = 0;
    float currentPanStepPosition = 0;
    
    float currentRotationPosition = 0;
    float currentRotationStepPosition = 0;
    
    void goToPosition(float percentPosition, float speed, bool overrideQueue = false);       // Pan motor, go to percent position (0% to 100%)
    void overrideAndGoToPosition(float anglePosition, float speed);                          // Ditto but overrides all previous values, if direction changes there will be a deceleration in original direction
    
    void rotateToPosition(float percentPosition, float speed, bool overrideQueue = false);   // Rotate motor (-n˚ to +n˚)
    void overrideAndRotateToPosition(float anglePosition, float speed);                      // Ditto but overrides all previous values, if direction changes there will be a deceleration in original direction
    
    void configure(bool fullConfiguration);                                                  // Configure positions, full configuration sets motor steps as well
    
  private:
    
    int currentPanCommandIndex = 0;
    int latestPanCommandIndex = 0;
    COMotorsCommand panComandQueue[MOTOR_COMMAND_QUEUE_CAPACITY];
    
    int currentRotationCommandIndex = 0;
    int latestRotationCommandIndex = 0;
    COMotorsCommand rotationComandQueue[MOTOR_COMMAND_QUEUE_CAPACITY];
    
};


#endif
