#include <avr/io.h>
#include <avr/interrupt.h>
#include <ctype.h>

#include "nixie.h"

void init_usart() {
    DDRD &= ~5; // RXD (PD0) and RTS' (PD2) in
    DDRD |= 2;  // TXD out

    UBRR0L = 129;  // baud divisor 129 --> 9600 baud
    UBRR0H = 0x00;
    UCSR0A = 0x02;  // double-speed (divisor of 8)
    UCSR0B = 0x90;  // RX interrupt enable, RX enable
    UCSR0C = 0x06;  // 8N1
}

unsigned char serial_buf[6];
int serial_buf_pos = 0;
int serial_valid = 0;

static void serial_line(const char* line, int length) {
    if (length < 6) {
        return;
    }
    for (int i = 0; i < 6; i++) {
        if (!isdigit(line[i])) {
            return;
        }
    }

    int h = (line[0] - '0') * 10 + (line[1] - '0');
    int m = (line[2] - '0') * 10 + (line[3] - '0');
    int s = (line[4] - '0') * 10 + (line[5] - '0');

    do_serial_time(h, m, s);
}

static void serial_receive(char c) {
    if (c == '\r') {
        return;
    }
    if (c == '\n') {
        if (serial_valid) {
            serial_line(serial_buf, serial_buf_pos);
        }
        serial_buf_pos = 0;
        serial_valid = 1;
        return;
    }
    if (serial_buf_pos >= 6) {
        return;
    }
    serial_buf[serial_buf_pos++] = c;
}

ISR(USART_RX_vect) {
    uint8_t status = UCSR0A;
    if (status & 0x10) {
        return;
    }
    if (!(status & 0x80)) {
        return;
    }
    char c = UDR0;
    serial_receive(c);
}
