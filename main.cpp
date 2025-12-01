#include "alarm.h"
#include "display.h"
#include "globals.h"
#include "mbed.h"
#include "menu.h"

I2C temp_sensor(I2C_SDA, I2C_SCL); // Configure I2C interface
InterruptIn btn_menu(D2);
InterruptIn btn_up(D3);
InterruptIn btn_dwn(D4);
InterruptIn btn_slct(D5);

Mutex temp_mutex;
Ticker reset_max_min;

const int temp_addr = 0x90; // I2C address of temperature sensor DS1631
char commands[] = {0x51, 0xAA};
bool initialised = false;
char read_temp[2];
float c_temp;
float max_temp;
float min_temp;

Thread thread1;
// Read and store the temp
void temp_thread(void const *args) {
  while (1) {
    // Write 0xAA to sensor to read the last converted temp
    temp_sensor.write(temp_addr, &commands[1], 1, false);
    // Read temp into the read_temp array
    temp_sensor.read(temp_addr, read_temp, 2);
    // Convert temp to Celsius. Shift MS byte right by 8 bits, OR in ls byte,
    // then divide all by 256, with float result.
    {
      ScopedLock<Mutex> lock(temp_mutex);
      c_temp = (float((read_temp[0] << 8) | read_temp[1]) / 256);

      if (!initialised) {
        max_temp = min_temp = c_temp;
        initialised = true;
      } else {
        if (c_temp > max_temp) {
          max_temp = c_temp;
        }
        if (c_temp < min_temp) {
          min_temp = c_temp;
        }
      }
    }
    // convert float to character string
    // Write 0x51 to the DS1631 (address 0x90) to start next temp conversion
    temp_sensor.write(temp_addr, &commands[0], 1, false);
    thread_sleep_for(1000);
  }
}

Thread thread2;
// Handle the menu, display of temp and user interaction
void show_temps() {
  float tc_copy, tmx_copy, tmn_copy;
  char line0[20], line1[20], line2[20];
  {
    ScopedLock<Mutex> lock(temp_mutex);
    tc_copy = c_temp;
    tmx_copy = max_temp;
    tmn_copy = min_temp;
  }
  const char *lines[LCD_ROWS];
  format_labelled_temp("Cur", tc_copy, line0, sizeof(line0));
  lines[0] = line0;
  format_labelled_temp("Max", tmx_copy, line1, sizeof(line1));
  lines[1] = line1;
  format_labelled_temp("Min", tmn_copy, line2, sizeof(line2));
  lines[2] = line2;
  dispay_lines(lines, 3);
}
void ui_thread(void const *args) {
  while (1) {
    handle_menu_input();
    if (ui_mode == 0) {
      show_temps();
    } else {
      display_menu();
    }
    thread_sleep_for(500);
  }
}

Thread thread3;
// Compare temp to limits and trigger buzzer if exceeded
void alarm_thread(void const *args) {
  while (1) {
    float t_copy;
    {
      ScopedLock<Mutex> lock(temp_mutex);
      t_copy = c_temp;
    }
    {
      ScopedLock<Mutex> lock(limit_mutex);
      if (upper_limit_set && t_copy > upper_limit) {
        play_alarm(200.0, 500, 500);
      } else if (lower_limit_set && t_copy < lower_limit) {
        play_alarm(125.0, 250, 250);
      }
    }
    thread_sleep_for(1000);
  }
}

// Resets the min and max temps every hour
void reset_max_min_hourly() {
  ScopedLock<Mutex> lock(temp_mutex);
  max_temp = min_temp = c_temp;
}

#if !MBED_TEST_MODE
// main() runs in its own thread in the OS
int main() {
  init_lcd();
  clr_lcd();
  btn_menu.fall(&menu_button_isr);
  btn_up.fall(&up_isr);
  btn_dwn.fall(&down_isr);
  btn_slct.fall(&select_isr);
  reset_max_min.attach(&reset_max_min_hourly, 1h);

  thread1.start(callback(temp_thread, &temp_sensor));
  thread2.start(callback(ui_thread, (void const *)nullptr));
  thread3.start(callback(alarm_thread, (void const *)nullptr));
  while (true) {
    __WFI();
  }
}

#endif