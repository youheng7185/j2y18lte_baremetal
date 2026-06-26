CCPREFIX ?= aarch64-linux-gnu-
CC     = $(CCPREFIX)gcc
LD     = $(CCPREFIX)ld
OBJCPY = $(CCPREFIX)objcopy

# Bare-metal AArch64 flags
#   -mbranch-protection=none  : disable PAC/BTI (paciasp/autiasp/bti instructions
#                               crash on cores that don't implement the extension,
#                               which is the case for most Android bootloader EL1 env)
#   -mgeneral-regs-only       : no NEON/FP register use; NEON stp-q stores fault
#                               if the memory region is typed as Device rather than Normal
#   -ffreestanding            : no host stdlib assumptions
#   -fno-builtin              : don't replace memset/memcpy etc. with NEON builtins
#   -fno-stack-protector      : no __stack_chk_guard (needs libc)
#   -fno-PIE / -fno-pie       : no position-independent code (fixed load address)
#   -march=armv8-a            : plain ARMv8-A, no architecture extensions assumed

CFLAGS = \
	-mbranch-protection=none \
	-mgeneral-regs-only \
	-ffreestanding \
	-fno-builtin \
	-fno-stack-protector \
	-fno-PIE \
	-fno-pie \
	-march=armv8-a \
	-Idrivers \
	-O2

OBJ = boot.o main.o drivers/display.o drivers/font.o drivers/timer.o

wrapped_kernel: wrapper.elf
	$(OBJCPY) -O binary $< $@

wrapper.elf: $(OBJ) linker.lds
	$(LD) $(OBJ) -o $@ -T linker.lds

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o drivers/*.o wrapper.elf wrapped_kernel