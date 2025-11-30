#include "display.h"

Mutex display_mutex;
/*
  Displays 1 or more lines on the LCD screen.
  Lines past LCD_ROWS will be discarded.
 */
void dispay_lines(const char *lines[], int n_lines) {
  if (n_lines > LCD_ROWS) {
    n_lines = LCD_ROWS;
  }
  ScopedLock<Mutex> lock(display_mutex);
  clr_lcd();
  for (int l = 0; l < n_lines; l++) {
    set_cursor(l, 0);
    const char *string = lines[l];
    while (*string) {
      write_data(*string++);
      wait_us(40);
    }
  }
}
/*
  Formats a temperature value with degrees C indicator and a specified label
*/
void format_labelled_temp(const char *label, float temp, char *buf,
                           size_t buf_size) {
  int integer_part = int(temp);
  int decimal_part = int((temp - integer_part) * 10 + 0.5f);
  if (decimal_part < 0)
    decimal_part = -decimal_part;
  snprintf(buf, buf_size, "%s: %d.%d C", label, integer_part, decimal_part);
}