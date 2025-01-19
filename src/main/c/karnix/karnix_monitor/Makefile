PROJ_NAME=karnix_monitor
#MODEL ?= ram
MODEL ?= xip
#
DEBUG=no
BENCH=no
MULDIV=yes
ATOMIC=yes
FPU=yes
COMPRESSED=yes

DEFS = -DCONFIG_HAS_MODBUS -DCONFIG_HAS_MAC
#DEFS += -DDEBUG_SBRK 

# LwIP support
LWIPDIR=../karnix_lwip/src
SOCDIR=../karnix_soc/src
include $(LWIPDIR)/Filelists.mk

CRT_RAM=src/crt_monitor_ram.S
LDSCRIPT_RAM=src/linker_monitor_ram.ld -Wl

CRT_XIP=src/crt_monitor_xip.S
LDSCRIPT_XIP=src/linker_monitor_xip.ld -Wl


SRCS += $(wildcard src/*.c) $(wildcard src/*.cpp)
SRCS += $(wildcard $(SOCDIR)/*.c) $(wildcard $(SOCDIR)/*.cpp)
SRCS += $(COREFILES) $(CORE4FILES) $(LWIPDIR)/netif/ethernet.c
SRCS += $(CRT)


OBJDIR = build

BUILD_NUMBER_FILE=$(OBJDIR)/build_number
BUILD_NUMBER=`cat $(BUILD_NUMBER_FILE)`


INC  = -I./src -I$(LWIPDIR) -I$(LWIPDIR)/include -I$(SOCDIR) -I$(SOCDIR)/include
LIBS =
LIBSINC = -L$(OBJDIR)
#LDSCRIPT = src/linker_monitor.ld -Wl

#include ../../../resources/gcc.mk
# Set it to yes if you are using the sifive precompiled GCC pack
SIFIVE_GCC_PACK ?= yes

ifeq ($(SIFIVE_GCC_PACK),yes)
	RISCV_NAME ?= riscv64-unknown-elf
	RISCV_PATH ?= /opt/riscv64
else
	RISCV_NAME ?= riscv32-unknown-elf
	ifeq ($(MULDIV),yes)
		RISCV_PATH ?= /opt/riscv32im/
	else
		RISCV_PATH ?= /opt/riscv32i/
	endif
endif

MABI=ilp32
MARCH := rv32i
ifeq ($(MULDIV),yes)
	MARCH := $(MARCH)m
endif
ifeq ($(ATOMIC),yes)
	MARCH := $(MARCH)a
endif
ifeq ($(FPU),yes)
	MARCH := $(MARCH)f
	#MABI := $(MABI)f
	MABI := $(MABI)f
	# Below uses too much RAM (over 100K)
	#CFLAGS += -Wl,-u,vfprintf
	#CFLAGS += -Wl,-u,_printf_float
	#CFLAGS += -Wl,-u,_scanf_float
endif
ifeq ($(COMPRESSED),yes)
	MARCH := $(MARCH)c
endif

#CFLAGS += -fwhole-program -march=$(MARCH)  -mabi=$(MABI) -DNDEBUG -fno-common -fno-builtin-printf -DBUILD_NUMBER=$(BUILD_NUMBER)+1 
# use suffix __attribute__((externally_visible))

CFLAGS += -march=$(MARCH)  -mabi=$(MABI) -DNDEBUG -flto -fno-common -fno-builtin-printf -DBUILD_NUMBER=$(BUILD_NUMBER)+1 $(DEFS) 
LDFLAGS += -march=$(MARCH)  -mabi=$(MABI) -specs=nano.specs -lnosys -lc_nano
#LDFLAGS += -march=rv32im -mabi=ilp32 -specs=nano.specs -lnosys -lc_nano


ifeq ($(DEBUG),yes)
	CFLAGS += -g3 -O0
endif

ifeq ($(DEBUG),no)
	CFLAGS += -Os
endif

ifeq ($(BENCH),yes)
	CFLAGS += -fno-inline
endif

ifeq ($(SIFIVE_GCC_PACK),yes)
	RISCV_CLIB=$(RISCV_PATH)/$(RISCV_NAME)/lib/$(MARCH)/$(MABI)/
else
	RISCV_CLIB=$(RISCV_PATH)/$(RISCV_NAME)/lib/
endif


ifeq ($(MODEL),ram)
	CRT = $(CRT_RAM)
	LDSCRIPT = $(LDSCRIPT_RAM)
	PROJ_NAME_M = $(PROJ_NAME)_ram
else
ifeq ($(MODEL),xip)
	CRT = $(CRT_XIP)
	LDSCRIPT = $(LDSCRIPT_XIP)
	PROJ_NAME_M = $(PROJ_NAME)_xip
else
	#
endif
endif


RISCV_OBJCOPY = $(RISCV_PATH)/bin/$(RISCV_NAME)-objcopy
RISCV_OBJDUMP = $(RISCV_PATH)/bin/$(RISCV_NAME)-objdump
RISCV_CC=$(RISCV_PATH)/bin/$(RISCV_NAME)-gcc

CFLAGS +=  -MD -fstrict-volatile-bitfields -fno-strict-aliasing
LDFLAGS +=  -nostdlib -lgcc -mcmodel=medany -nostartfiles -ffreestanding -Wl,-Bstatic,-T,$(LDSCRIPT),-Map,$(OBJDIR)/$(PROJ_NAME_M).map,--print-memory-usage
#LDFLAGS += -lgcc -lc -lg -nostdlib  -lgcc  -msave-restore    --strip-debug,

OBJS := $(SRCS)
OBJS := $(OBJS:.c=.o)
OBJS := $(OBJS:.cpp=.o)
OBJS := $(OBJS:.S=.o)
#OBJS := $(OBJS:..=miaou)
OBJS := $(subst ..,__,$(OBJS))
OBJS := $(addprefix $(OBJDIR)/,$(OBJS))


all:
	$(MAKE) ram
	$(MAKE) xip

ram: check_build_num_file
	@echo "*** Building for RAM ***"
	$(MAKE) MODEL=ram build_sources $(OBJDIR)/$(PROJ_NAME).hexx

xip: check_build_num_file
	@echo "*** Building for XIP ***"
	$(MAKE) MODEL=xip build_sources


build_sources: $(OBJDIR)/$(PROJ_NAME_M).elf $(OBJDIR)/$(PROJ_NAME_M).hex $(OBJDIR)/$(PROJ_NAME_M).bin $(OBJDIR)/$(PROJ_NAME_M).asm $(OBJDIR)/$(PROJ_NAME_M).v

check_build_num_file:
ifeq (,$(wildcard $(BUILD_NUMBER_FILE)))
	mkdir -p $(OBJDIR)
	echo 1 > $(BUILD_NUMBER_FILE)
endif

inc_build_num:
	@expr $(BUILD_NUMBER) + 1 > $(BUILD_NUMBER_FILE) 

$(OBJDIR)/%.elf: $(OBJS) | $(OBJDIR)
	$(RISCV_CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBSINC) $(LIBS)

%.hex: %.elf
	$(RISCV_OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(RISCV_OBJCOPY) -O binary $^ $@

%.hexx: %_ram.bin 
	hexdump -v -e '/4 "%08X\n"' < $^ > $@ 
	$(MAKE) inc_build_num

%.v: %.elf
	$(RISCV_OBJCOPY) -O verilog $^ $@

%.asm: %.elf
	$(RISCV_OBJDUMP) -S -d $^ > $@

%/src/cga.o: CFLAGS += -O1 

$(OBJDIR)/__/%.o: ../%.c
	mkdir -p $(dir $@)
	$(RISCV_CC) -c $(CFLAGS)  $(INC) -o $@ $^
	$(RISCV_CC) -S $(CFLAGS)  $(INC) -o $@.disasm $^

$(OBJDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(RISCV_CC) -c $(CFLAGS)  $(INC) -o $@ $^
	$(RISCV_CC) -S $(CFLAGS)  $(INC) -o $@.disasm $^

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(RISCV_CC) -c $(CFLAGS)  $(INC) -o $@ $^

$(OBJDIR)/__/%.o: ../%.cpp
	mkdir -p $(dir $@)
	$(RISCV_CC) -c $(CFLAGS)  $(INC) -o $@ $^

$(OBJDIR)/__/%.o: ../%.S
	mkdir -p $(dir $@)
	$(RISCV_CC) -c $(CFLAGS) -o $@ $^ -D__ASSEMBLY__=1

$(OBJDIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(RISCV_CC) -c $(CFLAGS) -o $@ $^ -D__ASSEMBLY__=1

$(OBJDIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(OBJDIR)/src
	rm -rf $(OBJDIR)/__
	rm -rf $(OBJDIR)/$(PROJ_NAME)*

clean-all : clean 

.SECONDARY: $(OBJS)
