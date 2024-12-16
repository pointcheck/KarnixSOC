#include <limits.h>
#include <string.h>
#include "utils.h"
#include "soc.h"
#include "hub.h"


struct hub_config {
	uint8_t	MATRIX_TYPE;		// 12 - for HUB12, 75 - for HUB75 
	uint8_t	MATRIX_WIDTH;		// pixels per row
	uint8_t MATRIX_PLANES;		// number of planes per matrix
	uint8_t MATRIX_ROWS_PER_PLANE;	// number of row in a plane
	uint8_t MATRIX_BITS_PER_PIXEL;	// 
} hub_configs[] = {
	{
		12,			// HUB12 matrix
		32,			// pixels per row
		4,			// number of planes per matrix
		4,			// number of row in a plane
		1			// Format: 0bRRRRRRRR
	},
	{
		75,			// HUB75 matrix
		80,			// pixels per row
		2,			// number of planes per matrix
		20,			// number of row in a plane
		8			// Format: 0bXXBBGGRR
	}
};


uint32_t hub_frame_size = 0;
uint32_t hub_frame_width = 0;
uint32_t hub_frame_height = 0;


int hub_init(uint32_t hub_type) {

	int hub_idx;

	/// Find corresponding item in config preset array 
	for(hub_idx = 0; hub_idx < sizeof(hub_configs) / sizeof(*hub_configs); hub_idx++) {
		if(hub_configs[hub_idx].MATRIX_TYPE == hub_type)
			break;
	}

	if(hub_idx == sizeof(hub_configs) / sizeof(*hub_configs))
		return -1; // not found


	hub_frame_size = hub_configs[hub_idx].MATRIX_WIDTH * 
			hub_configs[hub_idx].MATRIX_PLANES * 
			hub_configs[hub_idx].MATRIX_ROWS_PER_PLANE * 
			hub_configs[hub_idx].MATRIX_BITS_PER_PIXEL / 8;

	hub_frame_width = hub_configs[hub_idx].MATRIX_WIDTH;
	hub_frame_height = hub_configs[hub_idx].MATRIX_PLANES * hub_configs[hub_idx].MATRIX_ROWS_PER_PLANE;

	// Disable HUB controller
	HUB->CONTROL = 0;

	// Fill-in plane offsets in bytes which is an offset from base address
	for(int i = 0; i < hub_configs[hub_idx].MATRIX_PLANES; i++) {
		HUB->PLANE_OFFSETS[i] = (i * hub_configs[hub_idx].MATRIX_WIDTH * 
						hub_configs[hub_idx].MATRIX_BITS_PER_PIXEL * 
						hub_configs[hub_idx].MATRIX_ROWS_PER_PLANE) / 8;
	}

	// Clear framebuffer
	//memset((void*)HUB->FB, 0, hub_frame_size);

	// Configure and enable HUB controller 
	HUB->CONTROL   = ((255) << HUB_BITS_BRIGHTNESS)
			| ((hub_configs[hub_idx].MATRIX_WIDTH) << HUB_BITS_WIDTH)
			| ((hub_configs[hub_idx].MATRIX_PLANES) << HUB_BITS_PLANES)
			| ((hub_configs[hub_idx].MATRIX_TYPE) << HUB_BITS_TYPE)
			| HUB_MASK_ENABLE;


	return 0;
}




void hub_print(int x, int y, int color, char *text, int text_size, const char *font, int font_width, int font_height) {

	//printf("hub_print() x = %d, y = %d, color = %p, text = %s, font = %p, font_width = %d, font_height = %d\r\n", 
	//	x, y, color, text, font, font_width, font_height);


	for(int i = 0; i < text_size; i++) {

		hub_print_char(HUB->FB, text[i], x + i * font_width, y, color, hub_frame_width, hub_frame_height, HUB->CONTROL & HUB_MASK_TYPE, font, font_width, font_height); 
	} 
}



void hub_print_char(volatile uint8_t* fb, char c, int x, int y, char color, int frame_width, int frame_height, int hub_type, const char *font, int font_width, int font_height)
{
	volatile uint32_t* ffb = (uint32_t*)fb;

	int my_y;

	int lines = font_height / 8;


	if(font_height % 8) 
		lines++;

	//printf("hub_print_char() fb = %p, frame_width = %d, frame_height = %d, x = %d, y = %d, c = %c\r\n", fb, frame_width, frame_height, x, y, c);

	for(int line = 0; line < lines; line++) {

		int line_height = MIN(8, (font_height - line * 8));
		int my_x = x;

		for(int col = 0; col < font_width; col++) {

			if(my_x >= 0 && my_x < frame_width) {

				char bit_mask = 0x01;
				uint32_t f;
			
				f = font[font_width * c * lines + line * font_width + col];

				my_y = y + line * 8;

				for(int bit = 0; bit < line_height; bit++) {

					if(my_y >= 0 && my_y < frame_height) {

						int pixel_idx = my_y * frame_width + my_x; 
						int word_idx;
						int mask;
						int tmp;

						switch(hub_type) {
							case 12: {
								word_idx = pixel_idx / 32;
								mask = 1 << (31 - pixel_idx % 32);
							} break;
							case 75: {
								word_idx = pixel_idx / 4;
								mask = 0xff << ((3 - pixel_idx % 4) * 8);
							} break;
						}

						tmp = SWAP32(ffb[word_idx]);

						if((f & bit_mask)) {
							switch(hub_type) {
								case 12: {
									ffb[word_idx] = SWAP32(tmp | mask); 
								} break;
								case 75: {
									tmp &= ~mask;
									tmp |= (color << ((3 - pixel_idx % 4) * 8));
									ffb[word_idx] = SWAP32(tmp); 
								} break;
							}
						} else {
							switch(hub_type) {
								case 12: {
									ffb[word_idx] = SWAP32(tmp & ~mask); 
								} break;
								case 75: {
									tmp &= ~mask;
									ffb[word_idx] = SWAP32(tmp); 
								} break;
							}
						}
					}

					bit_mask = bit_mask << 1;
					my_y++;
				}
			}

			my_x++;
		}
	}

}


