#pragma once

/* util.c */
void delay();

/* serial.c */
void init_usart();

/* display.c */
void init_display();
void display_digits(int h1, int h2, int m1, int m2, int s1, int s2);

/* switches.c */
void init_switches();
void read_switches();
extern int switches;

/* timer.c */
void enable_timer_interrupt();

/* power.c */
void init_power();
void update_power();

/* main.c */
void do_timer_int();
void do_serial_time(int h, int m, int s);
