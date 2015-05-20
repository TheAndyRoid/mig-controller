
#include <Arduino.h>
#include "migmenu.h"
#include "encoder.h"
#include "lcd.h"
#include "motor.h"
#include "my_eeprom.h"




const char* wire_feed_unit_name[]={
"MPM",
"IPM",
"PWM",
NULL
};

const char* weld_mode_name[]={
"NORMAL",
"SPOT",
"PULSE",
NULL
};





#define DURATION  spot_setting_items[0].value
struct MenuItem spot_setting_items[]={
{SETTINGSECONDS,"DURATION",1000,NULL},
{MENUBACK,back_string,0,&settings_menu}
};
struct Menu spot_setting_menu = { spot_setting_items,
				  sizeof(spot_setting_items)/sizeof(spot_setting_items[0]),
				  menu_process,"SPOT SETTINGS"};


#define OFF_DURATION  pulse_setting_items[0].value
#define ON_DURATION  pulse_setting_items[1].value
struct MenuItem pulse_setting_items[]={
{SETTINGSECONDS,"ON DURATION",500,NULL},
{SETTINGSECONDS,"OFF DURATION",500,NULL},
{MENUBACK,back_string,0,&settings_menu}
};
struct Menu pulse_setting_menu = { pulse_setting_items,
				   sizeof(pulse_setting_items)/sizeof(pulse_setting_items[0]),
				   menu_process,
				   "PULSE SETTINGS"};



#define WELDING_MODE  setting_items[0].value
#define WIRE_FEED_UNITS setting_items[1].value
#define RUN_IN_SPEED setting_items[4].value
#define BURN_BACK setting_items[5].value
#define ON_DELAY setting_items[6].value
#define OFF_DELAY setting_items[7].value
#define GAS_PRE_FLOW setting_items[8].value
#define GAS_POST_FLOW setting_items[9].value
#define MOTOR_REVERSE_DURATION setting_items[10].value

 

struct MenuItem setting_items[]={
  {SETTINGSTRING,"WELDING MODE",0,weld_mode_name},
  {SETTINGSTRING,"WIRE FEED UNITS",2,wire_feed_unit_name},
  {SUBMENU,"SPOT SETTINGS",0,&spot_setting_menu},
  {SUBMENU,"PULSE SETTINGS",0,&pulse_setting_menu},
  {SETTINGPERCENTAGE,"RUN IN SPEED",0,NULL},
  {SETTINGSECONDS,"BURN BACK",20,NULL},
  {SETTINGSECONDS,"ON DELAY",0,NULL},
  {SETTINGSECONDS,"OFF DELAY",20,NULL},
  {SETTINGSECONDS,"GAS PRE FLOW",200,NULL},
  {SETTINGSECONDS,"GAS POST FLOW",1000,NULL},
  {SETTINGSECONDS,"MOTOR REVERSE",200,NULL},
  {ROOTMENU,cancel_string,0,(void*)&root_menu}
};

struct Menu settings_menu = { setting_items,
			      sizeof(setting_items)/sizeof(setting_items[0])
			      ,menu_process,
			      "SETTINGS"};

struct MenuItem root_items[]={
  {MAINMENU,NULL,50,&settings_menu}
};

struct Menu root_menu = { root_items,
			  sizeof(root_items)/sizeof(root_items[0]),
			  root_process,
			  NULL};



boolean setting_edit;
Menu *current_menu ;
unsigned int current_menu_item ;















struct Menu*  int_process(struct Menu* current_menu,unsigned int *ptr_current_menu_item,boolean *setting_edit){
  
  if (read_encoder_button_pushed()) {
    *setting_edit = false;
    //save the setting
    eeprom_save_profile(&root_menu,eeprom_current_profile());
    return current_menu;
  }

  int rot = read_encoder_rotation_dynamic();
  unsigned int current_menu_item = *ptr_current_menu_item;
  struct MenuItem* menu_item = &(current_menu->items[current_menu_item]);
  

  menu_item->value += rot;
 
  if(menu_item->value > 100000){
    menu_item->value = 0;
  }

  display_string_value(menu_item->name,0);
  char display_value[16];
  sprintf(display_value," %2.1u.%03u %7s ",(menu_item->value / 1000),(menu_item->value % 1000),seconds_string);
  display_string_value(display_value,1);



  return current_menu;
}

struct Menu*  string_process(struct Menu* current_menu,unsigned int *ptr_current_menu_item,boolean *setting_edit){

  if (read_encoder_button_pushed()) {
    *setting_edit = false;
    //save the setting
    eeprom_save_profile(&root_menu,eeprom_current_profile());
    return current_menu;
  }

  int rot = read_encoder_rotation();
  unsigned int current_menu_item = *ptr_current_menu_item;
  struct MenuItem* menu_item = &(current_menu->items[current_menu_item]);
  char **data  = (char**)menu_item->data;
  
  if(menu_item->value == 0 && rot < 0){
    //wrap to last
    while(data[menu_item->value+1] != NULL){
      menu_item->value++;
     }    
  }else if(rot > 0 && data[menu_item->value + rot] == NULL){
    menu_item->value=0;
  }else{
    menu_item->value += rot;
  }


  display_string_value(menu_item->name,0);
  display_string_value(data[menu_item->value],1);



  return current_menu;
}

struct Menu*  percentage_process(struct Menu* current_menu,unsigned int *ptr_current_menu_item,boolean *setting_edit){

  if (read_encoder_button_pushed()) {
    *setting_edit = false;
    eeprom_save_profile(&root_menu,eeprom_current_profile());
    return current_menu;
  }

  int rot = read_encoder_rotation_dynamic();
  unsigned int current_menu_item = *ptr_current_menu_item;
  struct MenuItem* menu_item = &(current_menu->items[current_menu_item]);
  
  if(menu_item->value < abs(rot) && rot < 0){
    //wrap to last
    menu_item->value = 100;
        
  }else if(rot + menu_item->value > 100 ){
    menu_item->value=0;
  }else{
    menu_item->value += rot;
  }


    char display_value[16];
  sprintf(display_value,"     %3.1u%%       ",menu_item->value);

  display_string_value(menu_item->name,0);
  display_string_value(display_value,1);



  return current_menu;
}

struct Menu*  root_process(struct Menu* current_menu,unsigned int *ptr_current_menu_item,boolean *setting_edit){
  

   struct MenuItem* menu_item = &(current_menu->items[*ptr_current_menu_item]);
   
   if (read_encoder_button_pushed()) {
     //save the setting
     eeprom_save_profile(&root_menu,eeprom_current_profile());
     eeprom_load_profile(&root_menu,eeprom_current_profile());
     //go to submenu
     *setting_edit = false;
     current_menu = (Menu*)menu_item->data;
     *ptr_current_menu_item = 0;
     
      return current_menu;      
   }

   int rot = read_encoder_rotation_dynamic();

   if(rot + (int)menu_item->value < get_min_pwm()){
     if ((int)menu_item->value < get_min_pwm()){
       menu_item->value = get_min_pwm();   
     }else{
       menu_item->value = 255;   
     }
   }else if(rot + menu_item->value > 255 ){
     menu_item->value=get_min_pwm();
   }else{
     menu_item->value += rot;
   }

   //read amps and current




   float volts = voltage;

   //split into upper and lower
   int upper_amp = amps;
   int lower_amp = (amps*10)-upper_amp*10;
   int upper_volt = volts;
   int lower_volt = (volts*10)-upper_volt*10;


   //convert 
   char display_row0[16];
   sprintf(display_row0,"%3.1u.%1uA    %3.1u.%1uV" ,upper_amp,lower_amp,upper_volt,lower_volt);
   display_string_value(display_row0,0);
   char display_row1[16];

   
   switch(WIRE_FEED_UNITS){
   case MPM:{
     unsigned int mmpm = pwmtommpm((byte)menu_item->value);
     unsigned int mpmU = (mmpm / 1000); //whole meters
     unsigned int mpmL = (mmpm%1000)/10; //centermeters
     sprintf(display_row1,"%-6s %2.1u.%2.2u %3s",
	     weld_mode_name[WELDING_MODE],
	     mpmU,
	     mpmL,
	     wire_feed_unit_name[WIRE_FEED_UNITS]);     
     break;
   }
   case IPM:{
     sprintf(display_row1,"%-6s   %3.1u %3s",
	     weld_mode_name[WELDING_MODE],
	     pwmtoipm((byte)menu_item->value),
	     wire_feed_unit_name[WIRE_FEED_UNITS]);
     break; }    
   case PWM:{
     sprintf(display_row1,"%-6s   %3.1u %3s",
	     weld_mode_name[WELDING_MODE],
	     menu_item->value,
	     wire_feed_unit_name[WIRE_FEED_UNITS]);
     break;     
   }}

   
   display_string_value(display_row1,1);
   


   return current_menu;

}

struct Menu*  menu_process(struct Menu* current_menu,unsigned int *ptr_current_menu_item,boolean *setting_edit){
 
   unsigned int current_menu_item = *ptr_current_menu_item;
   
   //editing a setting
   if(*setting_edit){
    switch(current_menu->items[current_menu_item].type){
    case SETTINGSECONDS:
      current_menu = int_process(current_menu,&current_menu_item,setting_edit);
      break;
    case SETTINGSTRING:
      current_menu = string_process(current_menu,&current_menu_item,setting_edit);
      break;
    case SETTINGPERCENTAGE:
      current_menu = percentage_process(current_menu,&current_menu_item,setting_edit);
      break;
    }
    return current_menu;
  }

  //navigating menu
   int rot = read_encoder_rotation();    
   MenuItem* menu_item = &(current_menu->items[current_menu_item]);
  //menu input processing
  if(current_menu_item == 0 && rot < 0){
    //wrap to last
    while(menu_item->type != MENUBACK && menu_item->type != ROOTMENU){
      current_menu_item++;
      menu_item = &(current_menu->items[current_menu_item]);
    }
  }else if(current_menu_item == current_menu->num_items-1 && rot > 0){
    //wrap to first
    current_menu_item = 0; 
  }else{
    current_menu_item += rot;  
  }

  //button pushed
  if (read_encoder_button_pushed()) {
    switch (menu_item->type){
    case SUBMENU:{
      //got a sub menue to go into
      *setting_edit = false;
      current_menu = (Menu*)menu_item->data;
      current_menu_item = 0;
    }
      break;
    case ROOTMENU:
      *setting_edit = true;
      current_menu_item = 0;
      current_menu = (Menu*)menu_item->data;
      break;
    case MENUBACK:{
      //go back to the correct menu
      *setting_edit = false;
      current_menu_item = 0;
      Menu *new_menu = (Menu*)menu_item->data;
      if ( new_menu == NULL){
      	break;
      }      
      while( new_menu->items[current_menu_item].data != current_menu){
      	current_menu_item++;
      }
      current_menu = (Menu*)menu_item->data;
    }break;
    case SETTINGSECONDS:{
      *setting_edit = true;
    }break;
    case SETTINGSTRING:{
      *setting_edit = true;
    }break;
    case SETTINGPERCENTAGE:{
      *setting_edit = true;
    }break;
    }
  }
  
  menu_item = &current_menu->items[current_menu_item];

  *ptr_current_menu_item = current_menu_item;
  
  
  display_string_value(current_menu->name,0);
  display_string_value(menu_item->name,1);

  return current_menu;

}


void init_menu(Menu *menu){
  setting_edit = false;
  current_menu  = menu;
  current_menu_item  = 0;
}

void show_menu(){
  current_menu = (*current_menu->process)(current_menu,&current_menu_item,&setting_edit);
}
