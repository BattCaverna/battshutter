#ifndef MOTOR_H
#define MOTOR_H

typedef enum MotorDir
{
  MS_STOP,
  MS_UP,
  MS_DOWN,
  MS_CNT
} MotorDir;

MotorDir motor_direction();
void motor_poll(MotorDir val);
MotorDir motor_position();

#endif
