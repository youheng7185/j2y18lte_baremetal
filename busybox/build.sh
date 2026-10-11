rm -rf initramfs_root
mkdir -p initramfs_root/{bin,dev,proc,sys,mnt,sysroot}

cp busybox-armv8l initramfs_root/bin/busybox
chmod +x initramfs_root/bin/busybox

for tool in sh mount mkdir switch_root mdev ls cat sleep setsid echo; do
  ln -s busybox initramfs_root/bin/$tool
done

# cat > initramfs_root/init << 'EOF'
# #!/bin/sh

# exec >/dev/console 2>&1 </dev/console

# echo "==> mounting proc"
# mount -t proc none /proc || echo "FAILED: proc"

# echo "==> mounting sys"
# mount -t sysfs none /sys || echo "FAILED: sys"

# echo "==> mounting devtmpfs"
# mount -t devtmpfs none /dev || echo "FAILED: devtmpfs"

# echo "==> /proc/partitions:"
# cat /proc/partitions

# echo "==> watchdog devices:"
# ls /dev/watchdog* 2>/dev/null

# echo "==> petting all watchdogs..."
# for wdog in /dev/watchdog /dev/watchdog0 /dev/watchdog1; do
#   echo 1 > $wdog 2>/dev/null && echo "pet $wdog"
# done

# echo "==> feed watchdog"
# echo 1 > /dev/watchdog 2>/dev/null || true

# echo "==> SHELL FAILED, petting watchdog and spinning..."
# count=0
# while true; do
#   echo 1 > /dev/watchdog0 2>/dev/null
#   echo 1 > /dev/watchdog1 2>/dev/null
#   sleep 1
#   count=$((count + 1))
#   echo "alive: ${count}s"
# done
# EOF
cat > initramfs_root/init << 'EOF'
#!/bin/sh

exec >/dev/console 2>&1 </dev/console

mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev

# Pet both watchdogs in background loop
(while true; do
  echo 1 > /dev/watchdog0 2>/dev/null
  echo 1 > /dev/watchdog1 2>/dev/null
  sleep 3
done) &

echo "==> /proc/interrupts:"
cat /proc/interrupts

echo "==> /proc/sys/kernel/watchdog:"
cat /proc/sys/kernel/watchdog 2>/dev/null

echo "==> dmesg watchdog lines:"
cat /proc/kmsg | grep -i "wdog\|watchdog\|bite\|bark\|reset\|rpm" &
KMSG_PID=$!
sleep 2
kill $KMSG_PID 2>/dev/null

count=0
while true; do
  sleep 1
  count=$((count + 1))
  echo "alive: ${count}s"
done
EOF

chmod +x initramfs_root/init

cd initramfs_root
find . | cpio -o -H newc | gzip -9 > ../initramfs.cpio.gz
cd ..

du -sh initramfs.cpio.gz