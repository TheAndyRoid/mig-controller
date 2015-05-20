#ifndef LCD_H
#define LCD_H


#include <Arduino.h>
#include <LiquidCrystal.h>

//These are what is currently on display

extern LiquidCrystal lcd;
void delta_update_display(const char* str,int row_num);
void display_int_value(unsigned int my_value,const char* units,byte row);
void display_string_value(const char* my_value,int row);

void render_display();

#endif
