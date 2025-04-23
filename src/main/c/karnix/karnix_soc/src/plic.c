#include "plic.h"
#include "utils.h"

void plic_print_stats(void) {
	char str[128];
	sprintf(str, "PLIC:\tENABLE = %p, PENDING = %p, POLARITY = %p, IRQLINE = %p, IRQLAST = %p\r\n",
		PLIC->ENABLE, PLIC->PENDING, PLIC->POLARITY, PLIC->IRQLINE, PLIC->IRQLAST);
	xprintf(str);
}

