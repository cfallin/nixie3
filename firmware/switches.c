#include <avr/io.h>

#include "nixie.h"

void init_switches() {
    DDRC = 0;
    PORTC = 0x3f;
}

// Active-low (zero bit) for pressed button/flipped switch, as from
// hardware (switches are from ground to input pin, and input pin has
// pull-up resistor).
int switch_debounce0 = 0;
int switch_debounce1 = 0;
int switch_debounce2 = 0;
// Active-high (one bit) for pressed button/flipped switch.
int switches = 0;

void read_switches() {
    switch_debounce2 = switch_debounce1;
    switch_debounce1 = switch_debounce0;
    switch_debounce0 = PINC;
    switches = ~(switch_debounce2 | switch_debounce1 | switch_debounce0);
}
