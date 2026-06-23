#include "lcd.h"
#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>

static void lcd_pulse_enable(void);
static void lcd_write_4bits(uint8_t data);
static void lcd_write(uint8_t data, uint8_t rs);

#define LCD_RS_DDR  DDRD
#define LCD_E_DDR   DDRD
#define LCD_D_DDR   DDRD

#define LCD_RS_PORT PORTD
#define LCD_E_PORT  PORTD
#define LCD_D_PORT  PORTD

#define LCD_RS_PIN  PD2
#define LCD_E_PIN   PD3
#define LCD_D4_PIN  PD4
#define LCD_D5_PIN  PD5
#define LCD_D6_PIN  PD6
#define LCD_D7_PIN  PD7

void lcd_init(void) {
    // init
    LCD_RS_DDR |= (1 << LCD_RS_PIN);
    LCD_E_DDR  |= (1 << LCD_E_PIN);
    LCD_D_DDR  |= (1 << LCD_D4_PIN) | (1 << LCD_D5_PIN) | (1 << LCD_D6_PIN) | (1 << LCD_D7_PIN);

    // powering on
    _delay_ms(50);

    // software reset sequence (as per HD44780 datasheet)
    lcd_write_4bits(0x30); // Send 0011
    _delay_ms(5);
    lcd_write_4bits(0x30); // Send 0011
    _delay_us(150);
    lcd_write_4bits(0x30); // Send 0011
    _delay_us(150);
    lcd_write_4bits(0x20); // Send 0010 (sets 4-bit mode)
    _delay_us(150);

    // 4. Set Function: 4-bit, 2-line, 5x8 font
    lcd_send_command(0x28);

    // 5. Display ON, Cursor OFF, Blink OFF
    lcd_send_command(0x0C);

    // 6. Clear Display
    lcd_clear();

    // 7. Set Entry Mode: Increment cursor, no shift
    lcd_send_command(0x06);
}

void lcd_send_command(uint8_t cmd) {
    lcd_write(cmd, 0); // rs = 0 for command
}

void lcd_send_data(uint8_t data) {
    lcd_write(data, 1); // rs = 1 for data
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_send_data(*str++);
    }
}

void lcd_clear(void) {
    lcd_send_command(0x01);
    _delay_ms(2); // clear display command takes longer
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x80 : 0xC0;
    address += col;
    lcd_send_command(address);
}


// --- Private Functions ---

static void lcd_pulse_enable(void) {
    LCD_E_PORT |= (1 << LCD_E_PIN);
    _delay_us(1); // Enable pulse must be >450ns
    LCD_E_PORT &= ~(1 << LCD_E_PIN);
    _delay_us(50); // Commands need >37us to settle
}

static void lcd_write_4bits(uint8_t data) {
    // data is a 4-bit value in the high nibble, e.g., 0x30
    LCD_D_PORT = (LCD_D_PORT & 0x0F) | (data & 0xF0);
    lcd_pulse_enable();
}

static void lcd_write(uint8_t data, uint8_t rs) {
    if (rs) {
        LCD_RS_PORT |= (1 << LCD_RS_PIN); // Set RS for data
    } else {
        LCD_RS_PORT &= ~(1 << LCD_RS_PIN); // Clear RS for command
    }

    // Send high nibble
    lcd_write_4bits(data & 0xF0);
    // Send low nibble
    lcd_write_4bits((data << 4) & 0xF0);
}
