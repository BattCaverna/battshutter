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
static MotorPos motor_timeout;

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

static long motor_start;
static bool motor_schedule_calibration = false;
static bool motor_calibration = false;

void motor_init()
{
  pinMode(MOTOR_PWR, OUTPUT);
  digitalWrite(MOTOR_PWR, MOTOR_OFF);
  pinMode(MOTOR_DIR, OUTPUT);
  digitalWrite(MOTOR_DIR, MOTOR_UP);
  curr_state = MFSM_STOP;
  last_cmd = MS_STOP;
  motor_timeout = MP_MIDDLE;
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
    motor_start = millis();
    motor_timeout = MP_MIDDLE;
    motor_calibration = motor_schedule_calibration;
    motor_schedule_calibration = false;
    return MFSM_MOVE;
  }

  return MFSM_WAIT_MOVE;
}

static MotorFSM fsm_move(MotorDir val)
{
  if (val != last_cmd)
  {
    digitalWrite(MOTOR_PWR, MOTOR_OFF);
    prev_move = millis();
    return MFSM_WAIT_STOP;
  }

  long delta = millis() - motor_start;
  motor_start = millis();
  if (val == MS_UP)
    shutter_cfg.motor_time += delta;
  else
    shutter_cfg.motor_time -= delta;

  shutter_cfg.motor_time = shutter_cfg.motor_time < 0 ? 0 : shutter_cfg.motor_time;

  if (motor_calibration)
    shutter_cfg.motor_time_max = shutter_cfg.motor_time > shutter_cfg.motor_time_max ? shutter_cfg.motor_time : shutter_cfg.motor_time_max;
  else
    shutter_cfg.motor_time = shutter_cfg.motor_time > shutter_cfg.motor_time_max ? shutter_cfg.motor_time_max : shutter_cfg.motor_time;

  if (millis() - prev_move > (MOTOR_TIMEOUT * 1000L))
  {
    digitalWrite(MOTOR_PWR, MOTOR_OFF);
    motor_timeout = (MotorPos)last_cmd;
    prev_move = millis();
  }
  else
    encoder_resetWorking();

  return MFSM_MOVE;
}

static MotorFSM fsm_wait_stop(MotorDir val)
{
  (void)val;
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

int motor_position()
{
  return (shutter_cfg.motor_time * 100L + shutter_cfg.motor_time_max / 2) / shutter_cfg.motor_time_max;
}

void motor_scheduleCalibration()
{
  shutter_cfg.motor_time = 0;
  shutter_cfg.motor_time_max = 1;
  motor_schedule_calibration = true;
}

long motor_position_time()
{
  return shutter_cfg.motor_time;
}

long motor_max()
{
  return shutter_cfg.motor_time_max;
}

MotorPos motor_timeoutStatus();

MotorPos motor_timeoutStatus()
{
  return motor_timeout;
}

void motor_setMax(long val)
{
  if (val < 0)
    val = 0;

  if (val > MOTOR_TIMEOUT * 1000L)
    val = MOTOR_TIMEOUT * 1000L;

  shutter_cfg.motor_time_max = val;
}

void motor_setCurr(long val)
{
  if (val < 0)
    val = 0;

  shutter_cfg.motor_time = val;
}
