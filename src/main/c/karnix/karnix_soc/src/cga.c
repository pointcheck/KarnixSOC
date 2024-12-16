#include "cga.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>

void cga_rotate_palette_left(uint32_t palettes_to_rotate) {

	cga_wait_vblank();

	for(int i = 0; i < 4; i++) {
		if(palettes_to_rotate & (1 << i)) {
			for(int component = 0; component < 3; component++) {
				uint32_t mask = 0xff << (component << 3);
				uint32_t tmp = CGA->PALETTE[i] & ~mask;
				uint32_t tmp2 = (CGA->PALETTE[i] >> (component << 3)) & 0xff;
				tmp2 = tmp2 << 1;
				tmp2 |= tmp2 & 0x100 ? 1 : 0; 
				CGA->PALETTE[i] = tmp | ((tmp2 & mask) << (component << 3)); 
			}
		}
	}
}

void cga_video_print(int x, int y, int colors, char *text, int text_size, const char *font, int font_width, int font_height) {

	for(int i = 0; i < text_size && text[i]; i++)
		cga_video_print_char(CGA->FB, text[i], x + i * font_width, y, colors, CGA_VIDEO_WIDTH, CGA_VIDEO_HEIGHT, font, font_width, font_height);

}


void cga_video_print_char(volatile uint8_t* fb, char c, int x, int y, char colors, int frame_width, int frame_height, const char *font, int font_width, int font_height)
{
	uint32_t* ffb = (uint32_t*)fb;

	int my_y;

	int lines = font_height / 8;
	int font_color = colors & 0x3;
	int bg_color = (colors >> 4) & 0x3;


	if(font_height % 8) 
		lines++;

	//printf("cga_video_print_char() fb = %p, frame_width = %d, frame_height = %d, x = %d, y = %d, c = %c\r\n", fb, frame_width, frame_height, x, y, c);

	for(int line = 0; line < lines; line++) {

		int line_height = MIN(8, (font_height - line * 8));
		int my_x = x;

		for(int col = 0; col < font_width; col++) {

			if(my_x >= 0 && my_x < frame_width) {

				uint32_t bit_mask = 0x01;
				uint32_t f;
			
				f = font[font_width * c * lines + line * font_width + col];

				my_y = y + line * 8;

				for(int bit = 0; bit < line_height; bit++) {

					if(my_y >= 0 && my_y < frame_height) {

						uint32_t pixel_idx = my_y * frame_width + my_x; 
						uint32_t word_idx = pixel_idx / 16;
						uint32_t mask = 0x03 << ((15 - (pixel_idx % 16)) * 2);
						uint32_t tmp = ffb[word_idx];

						if((f & bit_mask)) {
							if(!(colors & 0x08)) // 3-th bit is OR flag for font
								tmp &= ~mask;

							tmp |= (font_color << ((15 - (pixel_idx % 16)) * 2));
						} else {
							if(!(colors & 0x80)) // 7-th bit is OR flag for background 
								tmp &= ~mask;

							tmp |= (bg_color << ((15 - (pixel_idx % 16)) * 2));
						}

						ffb[word_idx] = tmp; 
					}

					bit_mask = bit_mask << 1;
					my_y++;
				}
			}

  		my_x++;
		}
	}
}

int cga_ram_test(int interations) {
	volatile unsigned int *mem;
	unsigned int fill;
	int fails = 0;

	for(int i = 0; i < interations; i++) {
		fill = 0xdeadbeef + i;
		mem = (unsigned int*) CGA->FB;

		printf("Filling video RAM at: %p, size: %d bytes...\r\n", mem, CGA_FRAMEBUFFER_SIZE);

		while((unsigned int)mem < (unsigned int)(CGA->FB + CGA_FRAMEBUFFER_SIZE)) {
			*mem++ = fill;
			fill += 0xdeadbeef; // generate pseudo-random data
		}

		fill = 0xdeadbeef + i;
		mem = (unsigned int*) CGA->FB;

		printf("Checking video RAM at: %p, size: %d bytes...\r\n", mem, CGA_FRAMEBUFFER_SIZE);

		while((unsigned int)mem < (unsigned int)(CGA->FB + CGA_FRAMEBUFFER_SIZE)) {
			unsigned int tmp = *mem;
			if(tmp != fill) {
				printf("Video RAM check failed at: %p, expected: %p, got: %p\r\n", mem, fill, tmp);
				fails++;
			} else {
				//printf("\r\nMem check OK     at: %p, expected: %p, got: %p\r\n", mem, fill, *mem);
			}
			mem++;
			fill += 0xdeadbeef; // generate pseudo-random data
			break;
		}

		if((unsigned int)mem == (unsigned int)(CGA->FB + CGA_FRAMEBUFFER_SIZE))
			printf("SRAM Fails: %d\r\n", fails);

		if(fails)
			break;
	}

	return fails++;
}

/*
 * cga_bitblit - copy image from src memory area to dst memory area.
 *
 * dst mem area is treaded as videoframebuffer of size CGA_VIDEO_WIDTH x CGA_VIDEO_HEIGHT pixels.
 * Each pixel in CGA graph mode is two bit color index. Pixel bits are packed left to right from
 * high to low bits of each byte.
 *
 */
void cga_bitblit(uint8_t *src_img, uint8_t *dst_img, int x, int y, int src_width, int src_height,
		int dst_width, int dst_height) {

	uint32_t* dst = (uint32_t*)dst_img;
	uint8_t* src = (uint8_t*)src_img;

	int pixel_idx, pixel_offset, word_idx, word_idx_prev = -1;
	uint32_t pixel_word, pixel_mask;

	int src_pixel_offset, src_word_idx;
	int src_stride = (src_width >> 2);
	uint32_t src_word;

	int col_start = 0;
	int col_end = src_width;

	// Y border limits
	if(y >= dst_height || y + src_height <= 0)
		return;

	if(y < 0) {
		src += -y * src_stride;
		src_height += y;
		y = 0;
	}

	if(y + src_height > dst_height)
		src_height = dst_height - y;
	
	// X border limits
	if(x >= dst_width || x + src_width <= 0)
		return;

	if(x < 0)
		col_start = -x;

	if(x + src_width > dst_width)
		col_end = dst_width - x;
	
	int pixel_idx_0 = y * dst_width + x;

	for(int row = 0; row < src_height; row++) {

		for(int col = col_start; col < col_end; col++) {

			pixel_idx = pixel_idx_0 + col;
			pixel_offset = (15 - (pixel_idx & 0xf)) << 1;

			word_idx = pixel_idx >> 4; // 16 pixels per 32 bit word 

			pixel_mask = 0x03 << pixel_offset;

			if(word_idx != word_idx_prev) {
				if(word_idx_prev != -1)
					dst[word_idx_prev] = pixel_word;

				pixel_word = dst[word_idx];
			}

			// Get image data
			src_word_idx = col >> 2; // 4 pixels per byte
			src_pixel_offset = (3 - (col & 0x03)) << 1;
			src_word = src[src_word_idx];

			pixel_word &= ~pixel_mask;

			if(pixel_offset >= src_pixel_offset)
				src_word <<= (pixel_offset - src_pixel_offset);
			else
				src_word >>= (src_pixel_offset - pixel_offset);

			pixel_word |= src_word & pixel_mask;

			word_idx_prev = word_idx;
		}

		src += src_stride;
		pixel_idx_0 += dst_width;
	}
	dst[word_idx] = pixel_word;
}


/*
 * cga_fill_screen - fills whole screen area with a constand color index.
 *
 */
void cga_fill_screen(char color) {
        uint32_t *fb = (uint32_t*) CGA->FB;

        color = color & 0x3;

        uint32_t filler = (color << 30) | (color << 28) | (color << 26) | (color << 24);
                 filler |= (filler >> 8) | (filler >> 16) | (filler >> 24);

        for(int i = 0; i < CGA_FRAMEBUFFER_SIZE / (8*4); i++) {
                *fb++ = filler;
                *fb++ = filler;
                *fb++ = filler;
                *fb++ = filler;
                *fb++ = filler;
                *fb++ = filler;
                *fb++ = filler;
                *fb++ = filler;
	}

}

void cga_draw_pixel(int x, int y, int color) {
        uint32_t *fb = (uint32_t*) CGA->FB;
	
	int pixel_idx = y * CGA_VIDEO_WIDTH + x;
	int word_idx = pixel_idx >> 4; // 16 pixels per 32 bit word 

	if(word_idx < 0 || word_idx > CGA_FRAMEBUFFER_SIZE/4)
		return;

	int pixel_offset = (15 - (pixel_idx & 0xf)) << 1;
	uint32_t pixel_mask = 0x03 << pixel_offset;
	uint32_t pixel_word = fb[word_idx];

	pixel_word &= ~pixel_mask;
	pixel_word |= ((color & 0x03) << pixel_offset);
	fb[word_idx] = pixel_word;
}

void cga_draw_line(int x0, int y0, int x1, int y1, int color) {
	int dx =  ABS(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -ABS(y1 - y0), sy = y0 < y1 ? 1 : -1; 
	int err = dx + dy, e2; /* error value e_xy */
 
	while(1) { 
		cga_draw_pixel(x0, y0, color);
		if (x0 == x1 && y0 == y1)
			break;
		e2 = 2*err;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		} /* e_xy+e_x > 0 */
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		} /* e_xy+e_y < 0 */
	}
}


/*
 * Set vertical scroll register value to scrl. Resulting effect is:
 * negative value - scroll up scrl scan-lines,
 * positive value - scroll down scrl scan-lines,
 * zero value - no scroll.
 */
void cga_set_scroll(int scrl) {
        CGA->CTRL &= ~CGA_CTRL_V_SCROLL;
        if(scrl >= 0) {   
                CGA->CTRL &= ~CGA_CTRL_V_SCROLL_DIR;
                CGA->CTRL |= (scrl << CGA_CTRL_V_SCROLL_SHIFT) & CGA_CTRL_V_SCROLL;
        } else {
                CGA->CTRL |= CGA_CTRL_V_SCROLL_DIR;
                CGA->CTRL |= ((-scrl) << CGA_CTRL_V_SCROLL_SHIFT) & CGA_CTRL_V_SCROLL;
        }
}

/*
 * Smoothly scroll all the text on the screen upwards by 16 pixels (one line).
 *
 * scroll_delay - delay in microseconds between steps.
 *
 */
void cga_text_scroll_up(int scroll_delay) {
	uint32_t *fb = (uint32_t*) CGA->FB;

	CGA->CTRL2 &= ~CGA_CTRL2_CURSOR_BLINK_EN;
	CGA->CTRL &= ~CGA_CTRL_V_SCROLL_DIR; 

	for(int i = 0; i < 16; i++) {
		cga_wait_vblank();
		CGA->CTRL &= ~CGA_CTRL_V_SCROLL;
		CGA->CTRL |= (i & 0x0f) << CGA_CTRL_V_SCROLL_SHIFT;
		delay_us(scroll_delay);
	}

	cga_wait_vblank();

	for(int col = 0; col < CGA_TEXT_WIDTH; col++) {
		uint32_t tmp = fb[col];
		for(int row = 0; row < CGA_TEXT_HEIGHT_TOTAL - 1; row++) {
			fb[row * CGA_TEXT_WIDTH + col] = fb[row * CGA_TEXT_WIDTH + col + CGA_TEXT_WIDTH];
		}
		fb[(CGA_TEXT_HEIGHT_TOTAL - 1) * CGA_TEXT_WIDTH + col] = tmp;
	}

	CGA->CTRL &= ~CGA_CTRL_V_SCROLL;
	CGA->CTRL2 |= CGA_CTRL2_CURSOR_BLINK_EN;
}


/*
 * Smoothly scroll all the text on the screen downwards by 16 pixels (one line).
 *
 * scroll_delay - delay in microseconds between steps.
 *
 */
void cga_text_scroll_down(int scroll_delay) {
	uint32_t *fb = (uint32_t*) CGA->FB;

	CGA->CTRL2 &= ~CGA_CTRL2_CURSOR_BLINK_EN;
	CGA->CTRL |= CGA_CTRL_V_SCROLL_DIR; 

	for(int i = 0; i < 16; i++) {
		cga_wait_vblank();
		CGA->CTRL &= ~CGA_CTRL_V_SCROLL;
		CGA->CTRL |= (i & 0x0f) << CGA_CTRL_V_SCROLL_SHIFT;
		delay_us(scroll_delay);
	}

	cga_wait_vblank();

	for(int col = 0; col < CGA_TEXT_WIDTH; col++) {
		uint32_t tmp = fb[(CGA_TEXT_HEIGHT_TOTAL - 1) * CGA_TEXT_WIDTH + col];
		for(int row = CGA_TEXT_HEIGHT_TOTAL - 1; row > 0; row--) {
			fb[row * CGA_TEXT_WIDTH + col] = fb[row * CGA_TEXT_WIDTH + col - CGA_TEXT_WIDTH];
		}
		fb[col] = tmp;
	}

	CGA->CTRL &= ~CGA_CTRL_V_SCROLL;
	CGA->CTRL2 |= CGA_CTRL2_CURSOR_BLINK_EN;
}


void cga_set_cursor_xy(int x, int y) {
	CGA->CTRL2 &= ~CGA_CTRL2_CURSOR_X;
	CGA->CTRL2 |= (x & 0xff) << CGA_CTRL2_CURSOR_X_SHIFT;
	CGA->CTRL2 &= ~CGA_CTRL2_CURSOR_Y;
	CGA->CTRL2 |= (y & 0xff) << CGA_CTRL2_CURSOR_Y_SHIFT;
}


void cga_set_cursor_style(int top, int bottom) {
	CGA->CTRL2 &= ~CGA_CTRL2_CURSOR_TOP;
	CGA->CTRL2 |= (top & 0x0f) << CGA_CTRL2_CURSOR_TOP_SHIFT;
	CGA->CTRL2 &= ~CGA_CTRL2_CURSOR_BOTTOM;
	CGA->CTRL2 |= (bottom & 0x0f) << CGA_CTRL2_CURSOR_BOTTOM_SHIFT;
}

void cga_text_print(uint8_t *framebuffer, int x, int y, int fg_color, int bg_color, char *text)
{
	if(!text)
		return;

	uint32_t *fb = (uint32_t*) framebuffer;
	uint32_t attributes = (fg_color << 8) | (bg_color << 16);

	fb += CGA_TEXT_WIDTH * y + x;

	for(int i = 0; text[i]; i++) {
		if(text[i] == '\n') {
			fb += CGA_TEXT_WIDTH;
		} else if(text[i] == '\r') {
			fb -= (fb - (uint32_t*)framebuffer) % CGA_TEXT_WIDTH;
		} else if(text[i] == '\t') {
			for(int j = 0; j < 8; j++)
				*fb++ = attributes | 0x20;
		} else if(text[i] == 0x1b) {
			i++;
			switch(text[i]) {
				case 'F': {
					i++;
					attributes &= ~ 0x0000ff00;
					attributes |= atoi(&text[i]) << 8;
					while(text[++i] != ';');
				} break;
				case 'B': {
					i++;
					attributes &= ~ 0x00ff0000;
					attributes |= atoi(&text[i]) << 16;
					while(text[++i] != ';');
				} break;
				case 'D': {
					i++;
					fb += CGA_TEXT_WIDTH * atoi(&text[i]);
					while(text[++i] != ';');
				} break;
				case 'U': {
					i++;
					fb -= CGA_TEXT_WIDTH * atoi(&text[i]);
					while(text[++i] != ';');
				} break;
				case 'L': {
					i++;
					fb -= atoi(&text[i]);
					while(text[++i] != ';');
				} break;
				case 'R': {
					i++;
					fb += atoi(&text[i]);
					while(text[++i] != ';');
				} break;
			}
			continue;
		} else
			*fb++ = attributes | text[i];
	}

}


