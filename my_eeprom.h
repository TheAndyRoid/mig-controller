#ifndef EEPROM_H
#define EEPROM_H



#include "migmenu.h"
#include <EEPROM.h>

#define EEPROMSIZE 512
/*
Read menu value from eeprom
*/


extern unsigned int eeprom_current_profile_num; //1 is first user profile 0 is defaults
extern unsigned int eeprom_start_offset;  //bytes
extern unsigned int eeprom_num_profiles; //calculated at startup  
extern unsigned int eeprom_profile_size; //calculated at startup in ints

unsigned int eeprom_read(struct Menu* menu, unsigned int menu_item);

/*
write value to eeprom
*/
void eeprom_update(struct Menu* menu, unsigned int menu_item);



int eeprom_calc_addr(struct Menu* menu, unsigned int menu_item, int profile_num);
int eeprom_calc_addr(struct Menu * current_menu,
		     int* address,
		     struct Menu* target_menu, 
		     unsigned int target_menu_item);

void init_eeprom(struct Menu *menu);


void eeprom_write_uint(int addr, unsigned int value);
unsigned int eeprom_read_uint(int addr);
void eeprom_update_uint(int addr,unsigned int value);



int eeprom_calc_size(struct Menu *menu);


void eeprom_load_profile(struct Menu *menu,int profile_num);
void eeprom_save_profile(struct Menu *menu,int profile_num);
unsigned int eeprom_current_profile();
void eeprom_clear();

#endif
