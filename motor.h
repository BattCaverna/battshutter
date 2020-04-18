#ifndef MOTOR_H
#define MOTOR_H

typedef enum MotorDir
{
  MS_STOP,
  MS_UP,
  MS_DOWN,
  MS_CNT
} MotorDir;


typedef enum MotorPos
{
  MP_MIDDLE,
  MP_ALL_UP,
  MP_ALL_DOWN,
  MP_CNT
} MotorPos;

MotorDir motor_currentDirection();
void motor_poll(MotorDir val);
MotorPos motor_timeoutStatus();

#endif
