#include <EEPROM.h>
#include "motor.h"
#include "cfg.h"

static int encoder_curr_pos = 0;
static int last_curr_pos = 0;
static bool prev_encoder;
static long last_edge;
static long last_save;
static int encoder_max_pos = 1;
static int last_max_pos = 0;

#define ENC_MAX_EEADDR 0

static void encoder_eepSave()
{
  if (millis() - last_save > EEP_SAVE_INTERVAL)
  {
    if (last_max_pos != encoder_max_pos)
      EEPROM.put(ENC_MAX_EEADDR, encoder_max_pos);

    if (last_curr_pos != encoder_curr_pos)
      EEPROM.put(ENC_MAX_EEADDR + sizeof(encoder_max_pos), encoder_curr_pos);

    last_curr_pos = encoder_curr_pos;
    last_max_pos = encoder_max_pos;
    last_save = millis();
  }
}

void encoder_init()
{
  pinMode(ENC_PIN, INPUT_PULLUP);
  prev_encoder = digitalRead(ENC_PIN);
  last_edge = 0;
  last_save = 0;
  EEPROM.get(ENC_MAX_EEADDR, encoder_max_pos);
  EEPROM.get(ENC_MAX_EEADDR + sizeof(encoder_max_pos), encoder_curr_pos);

  // Check if eeprom is valid
  if (encoder_max_pos < 0)
    encoder_max_pos = 1;

  if (encoder_curr_pos < 0)
    encoder_curr_pos = 0;
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

int encoder_max()
{
  return encoder_max_pos;
}

void encoder_setMax(int new_max)
{
  if (new_max < 1)
    new_max = 1;
  encoder_max_pos = new_max;
}

void encoder_setCurr(int new_curr)
{
  if (new_curr < 0)
    new_curr = 0;
  encoder_curr_pos = new_curr;
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

  encoder_eepSave();
}

// Positin in % (0 = closed, 100% = fully open)
int encoder_position()
{
  return (encoder_curr_pos * 100L + encoder_max_pos / 2) / encoder_max_pos;
}

// position in encoder steps
int encoder_position_step()
{
  return encoder_curr_pos;
}
