#ifndef TIMERCTRL_H_
#define TIMERCTRL_H_

#include <stdint.h>


typedef struct
{
  volatile uint32_t ENABLE;
  volatile uint32_t PRESCALER;
  volatile uint32_t PRESCALER_COUNTER;
  volatile uint32_t VALUE;
  volatile uint32_t LIMIT;
} Timer_Reg;

static void timer_prescaler(Timer_Reg *reg, uint32_t prescaler){
	reg->ENABLE = 0;
	reg->PRESCALER = prescaler;
	reg->PRESCALER_COUNTER = 0;
}

static void timer_run(Timer_Reg *reg, uint32_t limit){
	reg->VALUE = 0;
	reg->LIMIT = limit;
	reg->ENABLE = 1;
}


#endif /* TIMERCTRL_H_ */
