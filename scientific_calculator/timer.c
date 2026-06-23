#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

static volatile uint32_t timer1_millis;

ISR(TIMER1_COMPA_vect) {
    timer1_millis++;
}

void timer_init(void) {
    TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10); //CTC
    OCR1A = 249;
    TIMSK1 |= (1 << OCIE1A);
    sei();
}

uint32_t millis(void) {
    uint32_t m;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        m = timer1_millis;
    }
    return m;
}

void delay_ms(uint32_t ms) {
    uint32_t start = millis();
    while (millis() - start < ms);
}
