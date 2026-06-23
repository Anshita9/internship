#include "keypad.h"
#include "defines.h"
#include "gpio.h"
#include "timer.h"
#include <avr/io.h>



void keypad_init(void) {
    // Set rows to output
    gpio_set_direction(&KEYPAD_ROW_DDR, KEYPAD_ROW_1, GPIO_DIR_OUTPUT);
    gpio_set_direction(&KEYPAD_ROW_DDR, KEYPAD_ROW_2, GPIO_DIR_OUTPUT);
    gpio_set_direction(&KEYPAD_ROW_DDR, KEYPAD_ROW_3, GPIO_DIR_OUTPUT);
    gpio_set_direction(&KEYPAD_ROW_DDR, KEYPAD_ROW_4, GPIO_DIR_OUTPUT);
    gpio_set_direction(&KEYPAD_ROW_DDR, KEYPAD_ROW_5, GPIO_DIR_OUTPUT);

    // Set cols to input with pull-up
    gpio_set_direction(&KEYPAD_COL_DDR, KEYPAD_COL_1, GPIO_DIR_INPUT);
    gpio_set_pullup(&KEYPAD_COL_PORT, KEYPAD_COL_1, 1);
    gpio_set_direction(&KEYPAD_COL_DDR, KEYPAD_COL_2, GPIO_DIR_INPUT);
    gpio_set_pullup(&KEYPAD_COL_PORT, KEYPAD_COL_2, 1);
    gpio_set_direction(&KEYPAD_COL_DDR, KEYPAD_COL_3, GPIO_DIR_INPUT);
    gpio_set_pullup(&KEYPAD_COL_PORT, KEYPAD_COL_3, 1);
    gpio_set_direction(&KEYPAD_COL_DDR, KEYPAD_COL_4, GPIO_DIR_INPUT);
    gpio_set_pullup(&KEYPAD_COL_PORT, KEYPAD_COL_4, 1);
    gpio_set_direction(&KEYPAD_COL_DDR, KEYPAD_COL_5, GPIO_DIR_INPUT);
    gpio_set_pullup(&KEYPAD_COL_PORT, KEYPAD_COL_5, 1);
}

int keypad_scan(void) {
    uint8_t row_pins[] = {KEYPAD_ROW_1, KEYPAD_ROW_2, KEYPAD_ROW_3, KEYPAD_ROW_4, KEYPAD_ROW_5};
    uint8_t col_pins[] = {KEYPAD_COL_1, KEYPAD_COL_2, KEYPAD_COL_3, KEYPAD_COL_4, KEYPAD_COL_5};

    for (int r = 0; r < KEYPAD_ROWS; r++) {
        gpio_write(&KEYPAD_ROW_PORT, row_pins[r], 0);

        for (int c = 0; c < KEYPAD_COLS; c++) {
            if (gpio_read(&KEYPAD_COL_PIN, col_pins[c]) == 0) {
                // debounce
                delay_ms(20);
                while (gpio_read(&KEYPAD_COL_PIN, col_pins[c]) == 0);
                delay_ms(20);
                
                gpio_write(&KEYPAD_ROW_PORT, row_pins[r], 1); //reset cuz exiting
                return (r * KEYPAD_COLS) + c;
            }
        }
        gpio_write(&KEYPAD_ROW_PORT, row_pins[r], 1);
    }
    return -1; // all open
}
