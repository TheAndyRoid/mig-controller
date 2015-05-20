#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

#include "migmenu.h"
#include "motor.h"
#include "pins.h"

#define WELDER_OFF 0
#define WELDER_PRE_FLOW 1
#define WELDER_ON_DELAY 2
#define WELDER_RUN_IN 3
#define WELDER_ON 4
#define WELDER_OFF_DELAY 5
#define WELDER_BURN_BACK 6
#define WELDER_POST_FLOW 7


struct WelderState{
  byte current_state;
  boolean current_on;
  boolean gas_on;
  boolean wire_on;
  unsigned long next_state_time; //time till we can advance to the next state
};

extern unsigned long weld_time;
void update_welder_state(boolean trigger, float amps,struct WelderState* state);
void set_outputs(WelderState welder_state);
void torch_mode_control(bool *ptr_torch_trigger_pulled,WelderState welder_state);

#endif
