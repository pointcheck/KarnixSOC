#ifndef	__CONFIG_USERDATA_H__

#define	TETRIS_NUM_SCORES	8

typedef struct {
		struct	{
			uint32_t name;
			uint16_t score;
		} scrores[TETRIS_NUM_SCORES];
} ConfigUserData;

#endif // __CONFIG_USERDATA_H__

