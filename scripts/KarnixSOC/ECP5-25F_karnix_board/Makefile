RAMPROG = karnix_bootloader
#RAMPROG = karnix_monitor
#RAMPROG = karnix_xip_test
#RAMPROG = karnix_tetriscv
#RAMPROG = karnix_cga_test

VERILOG = ../../../KarnixSOCTopLevel.v ../../../src/main/verilog/TMDS_encoder.sv ../../../src/main/verilog/OBUFDS.sv

generate: ../../../KarnixSOCTopLevel_random.hexx ../../../font8x16x256.hex
	(cd ../../..; sbt "runMain vexriscv.demo.KarnixSOCVerilog")

sim:
	(cd ../../..; sbt "runMain vexriscv.demo.KarnixSOCSim")

hexx2bin: hexx2bin.c
	cc -o $@ $^

../../../font8x16x256.hex: koi8-r-8x16.bin
	objcopy -I binary -O ihex $^ $@


../../../KarnixSOCTopLevel_random.hexx: hexx2bin
	#ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 23552 # 92K RAM 
	#ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 24064 # 94K RAM 
	#ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 24576 # 96K RAM 
	#ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 22016 # 86K RAM 
	#ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 20480 # 80K RAM 
	#ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 23552 # 92K RAM 
	#ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 23040 # 90K RAM 
	#ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 18944 # 74K RAM 
	ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 18432 # 72K RAM 
	#ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 17920 # 70K RAM
	#ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 16384 
	#ecpbram -g ../../../KarnixSOCTopLevel_random.hexx -w 32 -d 5120 
	./hexx2bin < ../../../KarnixSOCTopLevel_random.hexx > ../../../KarnixSOCTopLevel_random.bin
	objcopy -v -O ihex -I binary --set-start 0x80000000 --change-addresses 0x80000000 ../../../KarnixSOCTopLevel_random.bin ../../../KarnixSOCTopLevel_random.hex
	#sed 's/^:10/:00/' < ../../../KarnixSOCTopLevel_random.he > ../../../KarnixSOCTopLevel_random.hex

../../../KarnixSOCTopLevel.v :
	(cd ../../..; sbt "runMain vexriscv.demo.KarnixSOCVerilog")


../../../src/main/c/karnix/$(RAMPROG)/build/$(RAMPROG).hexx:
	(cd ../../../src/main/c/karnix/$(RAMPROG); $(MAKE) ram)


../../../KarnixSOCTopLevel.v*.bin: ../../../src/main/c/karnix/$(RAMPROG)/build/$(RAMPROG).hexx


bin/KarnixSOCTopLevel.json : ${VERILOG} ../../../KarnixSOCTopLevel.v*.bin
	mkdir -p bin
	yosys -v2 -p "synth_ecp5 -abc9 -top KarnixSOCTopLevel -json bin/KarnixSOCTopLevel.json" ${VERILOG}

bin/KarnixSOCTopLevel_random_25F.config : karnix_cabga256.lpf bin/KarnixSOCTopLevel.json
	nextpnr-ecp5 --seed 1122 --speed 8 --25k --parallel-refine --lpf karnix_cabga256.lpf --placed-svg bin/KarnixSOCTopLevel_placed.svg --package CABGA256 --json bin/KarnixSOCTopLevel.json --textcfg $@ 

bin/KarnixSOCTopLevel_25F.config: bin/KarnixSOCTopLevel_random_25F.config ../../../src/main/c/karnix/$(RAMPROG)/build/$(RAMPROG).hexx ../../../KarnixSOCTopLevel_random.hexx
	ecpbram -v -i bin/KarnixSOCTopLevel_random_25F.config -o bin/KarnixSOCTopLevel_25F.config -f ../../../KarnixSOCTopLevel_random.hexx -t ../../../src/main/c/karnix/$(RAMPROG)/build/$(RAMPROG).hexx

bin/KarnixSOCTopLevel_25F-$(RAMPROG).bit : bin/KarnixSOCTopLevel_25F.config
	ecppack --svf bin/KarnixSOCTopLevel_25F.svf  bin/KarnixSOCTopLevel_25F.config bin/KarnixSOCTopLevel_25F-$(RAMPROG).bit

compile : bin/KarnixSOCTopLevel_25F-$(RAMPROG).bit

prog : bin/KarnixSOCTopLevel_25F-$(RAMPROG).bit
	openFPGALoader bin/KarnixSOCTopLevel_25F-$(RAMPROG).bit

clean :
	rm -f bin/* ../../../KarnixSOCTopLevel* ../../../font8x16x256.hex

gdb:
	$(GDB) -b 115200 -ex "set debug remote 1" -ex "target remote /dev/ttyUSB0" ../../../src/main/c/karnix/$(RAMPROG)/build/$(RAMPROG).elf 


