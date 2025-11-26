    .section .text                          ; following code goes to .text section (executable code)
    .align 2                                ; align to 2^2-byte boundary

    .equ UART0_BASE, 0x60000000             ; values taken from technical reference manual
    .equ UART_FIFO, UART0_BASE + 0x0000

    .global hello_world                     ; make hello_world visible outside this file, e.g. our C code
    .type hello_world, @function            ; declare function type
hello_world:
    addi sp, sp, -16                        ; allocate 16 bytes on the stack by decrementing the stack pointer
    sw ra, 12(sp)                           ; save return address

    la a0, hello_msg                        ; load address of hello_msg into a0
    call print_string

    lw ra, 12(sp)                           ; restore return address
    addi sp, sp, 16                         ; deallocate 16 bytes from the stack by incrementing the stack pointer
    ret

                                            ; no global thus making it a private function
    .type print_string, @function           ; declare function type
print_string:
    li a1, UART_FIFO                        ; FIFO address

print_loop:
    lb a2, 0(a0)                            ; load next byte 
    beqz a2, print_done                     ; branch if equal to zero: end on 0 (null terminator)
    sb a2, 0(a1)                            ; write to UART FIFO
    addi a0, a0, 1                          ; advance pointer to next byte (character)
    j print_loop

print_done:
    ret


    .section .rodata                        ; read-only data section
hello_msg:
    .string "Hello, World!\n"


    .section .text                          ; switch back to code section
    .align 2

    .global multiply
    .type multiply, @function               ; define multiply function
multiply:
    mul a0, a0, a1                          ; a0 = a0 * a1, result in a0
    ret
