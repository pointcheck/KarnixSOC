#ifndef _ENV_H_
#define _ENV_H_

#define	ENV_MEM_ADDRESS		0xA01D0000
#define	ENV_MEM_SIZE		0x10000

struct EnvHeader {
	uint8_t name_len;	// including trailing zero
	uint8_t value_len;	// including trailing zero
	// char name[name_len];
	// char value[value_len];
};

#endif // _ENV_H_

