#!/bin/bash
set -e

MKBOOTIMG=./mkbootimg
RAMDISK=twrp-3.7.0_9-0-j2y18lte.img-ramdisk.gz
DT=twrp-3.7.0_9-0-j2y18lte.img-dt

echo "==> Cleaning..."
make clean

echo "==> Building..."
make

echo "==> Packing boot.img..."
$MKBOOTIMG \
  --kernel wrapped_kernel \
  --ramdisk $RAMDISK \
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