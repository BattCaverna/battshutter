#include "motor.h"
#include "cfg.h"

static bool prev_encoder;
static bool do_pulse;
static long last_edge;
static bool encoder_working = false;

void encoder_init()
{
  pinMode(ENC_PIN, INPUT);
  prev_encoder = digitalRead(ENC_PIN);
  last_edge = millis() - MAX_ENC_PERIOD - 1;
}

void encoder_resetWorking()
{
  encoder_working = false;
}

bool encoder_moving()
{
  return (millis() - last_edge) < MAX_ENC_PERIOD;
}

void encoder_reset()
{
  shutter_cfg.encoder_curr_pos = 0;
  shutter_cfg.encoder_max_pos = 1;
}

int encoder_max()
{
  return shutter_cfg.encoder_max_pos;
}

void encoder_setMax(int new_max)
{
  if (new_max < 1)
    new_max = 1;
  shutter_cfg.encoder_max_pos = new_max;
}

void encoder_setCurr(int new_curr)
{
  if (new_curr < 0)
    new_curr = 0;
  shutter_cfg.encoder_curr_pos = new_curr;
}


#define ENC_DEBOUNCE_TIME 50 //ms

void encoder_poll()
{
  bool state = digitalRead(ENC_PIN);

  if (!state && prev_encoder)
  {
    // falling edge
    last_edge = millis();
    do_pulse = true;
  }


  if (millis() - last_edge > ENC_DEBOUNCE_TIME && do_pulse)
  {
    do_pulse = false;
    encoder_working = true;

    if (motor_currentDirection() == MS_DOWN)
      shutter_cfg.encoder_curr_pos--;
    else if (motor_currentDirection() == MS_UP)
      shutter_cfg.encoder_curr_pos++;

    if (shutter_cfg.encoder_curr_pos < 0)
      shutter_cfg.encoder_curr_pos = 0;

    if (shutter_cfg.encoder_curr_pos > shutter_cfg.encoder_max_pos)
      shutter_cfg.encoder_max_pos = shutter_cfg.encoder_curr_pos;
  }

  prev_encoder = state;
}

// Positin in % (0 = closed, 100% = fully open)
int encoder_position()
{
  // Use motor timeout as an heuristic if encoder is not connected
  if  ((millis() - last_edge > MAX_ENC_PERIOD) && !encoder_working)
    return motor_position();
  else
    return (shutter_cfg.encoder_curr_pos * 100L + shutter_cfg.encoder_max_pos / 2) / shutter_cfg.encoder_max_pos;
}

// position in encoder steps
int encoder_position_step()
{
  return shutter_cfg.encoder_curr_pos;
}
