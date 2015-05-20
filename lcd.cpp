#include "lcd.h"
#include "Arduino.h"
//These are what is currently on display

char current_row_0_string[] = "                ";
char current_row_1_string[] = "                ";


char new_row_0_string[] = "                ";
char new_row_1_string[] = "                ";

extern LiquidCrystal lcd;


void delta_update_display(const char* str,int row_num){
  char *new_row;

  switch(row_num){
  case 0:
    new_row = new_row_0_string;
    break;
  case 1:
    new_row = new_row_1_string;
    break;
  default:       
    new_row = new_row_1_string;
  }

  char my_str[] = "                ";  
  unsigned int len = strlen(str);
  if(len>sizeof(my_str)){
    len = sizeof(my_str);
  } 
  strncpy(my_str,str,len);
  strncpy(new_row,my_str,16);
}

void display_int_value(unsigned int my_value,const char* units,byte row){
  char str[16];
  sprintf(str, "%u", my_value);
  display_string_value(str,row);
}

void display_string_value(const char* my_value,int row){

  delta_update_display(my_value,row);

}

void render_display(){
  for(byte row_num = 0;row_num<2;row_num++){
       char *row;
       char *new_row;
      switch(row_num){
      case 0:
	row = current_row_0_string;
	new_row = new_row_0_string;
	break;
      case 1:
	  row = current_row_1_string;
	new_row = new_row_1_string;
	break;
      default:       
	  row = new_row_1_string;
	new_row = new_row_1_string;
      }
      
      for(unsigned int i= 0;i<16;i++){
	if(row[i] != new_row[i]){
	  lcd.setCursor(i,row_num); 
	  lcd.print(new_row[i]);
	  row[i] = new_row[i];
	}
      }
    }
}


