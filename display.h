/*
Header file of functions to display data on LCD
*/

#ifndef DISPLAY_H
#define DISPLAY_H
#include "lcd_4bit.h"
#define LCD_ROWS 4

//Function Prototypes
void dispay_lines(const char *lines[], int n_lines);
void format_temp_with_unit(float temp, char *buf, size_t buf_size);
#endif