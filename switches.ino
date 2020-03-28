#include "switches.h"
#include "cfg.h"

static long down_press_start;

void switches_init()
{
  pinMode(UP_SWITCH, INPUT_PULLUP);
  pinMode(DOWN_SWITCH, INPUT_PULLUP);
  down_press_start = millis();
}

Switches switches_poll();

Switches switches_poll()
{
  long now = millis();

  if (encoder_moving())
    down_press_start = now;

  if (digitalRead(UP_SWITCH) == 0)
  {
    down_press_start = now;
    return S_UP;
  }
  else if (digitalRead(DOWN_SWITCH) == 0)
  {
    // Check if we have reached the bottom home position: if we keep pressing the button
    // return an encoder reset event.
    if (now - down_press_start > RESET_ENC_TIME && !encoder_moving())
      return S_RESET_ENC;
    return S_DOWN;
  }

  down_press_start = now;
  return S_STOP;
}
