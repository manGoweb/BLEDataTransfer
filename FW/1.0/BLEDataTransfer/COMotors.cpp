#include <QueueArray.h>



#import "COMotors.h"


COMotors::COMotors() {
  
}

// Positioning

void COMotors::goToPosition(float percentPosition, float speed, bool overrideQueue) {
  COMotorsCommand command = COMotorsCommand();
  command.speed = speed;
  
  latestPanCommandIndex++;
  if (latestPanCommandIndex == MOTOR_COMMAND_QUEUE_CAPACITY) {
    
  }
  // TODO: If speed has been only increased, do we want to override too?
  panComandQueue[latestPanCommandIndex] = command;

  // TODO: Finish the current step with deceleration if direction changed. Start new command when the deceleration finishes
  currentPanCommandIndex = latestPanCommandIndex;
}

void COMotors::overrideAndGoToPosition(float percentPosition, float speed) {
  goToPosition(percentPosition, speed, true);
}

void COMotors::rotateToPosition(float percentPosition, float speed, bool overrideQueue) {
  // TODO: Replicate pan functionality
}

void COMotors::overrideAndRotateToPosition(float anglePosition, float speed) {
  rotateToPosition(anglePosition, speed, true);
}

// Configuration

void COMotors::configure(bool fullConfiguration) {
  
}

