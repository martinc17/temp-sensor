#include "globals.h"

int ui_mode = 0;

float upper_limit = 15.0f;
float lower_limit = 15.0f;

bool upper_limit_set = false;
bool lower_limit_set = false;

Mutex limit_mutex;