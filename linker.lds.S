OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)

ENTRY(_start)

SECTIONS
{
    . = 0x80080000;

    .text   : { *(.text) }
    .rodata : { *(.rodata) }
    .data   : { *(.data) }

    .bss : {
        . = ALIGN(16);
        bss_start = .;
        *(.bss)
        . = ALIGN(16);
        bss_end = .;
    }

    . = ALIGN(16);
    . = . + 0x4000;
    stack_top = .;

    .dtb ALIGN(0x1000) : {
        dtb = .;
        *(.dtb)
    }

    .kernel ALIGN(0x1000) : {
        kernel = .;
        *(.kernel)
    }

    .initramfs ALIGN(0x1000) : {
        initramfs = .;
        *(.initramfs)
        initramfs_end = .;
    }
}