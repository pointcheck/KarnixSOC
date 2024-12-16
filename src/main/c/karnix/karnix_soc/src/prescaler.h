#ifndef PRESCALERCTRL_H_
#define PRESCALERCTRL_H_

#include <stdint.h>


typedef struct
{
  volatile uint32_t LIMIT;
} Prescaler_Reg;

static void prescaler_init(Prescaler_Reg* reg, uint32_t limit){
	reg->LIMIT = limit;

}

#endif /* PRESCALERCTRL_H_ */
