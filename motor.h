#ifndef MOTOR_H
#define MOTOR_H


#include <Arduino.h>
#include "migmenu.h"

//note can't excede 65535 mm per minute
struct pwm{
  byte pwm;
  unsigned int mm;
};

extern struct pwm pwmLookup[];



void setmotor(byte pwm);

unsigned int pwmtommpm(byte pwm);
unsigned int pwmtoipm(byte pwm);
unsigned int pwmtompm(byte pwm);
void init_motor(const int in1,const int in2,const int enA);
int get_min_pwm();
byte clamp_motor_pwm(byte pwm);



#endif
