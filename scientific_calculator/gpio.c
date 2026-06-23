#include "gpio.h"

void gpio_set_direction(volatile uint8_t *port_ddr, uint8_t pin, gpio_direction_t dir) {
    if (dir == GPIO_DIR_OUTPUT) {
        *port_ddr |= (1 << pin);
    } else {
        *port_ddr &= ~(1 << pin);
    }
}

void gpio_write(volatile uint8_t *port, uint8_t pin, uint8_t value) {
    if (value) {
        *port |= (1 << pin);
    } else {
        *port &= ~(1 << pin);
    }
}

uint8_t gpio_read(volatile uint8_t *pin_reg, uint8_t pin) {
    return (*pin_reg & (1 << pin)) ? 1 : 0;
}

void gpio_set_pullup(volatile uint8_t *port, uint8_t pin, uint8_t enable) {
    if (enable) {
        *port |= (1 << pin);
    } else {
        *port &= ~(1 << pin);
    }
}
