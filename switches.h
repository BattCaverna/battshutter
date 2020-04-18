#ifndef SWITCHES_H
#define SWITCHES_H

typedef enum Switches
{
  S_NONE,
  S_UP,
  S_DOWN,
  S_RESET_ENC,
  S_STOPMOVING,
  S_CNT
} Switches;

Switches switches_poll();

#endif
