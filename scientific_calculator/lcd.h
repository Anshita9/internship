#ifndef LCD_H
#define LCD_H

#include <stdint.h>

void lcd_init(void);
void lcd_send_command(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_print(const char *str);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);

#endif // LCD_H
