// SPDX-License-Identifier: GPL-2.0

/*
 * This code was derived from Linux kernel VTY.
 *
 * Adopted by Ruslan Zalata <rz@fabmicro.ru>
 *
 */

#include <stdio.h>
#include <string.h>
#include "keyboard.h"
#include "usb_hid_keys.h"
#include "usb_hid_keyboard.h"

#if(KEYBOARD_DEBUG)
	#define	kbd_printf(...)	{printf( __VA_ARGS__);}
#else
	#define	kbd_printf(...)	{ }
#endif

#define	KEYBOARD_CRLF	0

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define conv_uni_to_8bit(c) ((int) ((c) & 0xff))
#define conv_8bit_to_uni(c) ((uint32_t)(c))


/*
 * Handler Tables.
 */

#define K_HANDLERS\
	k_self,		k_fn,		k_spec,		k_pad,\
	k_dead,		k_cons,		k_cur,		k_shift,\
	k_meta,		k_ascii,	k_lock,		k_lowercase,\
	k_slock,	k_dead2,	k_brl,		k_ignore

typedef int (k_handler_fn)(struct kbd_data *kbd, unsigned char value,
			    char up_flag);
k_handler_fn K_HANDLERS;
k_handler_fn *k_handler[16] = { K_HANDLERS };

#define FN_HANDLERS\
	fn_null,	fn_enter,	fn_show_ptregs,	fn_show_mem,\
	fn_show_state,	fn_send_intr,	fn_lastcons,	fn_caps_toggle,\
	fn_num,		fn_hold,	fn_scroll_forw,	fn_scroll_back,\
	fn_boot_it,	fn_caps_on,	fn_compose,	fn_SAK,\
	fn_dec_console, fn_inc_console, fn_spawn_con,	fn_bare_num

typedef int (fn_handler_fn)(struct kbd_data *kbd);
fn_handler_fn FN_HANDLERS;
fn_handler_fn *fn_handler[] = { FN_HANDLERS };


/* maximum values each key_handler can handle */
const unsigned char max_vals[] = {
	[ KT_LATIN	] = 255,
	[ KT_FN		] = ARRAY_SIZE(func_table) - 1,
	[ KT_SPEC	] = ARRAY_SIZE(fn_handler) - 1,
	[ KT_PAD	] = NR_PAD - 1,
	[ KT_DEAD	] = NR_DEAD - 1,
	[ KT_CONS	] = 255,
	[ KT_CUR	] = 3,
	[ KT_SHIFT	] = NR_SHIFT - 1,
	[ KT_META	] = 255,
	[ KT_ASCII	] = NR_ASCII - 1,
	[ KT_LOCK	] = NR_LOCK - 1,
	[ KT_LETTER	] = 255,
	[ KT_SLOCK	] = NR_LOCK - 1,
	[ KT_DEAD2	] = 255,
	[ KT_BRL	] = NR_BRL - 1,
};

const int NR_TYPES = ARRAY_SIZE(max_vals);

char capslock_state = 0;
char numlock_state = 0;
char dead_key_next = 0;
char applic_state = 0;

/*
 * Helper Functions.
 */
int put_queue(struct kbd_data *kbd, int ch)
{
	if(kbd == NULL)
		return -2;

	if(!kbd->buffer || !kbd->len || !kbd->time)
		return -3;

	if(*(kbd->len) >= kbd->size)
		return -4; // buffer overflow ?

	kbd->buffer[(*(kbd->len))++] = ch;
	kbd->buffer[*(kbd->len)] = 0;

	return 1;
}

int puts_queue(struct kbd_data *kbd, const char *cp)
{
	if(kbd == NULL)
		return -2;

	if(!kbd->buffer || !kbd->len || !kbd->time)
		return -3;

	int len = strlen(cp);

	if(*(kbd->len) + len >= kbd->size)
		return -4; // buffer overflow ?

	memcpy((void*)(kbd->buffer + *(kbd->len)), (void*)cp, len);

	*(kbd->len) += len; 

	return len;
}

int applkey(struct kbd_data *kbd, int key, char mode)
{
	char buf[] = { 0x1b, 'O', 0x00, 0x00 };

	buf[1] = (mode ? 'O' : '[');
	buf[2] = key;
	return puts_queue(kbd, buf);
}


/*
 * Special function handlers
 */
int fn_enter(struct kbd_data *kbd)
{
	#if (KEYBOARD_CRLF)
		put_queue(kbd, '\r');
		return put_queue(kbd, '\n');
	#else
		return put_queue(kbd, '\r');
	#endif
}

int fn_caps_toggle(struct kbd_data *kbd)
{
	capslock_state ^= 1;	
	return 0;
}

int fn_caps_on(struct kbd_data *kbd)
{
	capslock_state = 1;	
	return 0;
}

int fn_show_ptregs(struct kbd_data *kbd)
{
	return 0;
}

int fn_hold(struct kbd_data *kbd)
{
	return 0;
}

int fn_num(struct kbd_data *kbd)
{
	if (applic_state)
		return applkey(kbd, 'P', 1);
	else
		return fn_bare_num(kbd);
}

/*
 * Bind this to Shift-NumLock if you work in application keypad mode
 * but want to be able to change the NumLock flag.
 * Bind this to NumLock if you prefer that the NumLock key always
 * changes the NumLock flag.
 */
int fn_bare_num(struct kbd_data *kbd)
{
	numlock_state ^= 1;
	return 0;
}

int fn_lastcons(struct kbd_data *kbd)
{
	return 0;
}

int fn_dec_console(struct kbd_data *kbd)
{
	// Switch to previous console
	return 0;
}

int fn_inc_console(struct kbd_data *kbd)
{
	// Switch to next console
	return 0;
}

int fn_send_intr(struct kbd_data *kbd)
{
	// Send TTY_BREAK to serial line
	return 0;
}

int fn_scroll_forw(struct kbd_data *kbd)
{
	return 0;
}

int fn_scroll_back(struct kbd_data *kbd)
{
	//scrollback(kbd);
	return 0;
}

int fn_show_mem(struct kbd_data *kbd)
{
	//show_mem(0, NULL);
	return 0;
}

int fn_show_state(struct kbd_data *kbd)
{
	//show_state();
	return 0;
}

int fn_boot_it(struct kbd_data *kbd)
{
	//ctrl_alt_del();
	return 0;
}

int fn_compose(struct kbd_data *kbd)
{
	//ctrl_alt_del();
	dead_key_next = 1;
	return 0;
}

int fn_spawn_con(struct kbd_data *kbd)
{
	return 0;
}

int fn_SAK(struct kbd_data *kbd)
{
	return 0;
}

int fn_null(struct kbd_data *kbd)
{
	//do_compute_shiftstate();
	return 0;
}

/*
 * Special key handlers
 */
int k_ignore(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	return 0;
}

int k_spec(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	kbd_printf("k_spec: 0x%02x\r\n", value);

	if (up_flag)
		return 0;

	if (value >= ARRAY_SIZE(fn_handler))
		return -1;

	int ret = fn_handler[value](kbd);

	if(kbd && kbd->k_spec)
		kbd->k_spec(kbd, value);

	return ret;
}

int k_lowercase(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	kbd_printf("k_lowercase: 0x%02x\r\n", value);

	if (up_flag)
		return 0;

	int ret = put_queue(kbd, value);

	if(kbd && kbd->k_lowercase)
		kbd->k_lowercase(kbd, value);

	return ret;
}

int k_unicode(struct kbd_data *kbd, unsigned int value, char up_flag)
{
	kbd_printf("k_unicode: 0x%02x\r\n", value);

	if (up_flag)
		return 0;

	if (dead_key_next) {
		dead_key_next = 0;
		//diacr = value;
		return 0;
	}

	int c = conv_uni_to_8bit(value);
	if (c != -1)
		return put_queue(kbd, c);

	return -1;
}

/*
 * Handle dead key. Note that we now may have several
 * dead keys modifying the same character. Very useful
 * for Vietnamese.
 */
int k_deadunicode(struct kbd_data *kbd, unsigned int value, char up_flag)
{
	return 0;
}

int k_self(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	int ret = k_unicode(kbd, conv_8bit_to_uni(value), up_flag);

	if(kbd && kbd->k_self)
		kbd->k_self(kbd, value);
		
	return ret;
}

int k_dead2(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	return k_deadunicode(kbd, value, up_flag);
}

/*
 * Obsolete - for backwards compatibility only
 */
int k_dead(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	return 0;
}

int k_cons(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	return 0;
}

int k_fn(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	kbd_printf("k_fn: 0x%02x\r\n", value);

	if (up_flag)
		return 0;

	int ret = 0;

	if ((unsigned)value < ARRAY_SIZE(func_table)) {

		if (func_table[value])
			ret = puts_queue(kbd, func_table[value]);

	} else
		kbd_printf("k_fn called with value=%d\n", value);

	if(kbd && kbd->k_fn)
		kbd->k_fn(kbd, value);

	return ret;
}

int k_cur(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	const char cur_chars[] = "BDCA";

	kbd_printf("k_cur: 0x%02x\r\n", value);

	if (up_flag)
		return 0;

	int ret = applkey(kbd, cur_chars[value], 0);

	if(kbd && kbd->k_cur)
		kbd->k_cur(kbd, value);

	return ret;
}

int k_pad(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	const char pad_chars[] = "0123456789+-*/\015,.?()#";
	const char app_map[] = "pqrstuvwxylSRQMnnmPQS";

	if (up_flag)
		return 0;


	/* kludge... shift forces cursor/number keys */
	if (applic_state && !capslock_state) {
		return applkey(kbd, app_map[value], 1);
	}

	if (!numlock_state) {

		switch (value) {
		case KVAL(K_PCOMMA):
		case KVAL(K_PDOT):
			return k_fn(kbd, KVAL(K_REMOVE), 0);
		case KVAL(K_P0):
			return k_fn(kbd, KVAL(K_INSERT), 0);
		case KVAL(K_P1):
			return k_fn(kbd, KVAL(K_SELECT), 0);
		case KVAL(K_P2):
			return k_cur(kbd, KVAL(K_DOWN), 0);
		case KVAL(K_P3):
			return k_fn(kbd, KVAL(K_PGDN), 0);
		case KVAL(K_P4):
			return k_cur(kbd, KVAL(K_LEFT), 0);
		case KVAL(K_P6):
			return k_cur(kbd, KVAL(K_RIGHT), 0);
		case KVAL(K_P7):
			return k_fn(kbd, KVAL(K_FIND), 0);
		case KVAL(K_P8):
			return k_cur(kbd, KVAL(K_UP), 0);
		case KVAL(K_P9):
			return k_fn(kbd, KVAL(K_PGUP), 0);
		case KVAL(K_P5):
			return applkey(kbd, 'G', applic_state);
		}
	}

	kbd_printf("k_pad: 0x%02x\r\n", value);

	int ret = put_queue(kbd, pad_chars[value]);

	if (value == KVAL(K_PENTER) && KEYBOARD_CRLF)
		ret += put_queue(kbd, '\n');

	if(kbd && kbd->k_pad)
		kbd->k_pad(kbd, value);

	return ret;
}

int k_shift(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	kbd_printf("k_shift: 0x%02X\r\n", value);

	if (up_flag)
		return 0;

	return put_queue(kbd, value);
}

int k_meta(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	kbd_printf("k_meta: 0x%02X\r\n", value);

	if (up_flag)
		return 0;

	int ret = put_queue(kbd, '\033');
	ret += put_queue(kbd, value);

	if(kbd && kbd->k_meta)
		kbd->k_meta(kbd, value);

	return ret;
}

int k_ascii(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	kbd_printf("k_ascii: 0x%02X\r\n", value);

	if (up_flag)
		return 0;

	return put_queue(kbd, value);
}

int k_lock(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	kbd_printf("k_lock\r\n");
	return 0;
}

int k_slock(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	kbd_printf("k_slock\r\n");
	return 0;
}

int k_brl(struct kbd_data *kbd, unsigned char value, char up_flag)
{
	kbd_printf("k_brl\r\n");
	return 0;
}

int kbd_keycode(struct kbd_data *kbd, unsigned int keycode, int shift_pressed, int ctrl_pressed, int alt_pressed, char up_flag)
{
	unsigned short *keymap;
	unsigned short keysym;
	unsigned char type;

	int shift_final = (capslock_state > 0) ^ (shift_pressed > 0);

	if(shift_final && ctrl_pressed)
		keymap = shift_ctrl_map;	
	else if(ctrl_pressed && alt_pressed)
		keymap = ctrl_alt_map;
	else if(ctrl_pressed)
		keymap = ctrl_map;
	else if(alt_pressed)
		keymap = alt_map;
	else if(shift_final)
		keymap = shift_map;
	else 
		keymap = plain_map;

	kbd_printf("kbd_keycode: keycode = %d, shift = %d, keymap = %p, up_flag = %d\r\n", keycode, shift_final, keymap, up_flag);

	if (keycode < NR_KEYS)
		keysym = keymap[keycode];
	else
		return -1;

	type = KTYP(keysym);

	kbd_printf("kbd_keycode: keysym = 0x%04X, type = 0x%02X\r\n", keysym, type);

	if (type < 0xf0)
		return k_unicode(kbd, keysym, up_flag);

	type -= 0xf0;

	return (*k_handler[type])(kbd, keysym & 0xff, up_flag);
}

int kbd_hid_keycode(struct kbd_data *kbd, uint8_t hid_response[8])
{
	static uint8_t prev_hid_response[8] = { 0 };

	if(hid_response[2] > KEY_NONE && hid_response[2] < KEY_A) {
		kbd_printf("kbd_hid_keycode: errornous keycode = 0x%02X\r\n", hid_response[2]);
		return -1;
	}

	int shift_pressed = hid_response[0] & (KEY_MOD_LSHIFT | KEY_MOD_RSHIFT);
	int ctrl_pressed = hid_response[0] & (KEY_MOD_LCTRL | KEY_MOD_RCTRL);
	int alt_pressed = hid_response[0] & (KEY_MOD_LALT | KEY_MOD_RALT | KEY_MOD_LMETA | KEY_MOD_RMETA);

	// Check for depressed keys

	for(int i = 2; i < 8; i++) {

		if(prev_hid_response[i] >= KEY_A) {
			int j;

			unsigned char key_event =
				hid_keycode_to_linux_event[prev_hid_response[i]];

			for(j = 2; j < 8; j++)
				if(hid_response[j] == prev_hid_response[i])
					break; // key is still pressed

			if(j == 8) // key depressed
				kbd_keycode(kbd, key_event, shift_pressed, ctrl_pressed, alt_pressed, 1);
				
		}
	}
	

	// Check for keys newly pressed

	for(int i = 2; i < 8; i++) {
		
		if(hid_response[i] >= KEY_A) {
			int j;

			unsigned char key_event =
				hid_keycode_to_linux_event[hid_response[i]];

			for(j = 2; j < 8; j++)
				if(hid_response[i] == prev_hid_response[j])
					break; // key is still pressed
				
			if(j == 8) // key newly pressed
				kbd_keycode(kbd, key_event, shift_pressed, ctrl_pressed, alt_pressed, 0);

		}

	}

	memcpy((void*) prev_hid_response, (void*) hid_response, 8);

	return 0;
}

