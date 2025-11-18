    .section .text
    .align 2

    # UART0 Base (from TRM Table 5.3-2)
    .equ UART0_BASE, 0x60000000

    # FIFO Register Offset (from TRM 27.6.1)
    .equ UART_FIFO, UART0_BASE + 0x0000

    .global hello_world
    .type hello_world, @function
hello_world:
    addi sp, sp, -16
    sw ra, 12(sp)

    la a0, hello_msg
    call print_string

    lw ra, 12(sp)
    addi sp, sp, 16
    ret


    .type print_string, @function
print_string:
    li a1, UART_FIFO          # FIFO address

print_loop:
    lb a2, 0(a0)              # Load next byte
    beqz a2, print_done       # End on null
    sb a2, 0(a1)              # Write to UART FIFO
    addi a0, a0, 1            # Advance pointer
    j print_loop

print_done:
    ret


    .section .rodata
hello_msg:
    .string "Hello, World!\n"


    .section .text
    .align 2

    .global multiply
    .type multiply, @function
multiply:
    mul a0, a0, a1
    ret
