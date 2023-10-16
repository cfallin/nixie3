#include "nixie.h"

void delay() {
    for (int i = 0; i < 1000; i++) {
        asm volatile("nop");
    }
}
