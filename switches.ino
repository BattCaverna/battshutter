#include "switches.h"
#include "cfg.h"

void switches_init()
{
  pinMode(UP_SWITCH, INPUT_PULLUP);
  pinMode(DOWN_SWITCH, INPUT_PULLUP);
}

Switches switches_poll();

#define SW_DEBOUNCE_TIME 50 //ms

static bool switches_debounce(Switches curr_sw)
{
  static Switches prev_sw = S_NONE;
  static long last_key_start = 0;

  long now = millis();

  if (prev_sw != curr_sw)
    last_key_start = now;

  prev_sw = curr_sw;

  return (now - last_key_start > SW_DEBOUNCE_TIME);
}

static Switches long_press = S_NONE;

Switches switches_poll()
{
  static Switches prev_sw = S_NONE;
  static long last_key_start = 0;
  static long down_press_start = 0;

  Switches curr_sw;

  long now = millis();

  if (encoder_moving() || motor_position_time() != 0)
    down_press_start = now;

  if (digitalRead(UP_SWITCH) == 0)
  {
    down_press_start = now;
    curr_sw = S_UP;
  }
  else if (digitalRead(DOWN_SWITCH) == 0)
  {
    // Check if we have reached the bottom home position: if we keep pressing the button
    // return an encoder reset event.
    if (now - down_press_start > RESET_ENC_TIME && !encoder_moving() && motor_position_time() == 0)
      curr_sw = S_RESET_ENC;
    else
      curr_sw = S_DOWN;
  }
  else
  {
    down_press_start = now;
    curr_sw = S_NONE;
  }

  // Debounce
  if (!switches_debounce(curr_sw))
    return prev_sw;

  // Auto up/down handling
  if (prev_sw != curr_sw)
  {
    last_key_start = now;
    if (curr_sw != S_NONE)
      long_press = S_NONE;
  }
  else
  {
    if (now - last_key_start > LONG_PRESS_TIME && curr_sw != S_NONE)
      long_press = curr_sw;

    // Avoid keeping key pressed forever.
    // Use MOTOR_TIMEOUT +10%
    if (now - last_key_start > (long)(1.1 * MOTOR_TIMEOUT * 1000))
      long_press = S_NONE;
  }

  prev_sw = curr_sw;

  if (long_press != S_NONE)
    return long_press;
  else
    return curr_sw;
}

void switche_resetLongPression()
{
  long_press = S_NONE;
}
