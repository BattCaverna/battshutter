#include "motor.h"

#define ENC_PIN 4

static int encoder_curr_pos = 0;
static bool prev_encoder;

void encoder_init()
{
  pinMode(ENC_PIN, INPUT);
  prev_encoder = digitalRead(ENC_PIN);
}

int encoder_poll()
{
  bool state = digitalRead(ENC_PIN);
  if (state && !prev_encoder)
  {
    //Positive edge
    if (motor_status() == MS_DOWN)
      encoder_curr_pos--;
    else if (motor_status() == MS_UP)
      encoder_curr_pos++;
  }

  prev_encoder = state;
  return encoder_curr_pos;
}

int encoder_position()
{
  return encoder_curr_pos;
}
