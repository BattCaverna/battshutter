#ifndef MOTOR_H
#define MOTOR_H

typedef enum MotorStatus
{
  MS_STOP,
  MS_UP,
  MS_DOWN,
  MS_CNT
} MotorStatus;

MotorStatus motor_status();

#endif
