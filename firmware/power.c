#include <avr/io.h>
#include <avr/interrupt.h>
#include <ctype.h>

#include "nixie.h"

/*
 * PWM output is PB1 (OC1A); active-low (MOSFET is on when output is 0).
 *
 * We'll use the ADC in a feedback loop (we want ~1.6V on ADC6) to tune this.
 */

#define PWM_TOP    0x0040
#define PWM_THRESH 0x0014

void init_power() {
    // Turn on pull-up so the MOSFET is off by default until we configure
    // the PWM. (There is also an external pull-up to be sure; we just don't
    // want to fight it.)
    PORTB |= 2;

    ICR1 = PWM_TOP; // TOP = 0x00ff
    OCR1A = PWM_THRESH;
    TCCR1A = 0x82; // non-inverting PWM mode; WGM11 bit of mode field
    TCCR1B = 0x19; // fast PWM mode (WGM1{3,2,1,0} = 1110), no prescaler

    // Set PB1 as an output.
    DDRB |= 2;

    // Turn on ADC.
    ADMUX = 0x46;  // AVcc (5V) reference; ADC6 input.
    ADCSRB = 0x00; // trigger: free-running mode.
    // ADEN (enable); ADSC (start conv); ADATE (auto trigger); ADIE (ints);
    // prescale=128.
    ADCSRA = 0xef;
}

#define VOLTAGE 160  // slightly miscalibrated ref --> this produces about 170V.
#define TARGET (uint16_t)((uint32_t)1024 * VOLTAGE / 500)
#define TARGET_HIST (TARGET + 2)

char state = 1;

ISR(ADC_vect) {
    uint8_t lo = ADCL;
    uint8_t hi = ADCH;
    uint16_t value = (((uint16_t)hi) << 8) | ((uint16_t)lo);
    if (state && value > TARGET_HIST) {
        OCR1A = PWM_TOP;  // turn off
        state = 0;
    } else if (!state && value < TARGET) {
        OCR1A = PWM_THRESH;
        state = 1;
    }
}
