#include "menu.h"
#include "display.h"
#include "globals.h"
#include <cstdio>

int menu_select = 0;
int set_temp = 0;

float temp_lower;
float temp_upper;

/*
ISR for responding to menu button.
Will enter or exit menu state.
*/
void menu_button_isr() {
  if (ui_mode == 0) {
    ui_mode = 1;
    menu_select = 0;
  } else {
    ui_mode = 0;
  }
}

/*
  ISR for responding to up button.
  Will decrement menu select in menu state.
  Will increment temp limit in set temp state.
*/
void up_isr() {
  if (ui_mode == 1 && set_temp == 0) {
    menu_select--;
    if (menu_select < 0) {
      menu_select = 0;
    }
  }
  if (set_temp == 1) {
    temp_upper += 0.5f;
  }
  if (set_temp == 2) {
    temp_lower += 0.5f;
  }
}

/*
  ISR for responding to down button.
  Will increment menu select in menu state.
  Will decrement temp limit in set temp state.
*/
void down_isr() {
  if (ui_mode == 1 && set_temp == 0) {
    menu_select++;
    if (menu_select > 2) {
      menu_select = 0;
    }
  }
  if (set_temp == 1) {
    upper_limit -= 0.5f;
  }
  if (set_temp == 2) {
    lower_limit -= 0.5f;
  }
}
/*
  ISR for responding to the select button.
  Will confirm menu selection in menu state.
  Will save temp limit in set temp state.
*/
void select_isr() {
  if (ui_mode == 1 && set_temp == 0) {
    handle_menu_choice(menu_select);
  } else if (set_temp == 1) {
      ScopedLock<Mutex> lock(limit_mutex);
      upper_limit = temp_upper;
  } else if (set_temp == 2) {
      ScopedLock<Mutex> lock(limit_mutex);
      lower_limit = temp_lower;
  }
}

/*
Handles the menu choice of the user
*/
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

void display_menu() {
  const char *lines[LCD_ROWS];
  char buff[20];
  if (set_temp == 0) {
    lines[0] = (menu_select == 0) ? "> Set upper limit" : "  Set upper limit";
    lines[1] = (menu_select == 1) ? "> Set lower limit" : "  Set lower limit";
    lines[2] = (menu_select == 2) ? "> Disable limits" : "  Disable limits";
    dispay_lines(lines, 3);
  } else if (set_temp == 1) {
    sprintf(buff, " %.1f C", temp_upper);
    lines[0] = "Upper limit:";
    lines[1] = buff;
    lines[2] = "Adjust using up/down";
    lines[3] = "Press select to save";
  } else if (set_temp == 2) {
    sprintf(buff, " %.1f C", temp_lower);
    lines[0] = "Lower limit:";
    lines[1] = buff;
    lines[2] = "Adjust using up/down";
    lines[3] = "Press select to save";
  }
}