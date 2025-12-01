#include "menu.h"
#include "display.h"
#include "globals.h"
#include <cstdio>

int menu_select = 0;
int set_temp = 0;

float temp_lower;
float temp_upper;

volatile bool btn_menu_flag = false;
volatile bool btn_up_flag = false;
volatile bool btn_down_flag = false;
volatile bool btn_select_flag = false;

void menu_button_isr() { btn_menu_flag = true; }
void up_isr() { btn_up_flag = true; }
void down_isr() { btn_down_flag = true; }
void select_isr() { btn_select_flag = true; }


void handle_menu_choice(int select) {

  if (select == 0) {
    ScopedLock<Mutex> lock(limit_mutex);
    temp_upper = upper_limit;
    set_temp = 1;
  } else if (select == 1) {
    ScopedLock<Mutex> lock(limit_mutex);
    temp_lower = lower_limit;
    set_temp = 2;
  } else if (select == 2) {
    ScopedLock<Mutex> lock(limit_mutex);
    upper_limit_set = false;
    lower_limit_set = false;
  }
}

void handle_menu_input() {
    if (btn_menu_flag) {
        btn_menu_flag = false;
        if (ui_mode == 0) { ui_mode = 1; menu_select = 0; }
        else { ui_mode = 0; set_temp = 0; }
    }

    if (btn_up_flag) {
        btn_up_flag = false;
        if (ui_mode == 1 && set_temp == 0) menu_select = (menu_select > 0) ? menu_select-1 : 2;
        else if (set_temp == 1) temp_upper += 0.5f;
        else if (set_temp == 2) temp_lower += 0.5f;
    }

    if (btn_down_flag) {
        btn_down_flag = false;
        if (ui_mode == 1 && set_temp == 0) menu_select = (menu_select < 2) ? menu_select+1 : 0;
        else if (set_temp == 1) temp_upper -= 0.5f;
        else if (set_temp == 2) temp_lower -= 0.5f;
    }

    if (btn_select_flag) {
        btn_select_flag = false;
        ScopedLock<Mutex> lock(limit_mutex);
        if (ui_mode == 1 && set_temp == 0) handle_menu_choice(menu_select);
        else if (set_temp == 1) { upper_limit = temp_upper; upper_limit_set = true; set_temp = 0; }
        else if (set_temp == 2) { lower_limit = temp_lower; lower_limit_set = true; set_temp = 0; }
    }
}

void display_menu() {
  const char *lines[LCD_ROWS];
  char buff[20];
  if (set_temp == 0) {
    lines[0] = (menu_select == 0) ? "> Set upper limit" : "  Set upper limit";
    lines[1] = (menu_select == 1) ? "> Set lower limit" : "  Set lower limit";
    lines[2] = (menu_select == 2) ? "> Disable limits" : "  Disable limits";
    dispay_lines(lines, 3);
  } else if (set_temp == 1) {
    format_labelled_temp(" ", temp_upper, buff, sizeof(buff));
    lines[0] = "Upper limit:";
    lines[1] = buff;
    lines[2] = "Adjust using up/down";
    lines[3] = "Press select to save";
    dispay_lines(lines, 4);
  } else if (set_temp == 2) {
    format_labelled_temp(" ", temp_lower, buff, sizeof(buff));
    lines[0] = "Lower limit:";
    lines[1] = buff;
    lines[2] = "Adjust using up/down";
    lines[3] = "Press select to save";
    dispay_lines(lines, 4);
  }
}