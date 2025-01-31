#ifndef __USB_H__
#define __USB_H__

#include <stdint.h>
#include <string.h>

#define	USB_TYPE_NONE		0
#define	USB_TYPE_KEYBOARD	1
#define	USB_TYPE_MOUSE		2
#define	USB_TYPE_GAMEPAD	3

#define	USB_STATUS_CONERR_S	31
#define	USB_STATUS_CONERR_M	(0x01 << USB_STATUS_CONERR_S)
#define	USB_STATUS_CONERR(X)	(((X) & USB_STATUS_CONERR_M) >> USB_STATUS_CONERR_S)
#define	USB_STATUS_REPORT_S	30
#define	USB_STATUS_REPORT_M	(0x01 << USB_STATUS_REPORT_S)		
#define	USB_STATUS_REPORT(X)	(((X) & USB_STATUS_REPORT_M) >> USB_STATUS_REPORT_S)
#define	USB_STATUS_TYPE_S	28
#define	USB_STATUS_TYPE_M	(0x03 << USB_STATUS_TYPE_S)
#define	USB_STATUS_TYPE(X)	(((X) & USB_STATUS_TYPE_M) >> USB_STATUS_TYPE_S)
#define	USB_STATUS_RESET_S	0	// Software reset - active low	
#define	USB_STATUS_RESET_M	(0x01 << USB_STATUS_RESET_S)
#define	USB_STATUS_RESET(X)	(((X) & USB_STATUS_RESET_M) >> USB_STATUS_RESET_S)

#define	USB_KEYBOARD_KEY1_S	0
#define	USB_KEYBOARD_KEY1_M	(0xff << USB_KEYBOARD_KEY1_S)
#define	USB_KEYBOARD_KEY1(X)	(((X) & USB_KEYBOARD_KEY1_M) >> USB_KEYBOARD_KEY1_S)
#define	USB_KEYBOARD_KEY2_S	8
#define	USB_KEYBOARD_KEY2_M	(0xff << USB_KEYBOARD_KEY2_S)
#define	USB_KEYBOARD_KEY2(X)	(((X) & USB_KEYBOARD_KEY2_M) >> USB_KEYBOARD_KEY2_S)
#define	USB_KEYBOARD_KEY3_S	16
#define	USB_KEYBOARD_KEY3_M	(0xff << USB_KEYBOARD_KEY3_S)
#define	USB_KEYBOARD_KEY3(X)	(((X) & USB_KEYBOARD_KEY3_M) >> USB_KEYBOARD_KEY3_S)
#define	USB_KEYBOARD_KEY4_S	24	
#define	USB_KEYBOARD_KEY4_M	(0xff << USB_KEYBOARD_KEY4_S)
#define	USB_KEYBOARD_KEY4(X)	(((X) & USB_KEYBOARD_KEY4_M) >> USB_KEYBOARD_KEY4_S)

#define	USB_KEYMOD_S		0
#define	USB_KEYMOD_M		(0xff << USB_KEYMOD_S)
#define	USB_KEYMOD(X)		(((X) & USB_KEYMOD_M) >> USB_KEYMOD_S)

#define	USB_MOUSE_BUTTONS_S	0
#define	USB_MOUSE_BUTTONS_M	(0xff << USB_MOUSE_BUTTONS_S)
#define USB_MOUSE_BUTTONS(X)	(((X) & USB_MOUSE_BUTTONS_M) >> USB_MOUSE_BUTTONS_S)
#define	USB_MOUSE_DX_S		8
#define	USB_MOUSE_DX_M		(0xff << USB_MOUSE_DX_S)
#define USB_MOUSE_DX(X)		(char)(((X) & USB_MOUSE_DX_M) >> USB_MOUSE_DX_S)
#define	USB_MOUSE_DY_S		16	
#define	USB_MOUSE_DY_M		(0xff << USB_MOUSE_DY_S)
#define USB_MOUSE_DY(X)		(char)(((X) & USB_MOUSE_DY_M) >> USB_MOUSE_DY_S)

#define	USB_GAMEPAD_L_S		0
#define	USB_GAMEPAD_L_M		(0x01 << USB_GAMEPAD_L_S)
#define	USB_GAMEPAD_R_S		1
#define	USB_GAMEPAD_R_M		(0x01 << USB_GAMEPAD_R_S)
#define	USB_GAMEPAD_U_S		2
#define	USB_GAMEPAD_U_M		(0x01 << USB_GAMEPAD_U_S)
#define	USB_GAMEPAD_D_S		3
#define	USB_GAMEPAD_D_M		(0x01 << USB_GAMEPAD_D_S)
#define	USB_GAMEPAD_A_S		4
#define	USB_GAMEPAD_A_M		(0x01 << USB_GAMEPAD_A_S)
#define	USB_GAMEPAD_B_S		5
#define	USB_GAMEPAD_B_M		(0x01 << USB_GAMEPAD_B_S)
#define	USB_GAMEPAD_X_S		6
#define	USB_GAMEPAD_X_M		(0x01 << USB_GAMEPAD_X_S)
#define	USB_GAMEPAD_Y_S		7
#define	USB_GAMEPAD_Y_M		(0x01 << USB_GAMEPAD_Y_S)
#define	USB_GAMEPAD_SEL_S	8
#define	USB_GAMEPAD_SEL_M	(0x01 << USB_GAMEPAD_SEL_S)
#define	USB_GAMEPAD_STA_S	9
#define	USB_GAMEPAD_STA_M	(0x01 << USB_GAMEPAD_STA_S)

#pragma pack(1)
typedef struct
{
  volatile uint32_t STATUS;
  volatile uint32_t KEYBOARD;
  volatile uint32_t KEYMOD;
  volatile uint32_t MOUSE;
  volatile uint32_t GAMEPAD;
  volatile uint32_t DBGLOW;
  volatile uint32_t DBGHIGH;
} USB_Reg;
#pragma pack(0)

#define USB             ((USB_Reg*)(0xF00D0000))

static inline int usbGetType(USB_Reg* reg) {
	return USB_STATUS_TYPE(reg->STATUS);
}

#endif /* __USB_H__ */


