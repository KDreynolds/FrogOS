.section ".text.boot"

.global _start

.section ".text.boot"

.global _start

_start:
    // Debug output - write 'A' to UART
    mov x0, #0x09000000  // UART base address
    mov w1, #65  // ASCII 'A'
    strb w1, [x0]

    // Set up stack pointer
    ldr x30, =stack_top
    mov sp, x30

        // Debug output - write 'B' to UART
    mov w1, #66  // ASCII 'B'
    strb w1, [x0]

        // Clear BSS
    ldr x1, =__bss_start
    ldr x2, =__bss_end
    
    // Debug output - write bss_start address
    mov x3, x1
    mov w4, #48  // ASCII '0'
1:  and w5, w3, #0xF
    add w5, w5, w4
    strb w5, [x0]
    lsr x3, x3, #4
    cbnz x3, 1b

    // Debug output - write space
    mov w4, #32  // ASCII space
    strb w4, [x0]

    // Debug output - write bss_end address
    mov x3, x2
    mov w4, #48  // ASCII '0'
1:  and w5, w3, #0xF
    add w5, w5, w4
    strb w5, [x0]
    lsr x3, x3, #4
    cbnz x3, 1b

    // Calculate BSS size
    sub x2, x2, x1
    
    // Debug output - write '1' to UART
    mov w3, #49  // ASCII '1'
    strb w3, [x0]
    
    cbz x2, 2f
    
    // Debug output - write '2' to UART
    mov w3, #50  // ASCII '2'
    strb w3, [x0]

    // Try to clear just the first 8 bytes
    str xzr, [x1], #8

    // Debug output - write '3' to UART
    mov w3, #51  // ASCII '3'
    strb w3, [x0]

2:
    // Debug output - write 'C' to UART
    mov w3, #67  // ASCII 'C'
    strb w3, [x0]

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
