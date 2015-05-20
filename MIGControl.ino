/*
A control software for a MIG welder

*/

/*
                Start process
	       |   Pre-flow      |
       |  off  |       | on Delay|Run in Speed| normal |e
       
                Stop Process
	       |   Post-flow     |
       |  off  |       |burn back|  off delay | normal |

wire   |  off  |  off  |    off  | on         |   on   |
gas    |  off  |  on   |    on   | on         |   on   |
curent |  off  |  off  |    on   | on         |   on   |

trigger on moves to the right
trigger off starts moving to the leftt

                      Spot
       | Start Process | duration| Stop Process  |
wire   |       on      |   on    |     off       |
gas    |       on      |   on    |     off       |
curent |       on      |   on    |     off       |
trigger|       on      |   on    |     off       |



                      Pulse
       | Start Process | on duration | Stop Process  | off duration | start process | ....| Stop Process  |
wire   |       on      |     on      |     off       |     off      |      on       |     |     off       |
gas    |       on      |     on      |     off       |     off      |      on       |     |     off       |
curent |       on      |     on      |     off       |     off      |      on       |     |     off       |
trigger|       on      |     on      |     on        |     on       |      on       |     |     off       |

 */

#include <limits.h>
#include "encoder.h"

#include <EEPROM.h>
#include "my_eeprom.h"
#include "migmenu.h"
#include "motor.h"

#include "lcd.h"
#include "state.h"

#include "pins.h"











#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(23, 22, 16, 15, 14, 13); //tensy
LiquidCrystal lcd(2, 4, 5, 6, 7, 8); //arduino



// GLOBALS
boolean torch_trigger_pulled = false; // Trigger is up
boolean thermal_overload = true; //We should not be overloaded already
float voltage = 0; 
float amps = 0; 
struct WelderState welder_state = {WELDER_OFF,false,false,false,0};



void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  lcd.clear();
  pinMode (PIN_TORCH_TRIGGER,INPUT_PULLUP); //torch
  pinMode (PIN_THERMAL_SAFETY,INPUT); //thermal safety

  pinMode (PIN_VOLTAGE_READ,INPUT); //VOLTAGE sensor
  pinMode (PIN_CURRENT_READ,INPUT); //current sensor


  pinMode (PIN_CURRENT_ON,OUTPUT); //current
  pinMode (PIN_GAS_ON,OUTPUT); //gas



  //Encoder pins
  //  init_encoder(21,20,19);//teensy
  init_encoder(A5,A4,A3);//arduino

  //Motor control pins
  //  init_motor(12,11,10);//teensy
  init_motor(9,10,11);//arduino

  //start eeprom and load the last used profile
  //eeprom_clear();
  init_eeprom(&root_menu);
  //let switches stabilise
  delay(1000);

  init_menu(&root_menu);
}


void loop() {

  
  //GUI controls
  show_menu();
  
  //read torch
  torch_trigger_pulled = digitalRead(PIN_TORCH_TRIGGER);
  thermal_overload = digitalRead(PIN_THERMAL_SAFETY);
  
  voltage = (float)(analogRead(PIN_VOLTAGE_READ)*5)/1024.0;  //read and calculate voltage
  amps = ((float)(analogRead(PIN_CURRENT_READ)*300)/1024.0);  //read and calculate amps
 
 

  torch_mode_control(&torch_trigger_pulled,welder_state);
  
  
  if (thermal_overload){
    torch_trigger_pulled = false;
    display_string_value("OVER DUTY CYCLE!",1);
  }
 
 
  update_welder_state(torch_trigger_pulled,amps,&welder_state);
  //apply settings
 
 
 set_outputs(welder_state);

 
 
 render_display();
}

