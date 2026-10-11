# Create the directory structure
rm -rf initramfs_root
mkdir -p initramfs_root/{bin,dev,proc,sys,mnt,sysroot}

# Copy your busybox and make it executable
cp busybox-armv8l initramfs_root/bin/busybox
chmod +x initramfs_root/bin/busybox

# Symlink tools from busybox
for tool in sh mount mkdir switch_root mdev; do
  ln -s busybox initramfs_root/bin/$tool
done

# Write the init script
cat > initramfs_root/init << 'EOF'
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev

exec switch_root /sysroot /sbin/init
EOF
chmod +x initramfs_root/init

# Pack it
cd initramfs_root
find . | cpio -o -H newc | gzip -9 > ../initramfs.cpio.gz
cd ..

# Check the size
du -sh initramfs.cpio.gz