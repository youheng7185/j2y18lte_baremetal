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
    /* load stack from linker symbol, not hardcoded address */
    ldr     x1, =stack_top
    and     x1, x1, #~15
    mov     sp, x1

    /* clear bss */
    ldr     x1, =bss_start
    ldr     x2, =bss_end
bss_loop:
    cmp     x1, x2
    b.hs    done_bss
    stp     xzr, xzr, [x1], #16
    b       bss_loop
done_bss:
    bl      main
hang:
    b       hang