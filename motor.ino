#include "motor.h"

#define MOTOR_PWR 12
#define MOTOR_DIR 11

#define GUARD_TIME 50 //ms

static MotorStatus curr_status;
static MotorStatus new_status;
static long prev_move;

#define MOTOR_ON   HIGH
#define MOTOR_OFF  LOW
#define MOTOR_UP   LOW
#define MOTOR_DOWN HIGH


void motor_init()
{
  pinMode(MOTOR_PWR, OUTPUT);
  digitalWrite(MOTOR_PWR, MOTOR_OFF);
  pinMode(MOTOR_DIR, OUTPUT);
  digitalWrite(MOTOR_DIR, MOTOR_UP);
  prev_move = millis();
  curr_status = MS_STOP;
  new_status = MS_STOP;
}

void motor_enable(MotorStatus val);

void motor_enable(MotorStatus val)
{
  new_status = val;
  motor_poll();
}

static void motor_setDir(MotorStatus dir);

static void motor_setDir(MotorStatus dir)
{
    if (dir == MS_DOWN)
      digitalWrite(MOTOR_DIR, MOTOR_DOWN);
    else
      digitalWrite(MOTOR_DIR, MOTOR_UP);  
}

void motor_poll()
{  
  long now = millis();
  
  if (new_status == curr_status)
  {
    prev_move = now;
    return;
  }

  if (new_status == MS_STOP)
    digitalWrite(MOTOR_PWR, MOTOR_OFF);
  else
    motor_setDir(new_status);

  if (now - prev_move >= GUARD_TIME)
  {
    if (new_status == MS_STOP)
      motor_setDir(new_status);
    else
      digitalWrite(MOTOR_PWR, MOTOR_ON);
      
    curr_status = new_status;
  }
}

MotorStatus motor_status();

MotorStatus motor_status()
{
  return curr_status;
}
