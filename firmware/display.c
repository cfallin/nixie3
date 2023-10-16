#include <avr/io.h>

#include "nixie.h"

void init_display() {
    PORTB = 0;
    DDRB = 7;
}

void output_digit(int d) {
    if (d == 0) {
        d = 1;
    } else if (d == 1) {
        d = 0;
    } else if (d == 0xff) {
        d = 15;
    } else {
        d = 11 - d;
    }
    for (int i = 0; i < 4; i++) {
        PORTB = (d & 8) ? 2 : 0;
        delay();
        PORTB |= 1;
        delay();
        d <<= 1;
    }
    PORTB = 0;
}

void latch_display() {
    PORTB = 4;
    delay();
    delay();
    PORTB = 0;
    delay();
    delay();
}

void display_digits(int h1, int h2, int m1, int m2, int s1, int s2) {
    output_digit(h2);
    output_digit(h1);
    output_digit(m2);
    output_digit(m1);
    output_digit(s2);
    output_digit(s1);
    latch_display();
}
