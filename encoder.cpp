#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include "encoder.h"

byte ENCODER_PIN_A;
byte ENCODER_PIN_B;
byte ENCODER_PIN_BUTTON;


//-1 turn
//1 turn
//0 no change
int encoder_pin_A_last;
int read_encoder_rotation()
{
  int n = digitalRead(ENCODER_PIN_A);
  int ret = 0;
  if ((encoder_pin_A_last == LOW) && (n == HIGH)) {
    if (digitalRead(ENCODER_PIN_B) == LOW) {
      ret = -1;
    }else {
      ret = 1;
    }
  }
  encoder_pin_A_last = n;
  return ret;
}
//1 closed
//0 open
unsigned int encoder_button_reset;
int read_encoder_button_pushed(){
  if (digitalRead(ENCODER_PIN_BUTTON) == HIGH && 
      (millis()- encoder_button_reset>250)//wait 250ms before input again
      ) {
    encoder_button_reset = millis();
    return 1;
  }else{
    return 0;
  }
}

//same as read_encoder_rotation, but multiplies result based on 
//the interval between 4 rotations

unsigned long encoder_last_time;
unsigned int interval_sum;
int read_encoder_rotation_dynamic(){
  int ret = read_encoder_rotation();
  int multiplyer = 1;
  if (ret != 0) { 
    
    unsigned long this_interval = millis() - encoder_last_time;
    //remove 1/4 of the interval time
    interval_sum = interval_sum - (interval_sum>>2);
    interval_sum += this_interval;
    //reuse var
    this_interval = (interval_sum>>2);
    if(this_interval < 50){//50ms
      multiplyer = 1000;
    }else if(this_interval <100){//100ms
      multiplyer = 100;
    }else if(this_interval <200){//200ms
      multiplyer = 10;
    }
    encoder_last_time = millis();
  }
  return ret*multiplyer;
}
void init_encoder(byte pinA,byte pinB,byte pinButton){
  ENCODER_PIN_BUTTON = pinButton;
  ENCODER_PIN_A = pinA;
  ENCODER_PIN_B = pinB;
  pinMode (ENCODER_PIN_A,INPUT);
  pinMode (ENCODER_PIN_B,INPUT);
  pinMode (ENCODER_PIN_BUTTON,INPUT);
  encoder_pin_A_last = LOW;
  encoder_button_reset = millis();
  encoder_last_time = encoder_button_reset;
  interval_sum = 0;
}


#endif
