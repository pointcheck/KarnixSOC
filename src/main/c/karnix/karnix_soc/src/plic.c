#include "plic.h"

void plic_print_stats(void) {
        // Do not use printf() here because it's not re-entarable
        char str[128];
        sprintf(str, "PLIC: ENABLE = %p, PENDING = %p, POLARITY = %p, IRQLINE = %p, IRQLAST = %p\r\n", PLIC->ENABLE, PLIC->PENDING, PLIC->POLARITY, PLIC->IRQLINE, PLIC->IRQLAST);
        print(str);
}

