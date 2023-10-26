/*
 * Nixie Clock v3.0
 *
 * Copyright (c) 2023 Chris Fallin <chris@cfallin.org>
 * Placed under the Apache-2.0 license.
 */

/* 
 * Assume an ATmega328.
 *
 * IO connections (manufactured board version):
 *
 * PC5 is shift register clock (active high to shift).
 * PC4 is shift register data.
 * PD3 is shift register latch (active high to latch).
 *
 * Shift register output is nixie 2, 1; 4, 3; 6, 5. MSB first of BCD digits.
 *
 * PD0 is RXD from FT232 chip.
 * PD1 is TXD to FT232 chip.
 * PD2 is RTS' from FT232 chip.
 *
 * PB1 (OC1A) is out to high-voltage boost supply MOSFET driver (active low,
 * inverting driver).
 *
 * ADC6 is in from high-voltage line voltage divider (1/101 of voltage, via
 * 1M+10k divider).
 *
 * PC0 is J5 switch to ground.
 * PC1 is J6 switch to ground.
 * PC2 is J7 switch to ground.
 * PC3 is J8 switch to ground.
 *
 * Clock is 10MHz crystal, no divider.
 */

#include <avr/interrupt.h>

#include "nixie.h"

#define EXPECTED_TIMER_TICKS_PER_SEC 78125 // 20MHz / 256

long timer_ticks = 0;
long hz = EXPECTED_TIMER_TICKS_PER_SEC;
uint8_t time_h = 0, time_m = 0, time_s = 0;

#define DISPLAY_TIME 0
#define DISPLAY_SET 1
#define DISPLAY_CAL 2
int display_mode = 0;

#define UI_TIME 0
#define UI_TO_SET 1
#define UI_SET 2
#define UI_SET_NEXT 3
#define UI_SET_PLUS 4
#define UI_SET_MINUS 5

#define UI_TO_SET_CAL 6
#define UI_SET_CAL 7
#define UI_SET_CAL_NEXT 8
#define UI_SET_CAL_PLUS 9
#define UI_SET_CAL_MINUS 10
#define UI_TO_TIME 11

int ui_state = 0;
int ui_set_digit = 0;

void do_ui() {

    switch (ui_state) {
    case UI_TIME:
        display_mode = DISPLAY_TIME;
        if (switches & 8) {
            ui_state = UI_TO_SET;
        }
        break;
    case UI_TO_SET:
        display_mode = DISPLAY_TIME;
        if (!(switches & 8)) {
            ui_state = UI_SET;
            ui_set_digit = 0;
        }
        break;
    case UI_SET:
        display_mode = DISPLAY_SET;
        if (switches & 8) {
            ui_state = UI_TO_SET_CAL;
        } else if (switches & 4) {
            ui_set_digit++;
            if (ui_set_digit == 3) {
                ui_set_digit = 0;
            }
            ui_state = UI_SET_NEXT;
        } else if (switches & 2) {
            switch (ui_set_digit) {
                case 0:
                    time_h++;
                    if (time_h == 24) {
                        time_h = 0;
                    }
                    break;
                case 1:
                    time_m++;
                    if (time_m == 60) {
                        time_m = 0;
                    }
                    break;
                case 2:
                    time_s++;
                    if (time_s == 60) {
                        time_s = 0;
                    }
                    break;
            }
            ui_state = UI_SET_PLUS;
        } else if (switches & 1) {
            switch (ui_set_digit) {
                case 0:
                    if (time_h == 0) {
                        time_h = 24;
                    }
                    time_h--;
                    break;
                case 1:
                    if (time_m == 0) {
                        time_m = 60;
                    }
                    time_m--;
                    break;
                case 2:
                    if (time_s == 0) {
                        time_s = 60;
                    }
                    time_s--;
                    break;
            }
            ui_state = UI_SET_MINUS;
        }
        break;
    case UI_SET_NEXT:
        display_mode = DISPLAY_SET;
        if (!(switches & 4)) {
            ui_state = UI_SET;
        }
        break;
    case UI_SET_PLUS:
        display_mode = DISPLAY_SET;
        if (switches & 1) {
            switch (ui_set_digit) {
                case 0:
                    time_h = 0;
                    break;
                case 1:
                    time_m = 0;
                    break;
                case 2:
                    time_s = 0;
                    break;
            }
        } else if (!(switches & 2)) {
            ui_state = UI_SET;
        }
        break;
    case UI_SET_MINUS:
        display_mode = DISPLAY_SET;
        if (switches & 2) {
            switch (ui_set_digit) {
                case 0:
                    time_h = 0;
                    break;
                case 1:
                    time_m = 0;
                    break;
                case 2:
                    time_s = 0;
                    break;
            }
        } else if (!(switches & 1)) {
            ui_state = UI_SET;
        }
        break;
    case UI_TO_SET_CAL:
        display_mode = DISPLAY_SET;
        if (!(switches & 8)) {
            ui_state = UI_SET_CAL;
            ui_set_digit = 0;
        }
        break;
    case UI_SET_CAL:
        display_mode = DISPLAY_CAL;
        if (switches & 8) {
            ui_state = UI_TO_TIME;
        } else if (switches & 4) {
            ui_set_digit++;
            if (ui_set_digit == 5) {
                ui_set_digit = 0;
            }
            ui_state = UI_SET_CAL_NEXT;
        } else if (switches & 2) {
            int amt = 1;
            for (int i = 0; i < (4 - ui_set_digit); i++) {
                amt *= 10;
            }
            hz += amt;
            if (hz > 100000) {
                hz -= 100000;
            }
            ui_state = UI_SET_CAL_PLUS;
        } else if (switches & 1) {
            int amt = 1;
            for (int i = 0; i < (4 - ui_set_digit); i++) {
                amt *= 10;
            }
            hz -= amt;
            if (hz < amt) {
                hz += 100000;
            }
            ui_state = UI_SET_CAL_MINUS;
        }
        break;
    case UI_SET_CAL_NEXT:
        display_mode = DISPLAY_CAL;
        if (!(switches & 4)) {
            ui_state = UI_SET_CAL;
        }
        break;
    case UI_SET_CAL_PLUS:
        display_mode = DISPLAY_CAL;
        if (!(switches & 2)) {
            ui_state = UI_SET_CAL;
        }
        break;
    case UI_SET_CAL_MINUS:
        display_mode = DISPLAY_CAL;
        if (!(switches & 1)) {
            ui_state = UI_SET_CAL;
        }
        break;
    case UI_TO_TIME:
        display_mode = DISPLAY_CAL;
        if (!(switches & 8)) {
            ui_state = UI_TIME;
        }
        break;
    }
}

void update_display() {
    switch (display_mode) {
    case DISPLAY_TIME: {
        display_digits(time_h / 10, time_h % 10, time_m / 10, time_m % 10, time_s / 10, time_s % 10);
        break;
    }
    case DISPLAY_SET: {
        int second_half_second = timer_ticks > (hz / 2);
        int h1 = 15, h2 = 15, m1 = 15, m2 = 15, s1 = 15, s2 = 15;

        if (ui_set_digit != 0 || !second_half_second) {
            h1 = time_h / 10;
            h2 = time_h % 10;
        }
        if (ui_set_digit != 1 || !second_half_second) {
            m1 = time_m / 10;
            m2 = time_m % 10;
        }
        if (ui_set_digit != 2 || !second_half_second) {
            s1 = time_s / 10;
            s2 = time_s % 10;
        }
        display_digits(h1, h2, m1, m2, s1, s2);
        break;
    }
    case DISPLAY_CAL: {
        int second_half_second = timer_ticks > (hz / 2);
        int d1 = 15, d2 = 15, d3 = 15, d4 = 15, d5 = 15;

        if (ui_set_digit != 0 || !second_half_second) {
            d1 = (hz / 10000) % 10;
        }
        if (ui_set_digit != 1 || !second_half_second) {
            d2 = (hz / 1000) % 10;
        }
        if (ui_set_digit != 2 || !second_half_second) {
            d3 = (hz / 100) % 10;
        }
        if (ui_set_digit != 3 || !second_half_second) {
            d4 = (hz / 10) % 10;
        }
        if (ui_set_digit != 4 || !second_half_second) {
            d5 = hz % 10;
        }
        display_digits(15, d1, d2, d3, d4, d5);
    }
    }
}

int main()
{
    init_display();
    init_usart();
    init_switches();
    init_power();
    enable_timer_interrupt();
    sei();

    int pos = 0;
    int state = 0;
    while (1) {
        delay();
        read_switches();
        do_ui();
        update_display();
    }
}

void do_second() {
    time_s++;
    if (time_s == 60) {
        time_s = 0;
        time_m++;
    }
    if (time_m == 60) {
        time_m = 0;
        time_h++;
    }
    if (time_h == 24) {
        time_h = 0;
    }
}

void do_timer_int() {
    timer_ticks++;
    if (timer_ticks >= hz) {
        timer_ticks = 0;
        do_second();
    }
}

void do_serial_time(int h, int m, int s) {
    time_h = h;
    time_m = m;
    time_s = s;
    timer_ticks = 0;
}
