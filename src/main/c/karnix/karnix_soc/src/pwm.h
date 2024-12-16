#ifndef PWM_H_
#define PWM_H_


typedef struct
{
  volatile uint32_t PULSE_WIDTH;
  volatile uint32_t PULSE_TOP;
  volatile uint32_t ENABLE;
} Pwm_Reg;


#endif /* PWM_H_ */


