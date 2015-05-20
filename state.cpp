#include "state.h"
#include <Arduino.h>


unsigned long weld_time =0 ;


void torch_mode_control(bool *ptr_torch_trigger_pulled,WelderState welder_state){
  bool torch_trigger_pulled = *ptr_torch_trigger_pulled;
  unsigned long current_time = millis();
  switch(WELDING_MODE){
  case WELD_NORMAL:   
    break;
  case WELD_SPOT:
    if(torch_trigger_pulled){
      if(welder_state.current_state == WELDER_ON){ //change to WELDER_ON
	if( weld_time == 0){
	  weld_time = current_time + DURATION;
	}             
      }     
      if(weld_time<current_time && weld_time != 0){
	torch_trigger_pulled = false;
      } 
    }else{
      if(welder_state.current_state == WELDER_OFF 
	 || welder_state.current_state == WELDER_POST_FLOW){
	weld_time = 0;
      }
    }  
    
    break;
  case WELD_PULSE:
    if(torch_trigger_pulled){
      if(welder_state.current_state == WELDER_ON){
	if(weld_time == 0){
	  weld_time = current_time + OFF_DURATION;       
	}
      }     
      if(welder_state.current_state == WELDER_OFF 
	 || welder_state.current_state == WELDER_POST_FLOW){
	if(weld_time + ON_DURATION < current_time){
	  weld_time = 0;       
	}
      }
      if(weld_time<current_time && weld_time != 0){
	torch_trigger_pulled = false;
      }
    }else{
      if(welder_state.current_state == WELDER_OFF 
	 || welder_state.current_state == WELDER_POST_FLOW){
	weld_time = 0;
      }
    } 
    
    
    break;
  }  

  *ptr_torch_trigger_pulled = torch_trigger_pulled;
}


void set_outputs(WelderState welder_state){

 if(welder_state.wire_on){
   if(welder_state.current_state == WELDER_RUN_IN)
     setmotor(((float)WIRE_FEED_SPEED/100)*RUN_IN_SPEED);
   else{
     setmotor(WIRE_FEED_SPEED);
   }
 }else{
   setmotor(0);
 }
 
 if(welder_state.current_on){
   digitalWrite(PIN_CURRENT_ON,HIGH);
 }else{
   digitalWrite(PIN_CURRENT_ON,LOW);
 }

 if(welder_state.gas_on){
   digitalWrite(PIN_GAS_ON,HIGH);
 }else{
   digitalWrite(PIN_GAS_ON,LOW);
 }

}


void update_welder_state(boolean trigger, float amps,struct WelderState* state){
  unsigned long current_time = millis();

  boolean newstate = true;

  while(newstate){
  switch(state->current_state){
  case WELDER_OFF:
    state->gas_on = false;
    state->current_on = false;
    state->wire_on = false;
    if(trigger){
      state->current_state = WELDER_PRE_FLOW;
      state->next_state_time = current_time + (GAS_PRE_FLOW - ON_DELAY);
      newstate = true;
    }else{
      state->next_state_time = 0;
      newstate = true;
    }
    newstate = false;
    break;
  case WELDER_PRE_FLOW:
    state->gas_on = true;
    state->current_on = false;
    state->wire_on = false;
    if(trigger && current_time >= state->next_state_time){     
      state->current_state = WELDER_ON_DELAY;
      state->next_state_time = current_time + ON_DELAY;
      newstate = true;
    }else if(!trigger ){
      //arc could never have started, turn off
      state->current_state = WELDER_OFF;
      state->next_state_time = current_time; 
      newstate = true;
    }
    newstate = false;
    break;
  case WELDER_ON_DELAY:
    state->gas_on = true;
    state->current_on = true;
    state->wire_on = false;
    if(trigger && current_time >= state->next_state_time){
      state->current_state = WELDER_RUN_IN;
      state->next_state_time = current_time;
      newstate = true;
    }else if(!trigger){
      //arc should not have started, goto off
      state->current_state = WELDER_OFF;
      state->next_state_time = current_time; 
      newstate = true;
    }
    newstate = false;
    break;
  case WELDER_RUN_IN:
    state->gas_on = true;
    state->current_on = true;
    state->wire_on = true;
    //set run in speed here
    if(trigger && amps > 1){
      state->current_state = WELDER_ON;
      newstate = true;
    }else if(!trigger){
      //arc could be lit
      state->current_state = WELDER_OFF_DELAY;
      state->next_state_time = current_time + OFF_DELAY; 
      newstate = true;
    }
    newstate = false;
    break;
  case WELDER_ON:
    state->gas_on = true;
    state->current_on = true;
    state->wire_on = true;
    if(!trigger ){
      state->current_state = WELDER_OFF_DELAY;
      state->next_state_time = current_time + OFF_DELAY; 
      newstate = true;
    }
    newstate = false;
    break;
  case WELDER_OFF_DELAY:
    state->gas_on = true;
    state->current_on = true;
    state->wire_on = true;
    if( current_time > state->next_state_time){
      state->current_state = WELDER_BURN_BACK;
      state->next_state_time = current_time + BURN_BACK;
      newstate = true;
    }else if(trigger){
      state->current_state = WELDER_ON;
      newstate = true;
    }
    newstate = false;
    break;
  case WELDER_BURN_BACK:
    state->gas_on = true;
    state->current_on = true;
    state->wire_on = false;
    //always burn back correctly
    if( current_time >= state->next_state_time){
      state->next_state_time = current_time + (GAS_POST_FLOW - BURN_BACK);
      state->current_state = WELDER_POST_FLOW;
      newstate = true;
    }
    newstate = false;
    break;
  case WELDER_POST_FLOW:
    state->gas_on = true;
    state->current_on = false;
    state->wire_on = false;
    if(!trigger && current_time >= state->next_state_time){
      state->current_state = WELDER_OFF;
      state->next_state_time = 0;      
      newstate = true;
    }else if(trigger){
      state->current_state = WELDER_ON_DELAY;
      state->next_state_time = current_time + ON_DELAY;
      newstate = true;
    }
    newstate = false;
    break;
  }
}




}

