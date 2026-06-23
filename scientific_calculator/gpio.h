#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>

typedef enum {
    GPIO_DIR_INPUT,
    GPIO_DIR_OUTPUT
} gpio_direction_t;

void gpio_set_direction(volatile uint8_t *port_ddr, uint8_t pin, gpio_direction_t dir);
void gpio_write(volatile uint8_t *port, uint8_t pin, uint8_t value);
uint8_t gpio_read(volatile uint8_t *pin_reg, uint8_t pin);
void gpio_set_pullup(volatile uint8_t *port, uint8_t pin, uint8_t enable);

#endif // GPIO_H
