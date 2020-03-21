#ifndef SWITCHES_H
#define SWITCHES_H

typedef enum Switches
{
  S_STOP,
  S_UP,
  S_DOWN,
  S_CNT
} Switches;

Switches switches_poll();

#endif
