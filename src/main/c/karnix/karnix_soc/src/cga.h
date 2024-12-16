#ifndef __CGA_H__
#define __CGA_H__

#include <stdint.h>
#include <string.h>

extern const char font_6x8[];
extern const char font_12x16[];

#define CGA_VIDEO_WIDTH		320
#define	CGA_VIDEO_HEIGHT	240
#define CGA_TEXT_WIDTH		80	
#define	CGA_TEXT_HEIGHT		30	
#define	CGA_TEXT_HEIGHT_TOTAL	64
#define	CGA_FRAMEBUFFER_SIZE	(CGA_VIDEO_WIDTH*CGA_VIDEO_HEIGHT*2/8)
#define	CGA_OR_FONT		0x08
#define	CGA_OR_BG		0x80

#define	CGA_CTRL_VIDEO_EN		(1 << 31)	
#define	CGA_CTRL_BLANKING_EN		(1 << 30)	
#define	CGA_CTRL_VIDEO_MODE_SHIFT	24
#define	CGA_CTRL_VIDEO_MODE		(3 << CGA_CTRL_VIDEO_MODE_SHIFT)	
#define	CGA_CTRL_HSYNC_FLAG		(1 << 21)	
#define	CGA_CTRL_VSYNC_FLAG		(1 << 20)	
#define	CGA_CTRL_VBLANK_FLAG		(1 << 19)	
#define	CGA_CTRL_V_SCROLL_DIR		(1 << 10)	
#define	CGA_CTRL_V_SCROLL_SHIFT		0
#define	CGA_CTRL_V_SCROLL		(0x03ff << CGA_CTRL_V_SCROLL_SHIFT)

#define	CGA_CTRL2_CURSOR_X_SHIFT	0
#define	CGA_CTRL2_CURSOR_X		(0xff << CGA_CTRL2_CURSOR_X_SHIFT)
#define	CGA_CTRL2_CURSOR_Y_SHIFT	8
#define	CGA_CTRL2_CURSOR_Y		(0xff << CGA_CTRL2_CURSOR_Y_SHIFT)
#define	CGA_CTRL2_CURSOR_BOTTOM_SHIFT	16
#define	CGA_CTRL2_CURSOR_BOTTOM		(0x0f << CGA_CTRL2_CURSOR_BOTTOM_SHIFT)
#define	CGA_CTRL2_CURSOR_TOP_SHIFT	20
#define	CGA_CTRL2_CURSOR_TOP		(0x0f << CGA_CTRL2_CURSOR_TOP_SHIFT)
#define	CGA_CTRL2_CURSOR_BLINK_SHIFT	24
#define	CGA_CTRL2_CURSOR_BLINK		(0x07 << CGA_CTRL2_CURSOR_BLINK_SHIFT)
#define	CGA_CTRL2_CURSOR_BLINK_EN_SHIFT	27
#define	CGA_CTRL2_CURSOR_BLINK_EN	(1 << CGA_CTRL2_CURSOR_BLINK_EN_SHIFT)
#define	CGA_CTRL2_CURSOR_COLOR_SHIFT	28
#define	CGA_CTRL2_CURSOR_COLOR		(0x0f << CGA_CTRL2_CURSOR_COLOR_SHIFT)

#define	CGA_MODE_TEXT		0
#define	CGA_MODE_GRAPHICS1	1

#pragma pack(1)
typedef struct
{
  uint8_t FB[CGA_FRAMEBUFFER_SIZE];			// Framebuffer
  uint8_t unused1[48*1024-CGA_FRAMEBUFFER_SIZE];	//  
  volatile uint32_t PALETTE[16];				// offset 48K
  volatile uint32_t CTRL;				// 48K + 64 
  volatile uint32_t CTRL2;				// 48K + 128 
  uint8_t unused2[12200];				// 
  uint8_t CHARGEN[4096];				// offset 60K
} CGA_Reg;
#pragma pack(0)

#define CGA             ((CGA_Reg*)(0xF0040000))

int cga_ram_test(int interations);

void cga_video_print(int x, int y, int colors, char *text, int text_size, const char *font,
		int font_width, int font_height);

void cga_video_print_char(volatile uint8_t* fb, char c, int x, int y, char colors, int frame_width,
		int frame_height, const char *font, int font_width, int font_height);

void cga_bitblit(uint8_t *src_img, uint8_t *dst_img, int x, int y, int src_width, int src_height,
		int dst_width, int dst_height);
void cga_rotate_palette_left(uint32_t palettes_to_rotate);
void cga_fill_screen(char color);
void cga_draw_pixel(int x, int y, int color);
void cga_draw_line(int x1, int y1, int x2, int y2, int color);
void cga_text_print(uint8_t *framebuffer, int x, int y, int fg_color, int bg_color, char *text);
void cga_set_scroll(int scrl);
void cga_text_scroll_up(int scroll_delay);
void cga_text_scroll_down(int scroll_delay);
void cga_set_cursor_xy(int x, int y);
void cga_set_cursor_style(int top, int bottom);

static inline int cga_get_cursor_x(void) {
        return (CGA->CTRL2 >> CGA_CTRL2_CURSOR_X_SHIFT) & 0xff;
}

static inline int cga_get_cursor_y(void) {
        return (CGA->CTRL2 >> CGA_CTRL2_CURSOR_Y_SHIFT) & 0xff;
}

static inline void cga_wait_vblank(void) {
        while(!(CGA->CTRL & CGA_CTRL_VBLANK_FLAG));
}

static inline void cga_wait_vblank_end(void) {
        while(CGA->CTRL & CGA_CTRL_VBLANK_FLAG);
}

static inline void cga_set_palette(uint32_t c[16]) {
	memcpy((void *)CGA->PALETTE, c, 16 * 4);
}

static inline void cga_set_video_mode(int mode) {
	CGA->CTRL &= ~CGA_CTRL_VIDEO_MODE;
	CGA->CTRL |= (mode << CGA_CTRL_VIDEO_MODE_SHIFT) & CGA_CTRL_VIDEO_MODE;
	//printf("cga_set_video_mode: mode = %d, ctrl = %p\r\n", mode, CGA->CTRL);
}

#define	ESC_UP		"\033U"	// Move cursor upward 1 line
#define	ESC_DOWN	"\033D" // Move cursor downward 1 line (same as \n)
#define	ESC_LEFT	"\033L" // Move cursor to the left 1 char
#define	ESC_RIGTH	"\033R" // Move cursor to the right 1 char
#define	ESC_FG		"\033F" // Set foreground color
#define	ESC_BG		"\033B" // Set background color

#endif /* __CGA_H__ */


