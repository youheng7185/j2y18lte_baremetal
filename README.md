# j2y18lte baremetal

```
./build.sh
```
to compile and pack boot.img

# Minimal S-Boot Wrapper
This small wrapper is designed to help booting Mainline Linux on
devices using Samsung S-Boot bootloader, especially Exynos7885 phones.
It is currently capable of starting the linked Kernel image with a
linked DTB, optionally waiting for a power button press before starting
the kernel. The dtb given to the wrapper by S-Boot is ignored, but we
currently use the ramdisk loaded in memory by S-Boot.
The advantages are that we can modify the cmdline in the dtb without
S-Boot interfering and we can plug in UART before the kernel boots after
using Download mode to flash a boot.img

## Building:
Do a simple `make` with optionally overriding the default variables.
These variables are:

**CCPREFIX**: Prefix to compiler commands. Default: "aarch64-linux-gnu-"  
**KERNEL_PATH**: Path to the kernel image. Default: "kernel"  
**DTB_PATH**: Path to the dtb. Default: "dtb"  
**CMDLINE**: Command line passed to kernel. Default: "clk_ignore_unused"  
**POWER_KEY_BOOT**: Only boot when the power key is pressed.


Example command:
`make POWER_KEY_BOOT=1 CMDLINE="clk_ignore_unused fbcon=rotate:1" KERNEL_PATH=/path/to/kernel/arch/arm64/Image DTB_PATH=/path/to/kernel/arch/arm64/boot/dts/exynos/exynos7885-jackpotlte.dtb`

**Usage:**
The resulting "wrapped_kernel" can be used instead of a kernel in a
boot.img to be flashed onto an S-Boot device. Providing a seperate
dtb in the boot image is not neccessary.


## J2 U-Boot handoff build

`build3.sh` packages the wrapper around `u-boot-msm8917.Image`. It takes
`KERNEL_PATH`, `DTB_PATH`, `DT`, `MKBOOTIMG`, and `CMDLINE` from the environment
and uses repository-relative defaults. Provide `initramfs.cpio.gz` in this
folder before building; the Makefile embeds that filename. Build with:

```sh
bash ./build3.sh
```

This creates `boot.img` without flashing it. The optional `--flash` argument
actually runs `fastboot boot boot.img`, for temporary booting on a compatible
fastboot setup. Retain the original Samsung DT table expected by the wrapper.

The wrapper copies the actual embedded image size (bounded by `kernel_end`)
to the U-Boot load address rather than reading a fixed 32 MiB. It draws yellow
and cyan bands at the bottom of the J2 framebuffer before handing off.

Kernel: https://github.com/youheng7185/linux_j2y18lte/tree/msm89x7/7.1.3

U-Boot: https://github.com/youheng7185/u-boot

Debian XFCE rootfs, Samsung WCNSS firmware and SD installation guide:
https://github.com/youheng7185/mainline_linux_j2y18lte

Rootfs directories and generated images are local build inputs/outputs;
they are not part of this source update. Historically tracked binary files
are left untouched by this commit.
