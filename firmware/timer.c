#include <avr/io.h>
#include <avr/interrupt.h>

#include "nixie.h"

void enable_timer_interrupt() {
    TIMSK0 = (1 << TOIE0);
    OCR0A = 128;
    TCCR0A = (1 << WGM01);  // WGM01:WGM00 = 00b -- CTC mode
    TCCR0B = (1 << CS00);  // prescale by 1; overflow after 128 counts; 78125Hz interrupt at 10MHz xtal
}

ISR(TIMER0_OVF_vect) {
    do_timer_int();
}
