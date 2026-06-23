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

    // 1. HANDLE CLEAR
    if (strcmp(key_str, "Clear") == 0) {
        buffer_index = 0;
        expression_buffer[0] = '\0';
        lcd_clear();
        lcd_print("Cleared");
        delay_ms(500);
        lcd_clear();
        return;
    }

    // 2. FIX: SMART BACKSPACE (Handles multi-character words like "sin", "log10")
    if (strcmp(key_str, "Backspace") == 0) {
        if (buffer_index > 0) {
            // Check all potential multi-char tokens from both modes to see if our buffer ends with one
            int token_removed = 0;
            
            // Loop through both keypad maps to check if a token matches the end of our current buffer string
            for (int r = 0; r < KEYPAD_ROWS; r++) {
                for (int c = 0; c < KEYPAD_COLS; c++) {
                    const char* tok1 = keypad_mode_1[r][c];
                    const char* tok2 = keypad_mode_2[r][c];
                    
                    // Skip functional system command strings
                    if (strcmp(tok1, "Clear") == 0 || strcmp(tok1, "Backspace") == 0 || strcmp(tok1, "Mode") == 0 || strcmp(tok1, "=") == 0) continue;

                    int len1 = strlen(tok1);
                    if (buffer_index >= len1 && strcmp(&expression_buffer[buffer_index - len1], tok1) == 0) {
                        buffer_index -= len1;
                        token_removed = 1;
                        break;
                    }
                    
                    int len2 = strlen(tok2);
                    if (buffer_index >= len2 && strcmp(&expression_buffer[buffer_index - len2], tok2) == 0) {
                        buffer_index -= len2;
                        token_removed = 1;
                        break;
                    }
                }
                if (token_removed) break;
            }

            // Fallback: If no macro string matched, delete exactly 1 single standard character
            if (!token_removed) {
                buffer_index--;
            }

            expression_buffer[buffer_index] = '\0';
            lcd_clear();
            
            // If the string is not completely empty, show remaining expression
            if (buffer_index > 0) {
                lcd_print(expression_buffer);
            }
        }
        return;
    }

    // 3. HANDLE MODE CHANGE
    if (strcmp(key_str, "Mode") == 0) {
        current_mode = (current_mode == 1) ? 2 : 1;
        lcd_clear();
        lcd_print("Mode Changed");
        delay_ms(500);
        lcd_clear();
        // Redisplay current working expression after the notification fades
        if (buffer_index > 0) {
            lcd_print(expression_buffer);
        }
        return;
    }

    // 4. HANDLE EVALUATION
    if (strcmp(key_str, "=") == 0) {
        char result[32];
        compute(expression_buffer, 0, 0, 0, result);

        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print(expression_buffer);
        lcd_set_cursor(1, 0);
        lcd_print(result);

        buffer_index = 0;
        expression_buffer[0] = '\0';
        return;
    }

    // 5. HANDLE CHARACTER APPEND
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
        // Only trigger evaluation if a valid key coordinate is pressed
        if (key >= 0) {
            handle_key_press(key);
        }
    }
}
