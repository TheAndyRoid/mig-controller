#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

//-1 turn
//1 turn
//0 no change
extern int encoder_pin_A_last;
int read_encoder_rotation();

//1 closed
//0 open
extern unsigned int encoder_button_reset;
int read_encoder_button_pushed();
//same as read_encoder_rotation, but multiplies result based on 
//the interval between 4 rotations

extern unsigned long encoder_last_time;
extern unsigned int interval_sum;

extern byte ENCODER_PIN_A;
extern byte ENCODER_PIN_B;
extern byte ENCODER_PIN_BUTTON;


int read_encoder_rotation_dynamic();
void init_encoder(byte pinA,byte pinB,byte pinButton);

#endif
