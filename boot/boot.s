.section .multiboot, "a"
.align 4
.long 0x1BADB002
.long 0
.long 0xE4524FFE

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:


.section .text
.global _start
.type _start, @function

_start:
    mov $stack_top, %esp
    call kernel_main
    
halt:
    cli
    hlt
    jmp halt