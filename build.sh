#!/bin/bash
set -e

MKBOOTIMG=./mkbootimg
ALPINE=1/alpine-minirootfs-3.24.1-aarch64.tar.gz
DT=twrp-3.7.0_9-0-j2y18lte.img-dt

echo "==> Building initramfs from Alpine tarball..."
rm -rf initramfs_root
mkdir -p initramfs_root
tar -xzf $ALPINE -C initramfs_root

cat > initramfs_root/init << 'EOF'
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev
echo "Alpine Linux on $(uname -m)"
exec /bin/sh
EOF
chmod +x initramfs_root/init

cd initramfs_root
find . | cpio -o -H newc | gzip > ../initramfs.cpio.gz
cd ..

echo "==> Cleaning..."
make clean

echo "==> Building wrapped_kernel..."
make wrapped_kernel \
    CMDLINE="clk_ignore_unused ignore_loglevel earlyprintk rdinit=/sbin/init" \
    KERNEL_PATH=/home/lapchong/arm_mpu/phones/bootloader/j2y18lte_baremetal/1/Image \
    DTB_PATH=/home/lapchong/arm_mpu/phones/bootloader/j2y18lte_baremetal/1/msm8917-samsung-j2y18lte.dtb

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