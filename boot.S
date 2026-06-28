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
    /* ldr (literal pool) has unlimited range unlike adr (±1MB) */
    ldr     x0, =dtb
    ldr     x1, =kernel
    bl      main

hang:
    b       hang

.global boot_kernel
boot_kernel:
    /* x0=dtb, x1=0, x2=0, x3=0, x4=kernel entry — matches C signature */
    br      x4