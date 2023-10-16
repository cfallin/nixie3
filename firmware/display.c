#include <avr/io.h>

#include "nixie.h"

#define PC_DATA (1 << 4)
#define PC_CLK (1 << 5)
#define PD_LATCH (1 << 3)

void init_display() {
    PORTC = PORTC & ~(PC_DATA | PC_CLK);
    PORTD = PORTD & ~PD_LATCH;
    DDRC = DDRC | PC_DATA | PC_CLK;
    DDRD = DDRD | PD_LATCH;
}

static inline void set_data(int bit) {
    PORTC = (PORTC & ~PC_DATA) | (bit ? PC_DATA : 0);
}
static inline void set_clk(int bit) {
    PORTC = (PORTC & ~PC_CLK) | (bit ? PC_CLK : 0);
}
static inline void set_latch(int bit) {
    PORTD = (PORTD & ~PD_LATCH) | (bit ? PD_LATCH : 0);
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
        int bit = (d & 8);
        set_data(bit);
        delay();
        set_clk(1);
        delay();
        set_clk(0);
        d <<= 1;
    }
    delay();
    set_data(0);
}

void latch_display() {
    set_latch(1);
    delay();
    delay();
    set_latch(0);
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
