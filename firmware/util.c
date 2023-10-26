#include "nixie.h"

void delay() {
    for (int i = 0; i < 10; i++) {
        asm volatile("nop");
    }
}
