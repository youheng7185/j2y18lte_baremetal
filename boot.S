.text
.global _start
_start:
    /* ARM64 Linux Image header */
    b       real_start
    .long   0
    .quad   0x80000
    .quad   0x800000
    .quad   0
    .quad   0
    .quad   0
    .quad   0
    .long   0x644d5241
    .long   0

real_start:
    /* set up stack pointer, use safe area in DRAM below our load address */
    ldr     x30, =0x80070000
    mov     sp, x30
    b       main