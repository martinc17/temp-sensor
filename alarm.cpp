#include "alarm.h"
AnalogIn volume(A0);
PwmOut buzzer(D9);

/*
Function for sounding the buzzer at the specified frequency
*/
void play_alarm(float frequency, int duration, int rest) {
    buzzer.period(1.0 / frequency);
    buzzer = volume;
    thread_sleep_for(duration);
    buzzer = 0.0;
    thread_sleep_for(rest);
}
