CCPREFIX ?= arm-linux-gnueabi-
CC     = $(CCPREFIX)gcc
LD     = $(CCPREFIX)ld
OBJCPY = $(CCPREFIX)objcopy

OBJ = boot.o main.o

wrapped_kernel: wrapper.elf
	$(OBJCPY) -O binary $< $@

wrapper.elf: $(OBJ) linker.lds
	$(LD) $(OBJ) -o $@ -T linker.lds

%.o: %.S
	$(CC) -c -o $@ $<

%.o: %.c
	$(CC) -fno-builtin -ffreestanding -O2 -c -o $@ $<

clean:
	rm -f *.o wrapper.elf wrapped_kernel