#ifndef MIGMENU_H
#define MIGMENU_H


#include <Arduino.h>
#define MENUBACK 0
#define SETTINGSECONDS 1
#define SETTINGSTRING 2
#define SETTINGPERCENTAGE 3
#define SUBMENU 4
#define ROOTMENU 5
#define MAINMENU 6





struct MenuItem{
  byte type;
  const char *name;
  unsigned int value;
  void *data;
};
struct Menu{
  struct MenuItem * items;  
  unsigned int num_items;
  struct Menu* (*process)(struct Menu*,unsigned int *menu_item,boolean *setting_edit);  //gui function
  const char* name;
};



const char seconds_string[]=  "SECONDS";
const char cancel_string[] = "CANCEL";
const char back_string[] = "BACK";

extern const char* reset_settings_name[];

#define MPM 0
#define IPM 1
#define PWM 2

extern const char* wire_feed_unit_name[];

#define WELD_NORMAL 0
#define WELD_SPOT 1
#define WELD_PULSE 2

extern const char* weld_mode_name[];




struct Menu*  int_process(struct Menu* menu,unsigned int *current_menu_item,boolean *setting_edit);
struct Menu*  percentage_process(struct Menu* menu,unsigned int *current_menu_item,boolean *setting_edit);
struct Menu*  string_process(struct Menu* menu,unsigned int *current_menu_item,boolean *setting_edit);
struct Menu*  menu_process(struct Menu* current_menu,unsigned int *ptr_current_menu_item,boolean *setting_edit);
struct Menu*  root_process(struct Menu* menu,unsigned int *current_menu_item,boolean *setting_edit);


extern struct Menu settings_menu;
extern struct Menu root_menu; 

extern boolean setting_edit;
extern Menu *current_menu ;
extern unsigned int current_menu_item ;


void init_menu(Menu *menu);
void show_menu();



#define DURATION  spot_setting_items[0].value
extern struct MenuItem spot_setting_items[];
extern struct Menu spot_setting_menu;


#define OFF_DURATION  pulse_setting_items[0].value
#define ON_DURATION  pulse_setting_items[1].value

extern struct MenuItem pulse_setting_items[];
extern struct Menu pulse_setting_menu;



#define WELDING_MODE  setting_items[0].value
#define WIRE_FEED_UNITS setting_items[1].value
#define RUN_IN_SPEED setting_items[4].value
#define BURN_BACK setting_items[5].value
#define ON_DELAY setting_items[6].value
#define OFF_DELAY setting_items[7].value
#define GAS_PRE_FLOW setting_items[8].value
#define GAS_POST_FLOW setting_items[9].value
#define MOTOR_REVERSE_DURATION setting_items[10].value

extern struct MenuItem setting_items[];
extern struct MenuItem root_items[];
#define WIRE_FEED_SPEED root_items[0].value

extern float voltage;
extern float amps;


#endif
