#include "motor.h"
#include "cfg.h"

typedef enum
{
  MFSM_STOP = 0,
  MFSM_WAIT_MOVE,
  MFSM_MOVE,
  MFSM_WAIT_STOP,
  MFSM_CNT,
} MotorFSM;

typedef MotorFSM (*motor_fsm_t)(MotorDir);

static long prev_move;
static MotorDir last_cmd;
static MotorDir motor_timeout;
static bool encoder_working;

static MotorFSM fsm_stop(MotorDir val);
static MotorFSM fsm_wait_move(MotorDir val);
static MotorFSM fsm_move(MotorDir val);
static MotorFSM fsm_wait_stop(MotorDir val);

static const motor_fsm_t fsm_states[MFSM_CNT] =
{
  fsm_stop,
  fsm_wait_move,
  fsm_move,
  fsm_wait_stop,
};
static MotorFSM curr_state;

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
  curr_state = MFSM_STOP;
  last_cmd = MS_STOP;
  motor_timeout = MS_STOP;
  encoder_working = false;
}

static void motor_setDir(MotorDir dir);

static void motor_setDir(MotorDir dir)
{
  if (dir == MS_DOWN)
    digitalWrite(MOTOR_DIR, MOTOR_DOWN);
  else
    digitalWrite(MOTOR_DIR, MOTOR_UP);
}

void motor_poll(MotorDir val)
{
  MotorFSM new_state = fsm_states[curr_state](val);
#if !MODBUS_ON
  if (new_state != curr_state)
    Serial.println(curr_state);
#endif
  curr_state = new_state;
}

static MotorFSM fsm_stop(MotorDir val)
{
  last_cmd = val;
  if (val == MS_STOP)
  {
    digitalWrite(MOTOR_PWR, MOTOR_OFF);
    return MFSM_STOP;
  }

  motor_setDir(val);
  prev_move = millis();

  return MFSM_WAIT_MOVE;
}

static MotorFSM fsm_wait_move(MotorDir val)
{
  if (val == MS_STOP)
  {
    motor_setDir(MS_STOP);
    return MFSM_STOP;
  }

  if (val != last_cmd)
  {
    motor_setDir(val);
    prev_move = millis();
    last_cmd = val;
  }

  if (millis() - prev_move >= GUARD_TIME)
  {
    digitalWrite(MOTOR_PWR, MOTOR_ON);
    motor_timeout = MS_STOP;
    return MFSM_MOVE;
  }

  return MFSM_WAIT_MOVE;
}

static MotorFSM fsm_move(MotorDir val)
{
  encoder_working = encoder_moving();
  if (val != last_cmd)
  {
    digitalWrite(MOTOR_PWR, MOTOR_OFF);
    prev_move = millis();
    return MFSM_WAIT_STOP;
  }

  if (millis() - prev_move > (MOTOR_TIMEOUT * 1000L))
  {
    digitalWrite(MOTOR_PWR, MOTOR_OFF);
    motor_timeout = last_cmd;
    prev_move = millis();
  }

  return MFSM_MOVE;
}

static MotorFSM fsm_wait_stop(MotorDir val)
{
  if (millis() - prev_move >= GUARD_TIME)
  {
    motor_setDir(MS_STOP);
    return MFSM_STOP;
  }

  return MFSM_WAIT_STOP;
}

MotorDir motor_currentDirection()
{
  return last_cmd;
}


MotorDir motor_position();

MotorDir motor_position()
{
  if (encoder_working)
  {
    if (encoder_position() == 100)
      return MS_UP;
    else if (encoder_position() == 0)
      return MS_DOWN;
    else
      return MS_STOP;
  }
  else
    return motor_timeout;
}
