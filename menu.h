/*
Header file for functions controlling the menu
*/

#ifndef MENU_H
#define MENU_H

// Function prototypes
void menu_button_isr();
void up_isr();
void down_isr();
void select_isr();
void handle_menu_input();
void handle_menu_choice(int select);
void display_menu();

#endif