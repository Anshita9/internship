#include "defines.h"
#include "timer.h"
#include "lcd.h"
#include "keypad.h"
#include "pars.h"
#include "func.h"
#include <string.h>
#include <stdio.h>

#define MAX_EXPRESSION_LEN 64   // FIXED

static char expression_buffer[MAX_EXPRESSION_LEN];
static int buffer_index = 0;
static int current_mode = 1;

const char* keypad_mode_1[KEYPAD_ROWS][KEYPAD_COLS] = {
  {"0", "1", "2", "3", "4"},
  {"5", "6", "7", "8", "9"},
  {"+", "-", "*", "/", "sin"},
  {"cos", "tan", "exp", "ln", "Clear"},
  {"Backspace", ".", "=", "Mode", "pi"}
};

const char* keypad_mode_2[KEYPAD_ROWS][KEYPAD_COLS] = {
  {"0", "1", "2", "3", "4"},
  {"5", "6", "7", "8", "9"},
  {"(", ")", "^", "fact", "asin"},
  {"acos", "atan", "mod", "log10", "Clear"},
  {"Backspace", ".", "=", "Mode", "pi"}
};

static void append_char(const char* str) {
    int len = strlen(str);
    if (buffer_index + len >= MAX_EXPRESSION_LEN - 1) return;

    strcpy(&expression_buffer[buffer_index], str);
    buffer_index += len;
    expression_buffer[buffer_index] = '\0';
}

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
        return;
    }

    if (strcmp(key_str, "Backspace") == 0) {
        if (buffer_index > 0) {
            buffer_index--;
            expression_buffer[buffer_index] = '\0';
            lcd_clear();
            lcd_print(expression_buffer);
        }
        return;
    }

    if (strcmp(key_str, "Mode") == 0) {
        current_mode = (current_mode == 1) ? 2 : 1;
        lcd_clear();
        lcd_print("Mode Changed");
        delay_ms(500);
        lcd_clear();
        return;
    }

    if (strcmp(key_str, "=") == 0) {
        char result[32];
        compute(expression_buffer, 0, 0, 0, result);

        lcd_clear();
        lcd_set_cursor(0,0);
        lcd_print(expression_buffer);
        lcd_set_cursor(1,0);
        lcd_print(result);

        buffer_index = 0;
        expression_buffer[0] = '\0';
        return;
    }

    append_char(key_str);
    lcd_clear();
    lcd_print(expression_buffer);
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
}
