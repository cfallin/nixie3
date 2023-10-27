#include <avr/io.h>
#include <avr/interrupt.h>

#include "nixie.h"

void enable_timer_interrupt() {
    TCCR0A = (1 << WGM01);  // WGM01:WGM00 = 10b -- CTC mode
    TCCR0B = (1 << CS00);  // prescale by 1; overflow after 128 counts; 78125Hz interrupt at 10MHz xtal
    OCR0A = 128;
    TCNT0 = 0;
    TIMSK0 |= (1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect) {
    do_timer_int();
}
