#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

MKBOOTIMG="${MKBOOTIMG:-$SCRIPT_DIR/mkbootimg}"
DT="${DT:-$SCRIPT_DIR/twrp-3.7.0_9-0-j2y18lte.img-dt}"
KERNEL_PATH="${KERNEL_PATH:-$SCRIPT_DIR/u-boot-msm8917.Image}"
DTB_PATH="${DTB_PATH:-$SCRIPT_DIR/1/msm8917-samsung-j2y18lte.dtb}"
RAMDISK="${RAMDISK:-$SCRIPT_DIR/initramfs.cpio.gz}"
CMDLINE="${CMDLINE:-clk_ignore_unused pd_ignore_unused ignore_loglevel earlyprintk rdinit=/init deferred_probe_timeout=600}"

if [[ $# -gt 1 || ( $# -eq 1 && "$1" != "--flash" ) ]]; then
	printf 'Usage: %s [--flash]\n' "$0" >&2
	exit 2
fi

for input in "$MKBOOTIMG" "$DT" "$KERNEL_PATH" "$DTB_PATH" "$RAMDISK"; do
	if [[ ! -f "$input" ]]; then
		printf 'Missing input file: %s\n' "$input" >&2
		exit 1
	fi
done

echo "==> Cleaning previous wrapper build..."
make clean

echo "==> Building wrapped_kernel..."
make wrapped_kernel \
	CMDLINE="$CMDLINE" \
	KERNEL_PATH="$KERNEL_PATH" \
	DTB_PATH="$DTB_PATH"

echo "==> Packing boot.img..."
"$MKBOOTIMG" \
	--kernel wrapped_kernel \
	--ramdisk "$RAMDISK" \
	--dt "$DT" \
	--base 0x80000000 \
	--kernel_offset 0x00008000 \
	--ramdisk_offset 0x02000000 \
	--tags_offset 0x01e00000 \
	--board SRPQH31A000RU \
	--pagesize 2048 \
	-o boot.img

echo "==> Done! boot.img ready."

if [[ "${1:-}" == "--flash" ]]; then
	echo "==> Booting via fastboot..."
	fastboot boot boot.img
fi
