#ifndef __HUBxx_H__
#define __HUBxx_H__

#include <stdint.h>

extern const char font_6x8[];
extern const char font_12x16[];

extern uint32_t hub_frame_size;
extern uint32_t hub_frame_width;
extern uint32_t hub_frame_height;


#define	HUB_FRAMEBUFFER_SIZE	(4*1024)

#pragma pack(1)
typedef struct
{
  volatile uint8_t FB[HUB_FRAMEBUFFER_SIZE];				// Framebuffer: 4K 
  volatile uint8_t unused[16*1024-HUB_FRAMEBUFFER_SIZE];		//  
  volatile uint32_t CONTROL;						// Control register, see bit masks below
  volatile uint32_t PLANE_OFFSETS[8];					// Helper pointer to each plane data, relative to begin of framebuffer
} Hub_Reg;
#pragma pack(0)


#define	HUB_MASK_TYPE		0x0000007f
#define	HUB_MASK_ENABLE		0x00000080
#define	HUB_MASK_BRIGHTNESS	0x0000ff00
#define	HUB_MASK_WIDTH		0x00ff0000
#define	HUB_MASK_PLANES		0xff000000

#define	HUB_BITS_TYPE		0
#define	HUB_BITS_ENABLE		7
#define	HUB_BITS_BRIGHTNESS	8
#define	HUB_BITS_WIDTH		16
#define	HUB_BITS_PLANES		24

#define	HUB_TYPE_HUB12		12
#define	HUB_TYPE_HUB75		75

#define	HUB_COLOR_WHITE		0x3f

int hub_init(uint32_t hub_type);
void hub_print_char(volatile uint8_t* fb, char c, int x, int y, char color, int ledpad_width, int ledpad_height, int hub_type, const char *font, int font_width, int font_height);
void hub_print(int x, int y, int color, char *text, int text_size, const char *font, int font_width, int font_height);


#endif /* __HUB_H__ */


