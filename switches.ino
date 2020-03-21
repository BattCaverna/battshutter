#include "switches.h"
#include "cfg.h"

void switches_init()
{
  pinMode(UP_SWITCH, INPUT_PULLUP);
  pinMode(DOWN_SWITCH, INPUT_PULLUP);
}

Switches switches_poll();

Switches switches_poll()
{
  if (digitalRead(UP_SWITCH) == 0)
    return S_UP;
  else if (digitalRead(DOWN_SWITCH) == 0)
    return S_DOWN;
  else
    return S_STOP;
}
