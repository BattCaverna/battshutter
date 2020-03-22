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
  if (digitalRead(UP_SWITCH) == 0)
  {
    down_press_start = millis();
    return S_UP;
  }
  else if (digitalRead(DOWN_SWITCH) == 0)
  {
    // Check if we have reached the bottom home position: if we keep pressing the button an
    // return an encoder reset event.
    if (millis() - down_press_start > RESET_ENC_TIME && !encoder_moving())
      return S_RESET_ENC;
    return S_DOWN;
  }
  
  down_press_start = millis();
  return S_STOP;
}
