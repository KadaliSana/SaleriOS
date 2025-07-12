.section ".text.boot"

.globl __start

__start:
    // Set the stack pointer to a safe location right before the kernel image
    ldr x0, =__start
    mov sp, x0

    // Clear the BSS section
    ldr x1, =__bss_start
    ldr x2, =__bss_end
1:  
    cmp x1, x2              // Compare current address with the end of BSS
    b.ge 2f                 // If we've reached the end, branch to part 2
    str xzr, [x1], #8       // Store zero and increment address by 8 bytes
    b 1b                    // Loop back

2:  // Call the C kernel main function
    bl kmain

// Infinite loop in case kmain returns
halt:
    wfe
    b halt