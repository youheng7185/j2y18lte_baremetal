KERNEL_PATH ?= /home/lapchong/arm_mpu/phones/bootloader/j2y18lte_baremetal/1/Image
DTB_PATH    ?= /home/lapchong/arm_mpu/phones/bootloader/j2y18lte_baremetal/1/msm8917-samsung-j2y18lte.dtb
CMDLINE     ?= clk_ignore_unused ignore_loglevel earlyprintk rdinit=/init

CCPREFIX ?= aarch64-linux-gnu-
CC     = $(CCPREFIX)gcc
CPP    = $(CCPREFIX)cpp
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

kernel.o: $(KERNEL_PATH)
	$(OBJCPY) -I binary -O elf64-littleaarch64 -B aarch64 \
	    --rename-section .data=.kernel \
	    --redefine-sym _binary_Image_start=kernel \
	    $< $@

dtb.o: tmp.dtb
	$(OBJCPY) -I binary -O elf64-littleaarch64 -B aarch64 \
	    --rename-section .data=.dtb \
	    --redefine-sym _binary_tmp_dtb_start=dtb \
	    $< $@

OBJ = boot.o main.o drivers/display.o drivers/font.o drivers/timer.o kernel.o dtb.o initramfs.o

wrapped_kernel: wrapper.elf
	$(OBJCPY) -O binary $< $@

wrapper.elf: $(OBJ) linker.lds tmp.dtb
	$(LD) $(OBJ) -o $@ -T linker.lds --no-warn-rwx-segments

# Preprocess linker script, injecting KERNEL_PATH and DTB_PATH as symbols
linker.lds: linker.lds.S
	cp linker.lds.S linker.lds

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Patch the DTB with the kernel cmdline and initrd address range,
# then repack it as a binary DTB blob
RAMDISK_START ?= 0x82000000   # 0x80000000 + 0x02000000

tmp.dtb: $(DTB_PATH) cmdline_value
	$(eval RDSIZE := $(shell wc -c < initramfs.cpio.gz))
	$(eval RDEND  := $(shell printf '0x%X' $$(($(RAMDISK_START) + $(RDSIZE)))))
	( dtc -O dts $(DTB_PATH) && \
	  echo '/ { chosen { bootargs = "$(CMDLINE)"; linux,initrd-start = <0x82000000>; linux,initrd-end = <$(RDEND)>; }; };' \
	) | dtc -O dtb -o $@

# Sentinel file — rebuilds tmp.dtb only when CMDLINE actually changes
cmdline_value:
	@echo $(CMDLINE) > cmdline.tmp
	@diff -q cmdline_value cmdline.tmp 2>/dev/null || cp cmdline.tmp cmdline_value
	@rm cmdline.tmp

initramfs.o: initramfs.cpio.gz
	$(OBJCPY) -I binary -O elf64-littleaarch64 -B aarch64 \
	    --rename-section .data=.initramfs \
	    --redefine-sym _binary_initramfs_cpio_gz_start=initramfs \
	    --redefine-sym _binary_initramfs_cpio_gz_end=initramfs_end \
	    $< $@

clean:
	rm -f *.o drivers/*.o wrapper.elf wrapped_kernel linker.lds tmp.dtb cmdline_value