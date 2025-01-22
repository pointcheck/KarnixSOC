#ifndef __SRAM_H__
#define	__SRAM_H__

#define SRAM_SIZE       (512*1024)
#define SRAM_ADDR_BEGIN 0x90000000
#define SRAM_ADDR_END   (0x90000000 + SRAM_SIZE)

int sram_test_write_random_ints(int interations);

#endif // __SRAM_H__

