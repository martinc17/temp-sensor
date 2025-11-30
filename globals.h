/*
header file for global variables shared between modules
*/

#ifndef GLOBALS_H
#define GLOBALS_H

#include "mbed.h"


extern int ui_mode;

extern float upper_limit;
extern float lower_limit;

extern bool upper_limit_set;
extern bool lower_limit_set;

extern Mutex limit_mutex;

#endif
