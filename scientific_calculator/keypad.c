#include "keypad.h"
#include "defines.h"
#include "gpio.h"
#include "timer.h"
#include <avr/io.h>

// ---------------- INIT ----------------

void keypad_init(void) {
    // 1. Set Rows as output
    gpio_set_direction(&KEYPAD_ROW_DDR, KEYPAD_ROW_1, GPIO_DIR_OUTPUT);
    gpio_set_direction(&KEYPAD_ROW_DDR, KEYPAD_ROW_2, GPIO_DIR_OUTPUT);
    gpio_set_direction(&KEYPAD_ROW_DDR, KEYPAD_ROW_3, GPIO_DIR_OUTPUT);
    gpio_set_direction(&KEYPAD_ROW_DDR, KEYPAD_ROW_4, GPIO_DIR_OUTPUT);
    gpio_set_direction(&KEYPAD_ROW_DDR, KEYPAD_ROW_5, GPIO_DIR_OUTPUT);

    // FIX: Drive all rows HIGH initially so they start in an inactive state
    gpio_write(&KEYPAD_ROW_PORT, KEYPAD_ROW_1, 1);
    gpio_write(&KEYPAD_ROW_PORT, KEYPAD_ROW_2, 1);
    gpio_write(&KEYPAD_ROW_PORT, KEYPAD_ROW_3, 1);
    gpio_write(&KEYPAD_ROW_PORT, KEYPAD_ROW_4, 1);
    gpio_write(&KEYPAD_ROW_PORT, KEYPAD_ROW_5, 1);

    // 2. Columns as input pull-up
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

// ---------------- SCAN ----------------

int keypad_scan(void) {

    // OPTIMIZATION: Made 'static const' so they reside permanently in flash 
    // memory instead of being reallocated on the stack every function call.
    static const uint8_t row_pins[] = {
        KEYPAD_ROW_1,
        KEYPAD_ROW_2,
        KEYPAD_ROW_3,
        KEYPAD_ROW_4,
        KEYPAD_ROW_5
    };

    static const uint8_t col_pins[] = {
        KEYPAD_COL_1,
        KEYPAD_COL_2,
        KEYPAD_COL_3,
        KEYPAD_COL_4,
        KEYPAD_COL_5
    };

    for (int r = 0; r < KEYPAD_ROWS; r++) {

        // Activate specific row by driving it LOW
        gpio_write(&KEYPAD_ROW_PORT, row_pins[r], 0);

        for (int c = 0; c < KEYPAD_COLS; c++) {

            // Check if column is pulled LOW by a button press
            if (gpio_read(&KEYPAD_COL_PIN, col_pins[c]) == 0) {

                delay_ms(20); // Debounce press

                // Wait until the user releases the key (blocking)
                while (gpio_read(&KEYPAD_COL_PIN, col_pins[c]) == 0);

                delay_ms(20); // Debounce release

                // Reset the row back to HIGH before returning
                gpio_write(&KEYPAD_ROW_PORT, row_pins[r], 1);

                return (r * KEYPAD_COLS) + c;
            }
        }

        // Deactivate row before moving to the next one
        gpio_write(&KEYPAD_ROW_PORT, row_pins[r], 1);
    }

    return -1; // No key pressed
}
