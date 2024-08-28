.section ".text.boot"

.global _start

_start:
    // Set up stack pointer
    ldr x30, =stack_top
    mov sp, x30

    // Clear BSS
    ldr x1, =__bss_start
    ldr x2, =__bss_end
    sub x2, x2, x1
    cbz x2, 2f
1:  str xzr, [x1], #8
    sub x2, x2, #8
    cbnz x2, 1b
2:

    // Call kernel_main
    bl kernel_main

    // Halt if kernel_main returns
halt:
    wfe
    b halt

.section ".bss"
.align 16
stack_bottom:
.skip 16384 // 16 KB
stack_top:
