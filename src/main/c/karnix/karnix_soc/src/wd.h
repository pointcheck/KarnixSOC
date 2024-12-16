#ifndef _WD_H_
#define _WD_H_

#include <stdio.h>
#include "soc.h"

typedef struct
{
  volatile uint32_t ENABLE;
  volatile uint32_t COUNTDOWN;
  volatile uint32_t REBOOT;
} WD_Reg;


#endif /* _WD_H_ */

