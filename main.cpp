#include "alarm.h"
#include "display.h"
#include "mbed.h"

I2C temp_sensor(I2C_SDA, I2C_SCL); // Configure I2C interface

Mutex temp_mutex;

const int temp_addr = 0x90; // I2C address of temperature sensor DS1631
char commands[] = {0x51, 0xAA};
char read_temp[2];
float c_temp;
float max_temp;
float min_temp;
bool upper_limit_set;
bool lower_limit_set;
float upper_limit;
float lower_limit;
char buff[10];

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
    }
    // convert float to character string
    // Write 0x51 to the DS1631 (address 0x90) to start next temp conversion
    temp_sensor.write(temp_addr, &commands[0], 1, false);
    thread_sleep_for(1000);
  }
}

Thread thread2;
// Handle the menu, display of temp and user interaction
void ui_thread(void const *args) {
  while (1) {
    float tc_copy;
    {
      ScopedLock<Mutex> lock(temp_mutex);
      tc_copy = c_temp;
    }
    const char *lines[LCD_ROWS];
    format_temp_with_unit(tc_copy, buff, sizeof(buff));
    lines[0] = buff;
    dispay_lines(lines, 1);
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
    if (upper_limit_set && t_copy > upper_limit) {
      play_alarm(200.0, 500, 500);
    } else if (lower_limit_set && t_copy < lower_limit) {
      play_alarm(125.0, 250, 250);
    }
    thread_sleep_for(1000);
  }
}

#if !MBED_TEST_MODE
// main() runs in its own thread in the OS
int main() {
  init_lcd();
  clr_lcd();

  thread1.start(callback(temp_thread, &temp_sensor));
  thread2.start(callback(ui_thread, (void const *)nullptr));
  thread3.start(callback(alarm_thread, (void const *)nullptr));
  while (true) {
    __WFI();
  }
}

#endif