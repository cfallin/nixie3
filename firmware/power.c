#include <avr/io.h>
#include <avr/interrupt.h>
#include <ctype.h>

#include "nixie.h"

/*
 * PWM output is PB1 (OC1A); active-low (MOSFET is on when output is 0).
 *
 * For a 12V supply and 160V output, we want a duty cycle of ~13.3:1, or (13.3
 * / 14.3) = 93% on-time. Because of the inverting driver, that translates to
 * 7% high PWM. Let's try a comparator value of 7% * 0xffff = 4587.
 *
 * We'll use the ADC in a feedback loop (we want ~1.6V on ADC6) to tune this.
 */

void init_power() {
    // Turn on pull-up so the MOSFET is off by default until we configure
    // the PWM. (There is also an external pull-up to be sure; we just don't
    // want to fight it.)
    PORTB |= 2;

    ICR1 = 0x0040; // TOP = 0x00ff
    OCR1A = 0x0034;  // 90% duty cycle
    TCCR1A = 0x82; // non-inverting PWM mode; WGM11 bit of mode field
    TCCR1B = 0x19; // fast PWM mode (WGM1{3,2,1,0} = 1110), no prescaler

    // Set PB1 as an output.
    DDRB |= 2;

    // Turn on ADC.
    // TODO.
}

void update_power() {
    // TODO.
}
