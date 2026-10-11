#!/bin/bash
set -e

MKBOOTIMG=./mkbootimg
DT=twrp-3.7.0_9-0-j2y18lte.img-dt

echo "==> Cleaning..."
make clean

echo "==> Building wrapped_kernel..."
make wrapped_kernel \
CMDLINE="clk_ignore_unused pd_ignore_unused ignore_loglevel earlyprintk rdinit=/init deferred_probe_timeout=600" \
KERNEL_PATH=/home/lapchong/arm_mpu/phones/bootloader/j2y18lte_baremetal/1/Image \
DTB_PATH=/home/lapchong/arm_mpu/phones/bootloader/j2y18lte_baremetal/1/msm8917-samsung-j2y18lte.dtb

rm -rf initramfs.cpio.gz
cp busybox/initramfs.cpio.gz initramfs.cpio.gz

echo "==> Packing boot.img..."
$MKBOOTIMG \
--kernel wrapped_kernel \
--ramdisk initramfs.cpio.gz \
--dt $DT \
--base 0x80000000 \
--kernel_offset 0x00008000 \
--ramdisk_offset 0x02000000 \
--tags_offset 0x01e00000 \
--board SRPQH31A000RU \
--pagesize 2048 \
-o boot.img

echo "==> Done! boot.img ready."

if [ "$1" == "--flash" ]; then
    echo "==> Booting via fastboot..."
    fastboot boot boot.img
fi