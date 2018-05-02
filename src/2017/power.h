#include <Arduino.h>

#ifndef HYDRINO_POWER
#define HYDRINO_POWER

#define ON true
#define OFF false

struct cycle {
  unsigned int power:10;
  uint8_t cycle:2;
  uint8_t timing:4;
  unsigned long long sleep_start = 0;
  unsigned long long sleep_until = 0;
  uint8_t sleep_cycles = 0;
} __attribute__((packed));

typedef cycle cycle;

/**
 * Because we're now using only 4 bits for the timing, the maximum ratio between
 * on and off times is 1:15 or 6.25% on (1.0/16 on/total). If a smaller ratio is
 * desired, overflow into the next cycle. eg: 15 off, 1 on, 0 off, 5 off = 1:20
 */
struct cycle_timing {
  unsigned int power:10;
  uint8_t reserved:6;
  uint8_t times[] = {};

  bool motorA1:1;
  bool motorB1:1;
  bool motorA2:1;
  bool motorB2:1;
  bool motorA3:1;
  bool motorB3:1;
  bool motorA4:1;
  bool motorB4:1;
} __attribute__((packed));

typedef cycle_timing cycle_timing;

cycle_timing generateCycleTiming(int power,
  uint8_t one, bool a1, bool b1,
  uint8_t two, bool a2, bool b2,
  uint8_t three, bool a3, bool b3,
  uint8_t four, bool a4, bool b4){
  cycle_timing cycleTiming;
  cycleTiming.power = power;

  cycleTiming.one = one;
  cycleTiming.two = two;
  cycleTiming.three = three;
  cycleTiming.four = four;

  cycleTiming.motorA1 = a1;
  cycleTiming.motorB1 = b1;
  cycleTiming.motorA2 = a2;
  cycleTiming.motorB2 = b2;
  cycleTiming.motorA3 = a3;
  cycleTiming.motorB3 = b3;
  cycleTiming.motorA4 = a4;
  cycleTiming.motorB4 = b4;
  return cycleTiming;
}

#endif
