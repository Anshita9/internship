#ifndef DEFINES_H
#define DEFINES_H

#define F_CPU 16000000UL // 16 MHz clock speed

// Keypad configuration
#define KEYPAD_ROWS 5
#define KEYPAD_COLS 5

// LCD configuration (pin mapping based on original Arduino sketch)
// Arduino Pin -> AVR Pin
// RS -> PD2
// E  -> PD3
// D4 -> PD4
// D5 -> PD5
// D6 -> PD6
// D7 -> PD7
#define LCD_RS_PORT     PORTD
#define LCD_RS_PIN      PD2
#define LCD_E_PORT      PORTD
#define LCD_E_PIN       PD3
#define LCD_DATA_PORT   PORTD
#define LCD_D4_PIN      PD4
#define LCD_D5_PIN      PD5
#define LCD_D6_PIN      PD6
#define LCD_D7_PIN      PD7

// Keypad Pin Mapping
// Rows (Output)
// D8-D12 -> PB0-PB4
#define KEYPAD_ROW_PORT PORTB
#define KEYPAD_ROW_DDR  DDRB
#define KEYPAD_ROW_1    PB0
#define KEYPAD_ROW_2    PB1
#define KEYPAD_ROW_3    PB2
#define KEYPAD_ROW_4    PB3
#define KEYPAD_ROW_5    PB4

// Cols (Input)
// A0-A4 -> PC0-PC4
#define KEYPAD_COL_PORT PORTC
#define KEYPAD_COL_DDR  DDRC
#define KEYPAD_COL_PIN  PINC
#define KEYPAD_COL_1    PC0
#define KEYPAD_COL_2    PC1
#define KEYPAD_COL_3    PC2
#define KEYPAD_COL_4    PC3
#define KEYPAD_COL_5    PC4


#endif // DEFINES_H
