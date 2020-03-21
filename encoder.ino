#include "motor.h"
#include "cfg.h"

static int encoder_curr_pos = 0;
static bool prev_encoder;

void encoder_init()
{
  pinMode(ENC_PIN, INPUT_PULLUP);
  prev_encoder = digitalRead(ENC_PIN);
}

int encoder_poll()
{
  bool state = digitalRead(ENC_PIN);
  if (state && !prev_encoder)
  {
    //Positive edge
    if (motor_direction() == MS_DOWN)
      encoder_curr_pos--;
    else if (motor_direction() == MS_UP)
      encoder_curr_pos++;
  }
  
  if (encoder_curr_pos < 0)
    encoder_curr_pos = 0;

  prev_encoder = state;
  return encoder_curr_pos;
}

int encoder_position()
{
  return encoder_curr_pos;
}
