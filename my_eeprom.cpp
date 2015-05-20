

#include "my_eeprom.h"
#include "migmenu.h"
#include <Arduino.h>


unsigned int eeprom_current_profile_num = 2; //1 is first user profile 0 is defaults
unsigned int eeprom_start_offset = 2;  //bytes
unsigned int eeprom_num_profiles = 10; //calculated at startup  
unsigned int eeprom_profile_size = 0; //calculated at startup in ints




void init_eeprom(struct Menu *menu){
  eeprom_profile_size = eeprom_calc_size(menu);
  if(EEPROM.read(0) == 255||
     EEPROM.read(0) == 0){
    //clean eeprom 
    eeprom_clear();
    EEPROM.write(0, eeprom_current_profile_num );
    for(unsigned int i = 0; i < eeprom_num_profiles; i++){
      eeprom_save_profile(menu,i); //save the defaults
    }    
  }
  eeprom_current_profile_num = EEPROM.read(0);
  eeprom_load_profile(menu,eeprom_current_profile_num);
}


bool is_menu_item(int type){
  if (type == SUBMENU ||
      type == MAINMENU){
    return true;
  }else{
    return false;
  }
}
bool is_value_item(int type){
  if ( type == SETTINGSECONDS ||
       type == SETTINGPERCENTAGE || 
       type == MAINMENU || 
       type == SETTINGSTRING){
    return true;
  }else{
    return false;
  }
}






int eeprom_calc_addr(struct Menu* target_menu, 
		     unsigned int target_menu_item,
		     int profile_num){
  int address = eeprom_start_offset + (eeprom_profile_size*profile_num); 
  //calculate profile offset
  if(eeprom_calc_addr(&root_menu,&address,target_menu,target_menu_item) == 0){
    return address;    
  }else{
    return -1;
  }
}				

  

//returns -1 when not found
//return 0 when found
//address will be set to the correct address for the given item
//increment the address once for each saveable value passed
int eeprom_calc_addr(struct Menu * current_menu,
		     int* address,
		     struct Menu* target_menu, 
		     unsigned int target_menu_item){
  

  for(unsigned int i = 0; i<current_menu->num_items;i++){
    if (current_menu == target_menu && i == target_menu_item){
      //item was found return
      return 0;
    }  
    
    if (is_value_item(current_menu->items[i].type)){
      //uint is 2 bytes
      *address = *address+2;
    }
    if (is_menu_item(current_menu->items[i].type)){
      //jump down into the menu don't increase the address as these have no savable values
      int ret = eeprom_calc_addr((Menu*)(current_menu->items[i].data),address,target_menu,target_menu_item);
      if (ret == 0){
	//item was found return
	return ret;
      }
    }
  }
  //this is the failure point
  return -1;
}


void eeprom_update(struct Menu* menu, unsigned int menu_item){
  int addr = eeprom_calc_addr(menu,menu_item,eeprom_current_profile());
  if(addr != -1){
    eeprom_write_uint(addr, menu->items[menu_item].value);
  }
}



void eeprom_write_uint(int addr, unsigned int value){
  byte lowByte = ((value >> 0) & 0xFF);
  byte highByte = ((value >> 8) & 0xFF);  
  EEPROM.write(addr, lowByte);
  EEPROM.write(addr + 1, highByte);

}


unsigned int eeprom_read_uint(int addr){
  byte lowByte = EEPROM.read(addr);
  byte highByte = EEPROM.read(addr + 1);  
  return ((lowByte & 0xFF)<< 0)  | ((highByte &0xFF)<< 8)  ;

}


void eeprom_update_uint(int addr, unsigned int value){
  //read the value
  unsigned int current_value = eeprom_read_uint(addr);
  if (current_value != value){
    eeprom_write_uint(addr,value);
  }
}




void eeprom_save_profile(struct Menu * current_menu,int profile_num){
  for(unsigned int i = 0; i<current_menu->num_items;i++){
    if (is_value_item(current_menu->items[i].type)){
      //save the value
      int addr = eeprom_calc_addr(current_menu,i,profile_num);
      eeprom_update_uint(addr,current_menu->items[i].value);
    }
    if (is_menu_item(current_menu->items[i].type)){
      //jump down the menu
      eeprom_save_profile((Menu*)(current_menu->items[i].data),profile_num);
    }
  }
}



void eeprom_load_profile(struct Menu * current_menu,int profile_num){
  for(unsigned int i = 0; i<current_menu->num_items;i++){
    if (is_value_item(current_menu->items[i].type)){
      //save the value
      int addr = eeprom_calc_addr(current_menu,i,profile_num);
      current_menu->items[i].value = eeprom_read_uint(addr);
    }
    if (is_menu_item(current_menu->items[i].type)){
      //jump down the menu
      eeprom_load_profile((Menu*)(current_menu->items[i].data),profile_num);
    }
  }
}




int eeprom_calc_size(struct Menu * current_menu){
  int size = 0;
  for(unsigned int i = 0; i<current_menu->num_items;i++){
    if (is_value_item(current_menu->items[i].type)){
      size = size + 2; //uint takes 2 bytes
      //add one to the count
    }
    if (is_menu_item(current_menu->items[i].type)){
      //jump down and count items
      size = size + eeprom_calc_size((Menu*)(current_menu->items[i].data));
    }
  }
  return size;
}


unsigned int eeprom_current_profile(){
  return eeprom_current_profile_num;
}


void eeprom_clear(){
  for(unsigned int i = 0;i<EEPROMSIZE;i++){
    EEPROM.write(i,0);   
  }
}
