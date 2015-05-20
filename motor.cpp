
#include <Arduino.h>
#include "motor.h"

byte MOTOR_PIN_IN1;
byte MOTOR_PIN_IN2;
byte MOTOR_PIN_ENA;

//PWM, FEED IN MM PER MIN
struct pwm pwmLookup[]={
  {30,100},
  {127,2200},
  {255,22000}
};

unsigned int pwmtommpm(byte pwm){
//linear interpilation
unsigned int len = sizeof(pwmLookup)/sizeof(pwmLookup[0]);
unsigned int pwmhigh = len;
unsigned int pwmlow = 0;


 for(byte  i = 0;i < len ; i++){
   if(pwmLookup[i].pwm >= pwm ){
     pwmhigh = i;
     break;
   }    
 }
 for(byte  i = len;i-- > 0 ;){
   if(pwmLookup[i].pwm <= pwm ){
     pwmlow = i;
     break;
   }    
 }
 
 float d = pwmLookup[pwmhigh].pwm - pwmLookup[pwmlow].pwm;
 float dx = pwm - pwmLookup[pwmlow].pwm;
 float ratio =   dx/d;
 
 return pwmLookup[pwmlow].mm + (unsigned int)(ratio *(pwmLookup[pwmhigh].mm - pwmLookup[pwmlow].mm));
 
}



unsigned int pwmtoipm(byte pwm){
  //convert ipm to mmpm
  unsigned int mm =  pwmtommpm(pwm);
 return mm/25.4;
}

void init_motor(const int in1,const int in2,const int enA){

  MOTOR_PIN_IN1 = in1;
  MOTOR_PIN_IN2 = in2;
  MOTOR_PIN_ENA = enA;

  //  analogWriteFrequency(MOTOR_PIN_ENA, 187500);
  pinMode (MOTOR_PIN_IN1,OUTPUT); //12
  pinMode (MOTOR_PIN_IN2,OUTPUT); //11
  pinMode (MOTOR_PIN_ENA,OUTPUT); //10
}


unsigned long BRAKE_TIME = millis();
void setmotor(byte pwm){
  unsigned long current_time = millis();
  if(pwm == 0){
    if(BRAKE_TIME>current_time){
      analogWrite(MOTOR_PIN_ENA,0);
      //brake the motor
      digitalWrite(MOTOR_PIN_IN1,HIGH);
      digitalWrite(MOTOR_PIN_IN2,HIGH);
    }else{
      analogWrite(MOTOR_PIN_ENA,pwm);
      //brake the motor
      digitalWrite(MOTOR_PIN_IN1,HIGH);
      digitalWrite(MOTOR_PIN_IN2,LOW);
    }
    
  }else{
   
    analogWrite(MOTOR_PIN_ENA,clamp_motor_pwm(pwm));
    digitalWrite(MOTOR_PIN_IN1,HIGH);
    //digitalWrite(MOTOR_PIN_ENA,HIGH);
    //analogWrite(MOTOR_PIN_IN1,pwm);

    digitalWrite(MOTOR_PIN_IN2,LOW);
    BRAKE_TIME = current_time + MOTOR_REVERSE_DURATION;
  } 
}

 int get_min_pwm(){
  if( WIRE_FEED_UNITS == MPM || WIRE_FEED_UNITS == IPM ){
      return pwmLookup[0].pwm;
  }else{
    return 0;
  }
}
byte clamp_motor_pwm(byte pwm){
  if( WIRE_FEED_UNITS == MPM || WIRE_FEED_UNITS == IPM ){
    //return the smallest possible
    if (pwm < get_min_pwm()){
      pwm = get_min_pwm();
    }
  }
  return pwm;
}

