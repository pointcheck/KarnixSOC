#include "usb10.h"

int usb10_wait_cmd_complete(USB10_Reg* reg, int timeout) {

	int i;

	// Make sure command execution has begun
	for(i = 0; i < 100; i++)
	       if(reg->COMMAND & USB10_CMD_START_BIT)
		       break;

	// Wait for execution to complete
	for(i = 0; i < timeout; i++)
	       if(!(reg->STATUS & USB10_STATUS_BUSY_BIT))
			return 1;

	return 0;
}
