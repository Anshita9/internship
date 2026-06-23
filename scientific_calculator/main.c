#include "defines.h"
#include "timer.h"
#include "lcd.h"
#include "keypad.h"
#include "pars.h"
#include "func.h" // Include func.h for math functions
#include <string.h>
#include <stdio.h>

#define MAX_EXPRESSION_LEN 16

static char expression_buffer[MAX_EXPRESSION_LEN];
static int buffer_index = 0;
static int current_mode = 1;

const char* keypad_mode_1[KEYPAD_ROWS][KEYPAD_COLS] = {
  {"0", "1", "2", "3", "4"},
  {"5", "6", "7", "8", "9"},
  {"+", "-", "*", "/", "sin("},
  {"cos(", "tan(", "exp(", "ln(", "Clear"},
  {"Backspace", ".", "=", "Mode shift", "3.14159"}
};

const char* keypad_mode_2[KEYPAD_ROWS][KEYPAD_COLS] = {
  {"0", "1", "2", "3", "4"},
  {"5", "6", "7", "8", "9"},
  {"(", ")", "^", "fact(", "asin("},
  {"acos(", "atan(", "mod(", "log10(", "Clear"},
  {"Backspace", ".", "=", "Mode shift", "3.14159"}
};

void handle_key_press(int key_index) {
    if (key_index < 0) return;

    int row = key_index / KEYPAD_COLS;
    int col = key_index % KEYPAD_COLS;

    const char* key_str = (current_mode == 1) ? keypad_mode_1[row][col] : keypad_mode_2[row][col];

    if (strcmp(key_str, "Clear") == 0) {
        buffer_index = 0;
        expression_buffer[0] = '\0';
        lcd_clear();
        lcd_print("Cleared");
        delay_ms(500);
        lcd_clear();
    } else if (strcmp(key_str, "Backspace") == 0) {
        if (buffer_index > 0) {
            // Must handle multi-char tokens. TODO
            expression_buffer[--buffer_index] = '\0';
            lcd_clear();
            lcd_print(expression_buffer);
        }
    } else if (strcmp(key_str, "Mode shift") == 0) {
        current_mode = (current_mode == 1) ? 2 : 1;
        lcd_clear();
        lcd_print(current_mode == 1 ? "Mode 1" : "Mode 2");
        delay_ms(500);
        lcd_clear();
        lcd_print(expression_buffer); // reprint
    } else if (strcmp(key_str, "=") == 0) {
        if (buffer_index > 0) {
            char result[32];
            compute(expression_buffer, 0, 0, 0, result);
            
            lcd_clear();
            lcd_set_cursor(0,0);
            lcd_print(expression_buffer);
            lcd_set_cursor(1, 0);
            lcd_print("=");
            lcd_print(result);
            
            // next
            buffer_index = 0;
            expression_buffer[0] = '\0';
        }
    } else {
        if (buffer_index + strlen(key_str) < MAX_EXPRESSION_LEN) {
            strcat(expression_buffer, key_str);
            buffer_index += strlen(key_str);
            lcd_clear();
            lcd_print(expression_buffer);
        } else {
            lcd_clear();
            lcd_print("Buffer Full!");
            delay_ms(1000);
            lcd_clear();
            lcd_print(expression_buffer);
        }
    }
}

int main(void) {
    timer_init();
    lcd_init();
    keypad_init();
    math_init();

    lcd_print("Calculator");
    delay_ms(1000);
    lcd_clear();

    while (1) {
        int key = keypad_scan();
        handle_key_press(key);
    }

    return 0;
}
