ifndef CROSS_COMPILE
CROSS_COMPILE=riscv32-unknown-elf-
endif

# Optimization: do not use -O3 as it generates sb/sh/lh/lhu that the *bus*
# is not able to handle yet
TARGET_CC = $(CROSS_COMPILE)gcc -g -O1 -march=rv32im -mabi=ilp32
TARGET_LD = $(CROSS_COMPILE)ld -nostartfiles -m elf32lriscv
TARGET_OBJDUMP = $(CROSS_COMPILE)objdump
TARGET_READELF = $(CROSS_COMPILE)readelf
HOST_CC = gcc
HOST_LD = ld
HOST_OBJDUMP = objdump
HOST_READELF = readelf


INCLUDE = -I. -I..

HEADERS = ../fixed_point_params.h ../address_map.h ../adc_params.h

OBJS = main.o trap.o it.o boot.o
EXEC = a.out

# Disassembly of the generated executable.
dump.dis: $(EXEC)
	$(TARGET_OBJDUMP) --disassembler-options=no-aliases,numeric -d $< > $@

# Summary of the sections and their size.
sections.txt: $(EXEC)
	$(TARGET_READELF) -S $< > $@

# START CUT
$(EXEC): $(OBJS) ldscript
	$(TARGET_LD) -Tldscript -o $@  $(OBJS) -L/opt/riscv/lib/gcc/riscv32-unknown-elf/8.2.0 -lgcc

main.o: main.c $(HEADERS)
	$(TARGET_CC) $(INCLUDE) -c $< -o $@

trap.o : trap.c
	$(TARGET_CC) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(TARGET_CC) -c $<

# UNCOMMENT $(EXEC): # TODO: dependencies
# UNCOMMENT 	echo "Rule for linking not implemented."; exit 1
# TODO: use $(TARGET_LD), and use the -T option to point to linker script ldscript

# UNCOMMENT main.o: # TODO: dependencies
# UNCOMMENT 	echo "Rule for compilation not implemented"; exit 1
# TODO: compile with target C compiler

# UNCOMMENT %.o: %.s
# UNCOMMENT 	echo "Rule for assembly not implemented"; exit 1

.PHONY: clean realclean
clean:
	-$(RM) $(OBJS) $(EXEC) dump.dis sections.txt

realclean: clean
	-$(RM) *~