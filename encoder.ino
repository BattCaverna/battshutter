#include "motor.h"
#include "cfg.h"

static long encoder_curr_pos = 0;
static bool prev_encoder;
static long last_edge;
static long encoder_max_pos = 1;

void encoder_init()
{
  pinMode(ENC_PIN, INPUT_PULLUP);
  prev_encoder = digitalRead(ENC_PIN);
  last_edge = 0;
}

bool encoder_moving()
{
  return (millis() - last_edge) < MAX_ENC_PERIOD;
}

void encoder_reset()
{
  encoder_curr_pos = 0;
  encoder_max_pos = 1;
}

long encoder_max()
{
  return encoder_max_pos;
}


void encoder_poll()
{
  bool state = digitalRead(ENC_PIN);
  if (state && !prev_encoder)
  {
    //Positive edge
    last_edge = millis();
    
    if (motor_direction() == MS_DOWN)
      encoder_curr_pos--;
    else if (motor_direction() == MS_UP)
      encoder_curr_pos++;
  }
  
  if (encoder_curr_pos < 0)
    encoder_curr_pos = 0;

  if (encoder_curr_pos > encoder_max_pos)
    encoder_max_pos = encoder_curr_pos;

  prev_encoder = state;
}

int encoder_position()
{
  return (encoder_curr_pos * 100 + encoder_max_pos / 2) / encoder_max_pos;
}
