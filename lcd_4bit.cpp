/*A simple set of functions to write to 2x16 LCD,
operating in 4-bit mode.                                        */
#include "lcd_4bit.h"

DigitalOut CS(D10);
SPI ser_port(D11, D12, D13); // Initialise SPI, using default settings

// Manufacturer specified initialisation process
void init_lcd(void) { 
  thread_sleep_for(40);
  shift_out(0x30); // function set 8-bit
  wait_us(37);
  write_cmd(0x20); // function set
  wait_us(37);
  write_cmd(0x20); // function set
  wait_us(37);
  write_cmd(0x0C); // display ON/OFF
  wait_us(37);
  write_cmd(0x01); // display clear
  wait_us(1520);
  write_cmd(0x06); // entry-mode set
  wait_us(37);
  write_cmd(0x28); // function set
  wait_us(37);
}

// Writes 4 bit data and mode to the LCD
void write_4bit(int data, int mode) { // mode is RS line, cmd=0, data=1
  int hi_n;
  int lo_n;
  hi_n = (data & 0xF0); // form the two 4-bit nibbles that will be sent
  lo_n = ((data << 4) & 0xF0);
  // send each word twice, strobing the Enable line
  shift_out(hi_n | ENABLE | mode);
  wait_us(1);
  shift_out(hi_n & ~ENABLE);
  shift_out(lo_n | ENABLE | mode);
  wait_us(1);
  shift_out(lo_n & ~ENABLE);
}

// Sends word to SPI port
void shift_out(int data) { 
  CS = 0;
  ser_port.write(data);
  CS = 1;
}

// Configures LCD command word
void write_cmd(int cmd) { 
  write_4bit(cmd, COMMAND_MODE);
}
// Configures LCD data word
void write_data(char c) {   
  write_4bit(c, DATA_MODE);
}

// Clears the display and waits the required time
void clr_lcd(void) {
  write_cmd(0x01);
  wait_us(1520);
}

// Sets the cursor to the position indicated by `line` and `pos`
void set_cursor(int line, int pos) {
  int addr = (line == 0 ? 0x00 : 0x40) + pos;
  write_cmd(0x80 | addr);
  wait_us(40);
}