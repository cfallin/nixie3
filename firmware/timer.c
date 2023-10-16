#include <avr/io.h>
#include <avr/interrupt.h>

#include "nixie.h"

void enable_timer_interrupt() {
    TIMSK |= (1 << TOIE0);
    TCCR0 |= (1 << CS00);  // prescale by 1; overflow after 256 counts; 78125Hz interrupt at 20MHz xtal
}

ISR(TIMER0_OVF_vect) {
    do_timer_int();
}
