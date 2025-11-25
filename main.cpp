#include "lcd_4bit.h"
#include "mbed.h"

Mutex lcd_mutex;
I2C temp_sensor(I2C_SDA, I2C_SCL); // Configure I2C interface

const int temp_addr = 0x90; // I2C address of temperature sensor DS1631
char commands[] = {0x51, 0xAA};
char read_temp[2];
float c_temp;
float max_temp;
float min_temp;
char LCD_result[9];

Thread thread1; // Read temp and display on LCD
void temp_thread(void const *args) {
  while (1) {
    lcd_mutex.lock();
    // Write 0xAA to sensor to read the last converted temp
    temp_sensor.write(temp_addr, &commands[1], 1, false);
    // Read temp into the read_temp array
    temp_sensor.read(temp_addr, read_temp, 2);
    // Convert temp to Celsius. Shift MS byte right by 8 bits, OR in ls byte,
    // then divide all by 256, with float result.
    float c_temp = (float((read_temp[0] << 8) | read_temp[1]) / 256);
    // convert float to character string
    sprintf(LCD_result, "%d.%d", int(c_temp), int((c_temp - int(c_temp)) * 10));
    write_cmd(0x80); // set cursor to start of first line
    wait_us(40);
    for (char i = 0; i < 4; i++) { // write 4 characters from string
      write_data(LCD_result[i]);
    } // end of for()
    print_lcd(" degrees C");
    // Write 0x51 to the DS1631 (address 0x90) to start next temp conversion
    temp_sensor.write(temp_addr, &commands[0], 1, false);
    lcd_mutex.unlock();
    thread_sleep_for(1000);
  }
}

Thread thread2;
void ui_thread (void const *args) {

}

#if !MBED_TEST_MODE
// main() runs in its own thread in the OS
int main() {
  init_lcd();
  clr_lcd();

  thread1.start(callback(temp_thread, &temp_sensor));
  while (true) {
    __WFI();
  }
}

#endif