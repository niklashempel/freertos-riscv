# ESP32C6 CPU

The following information are taken from the [ESP32-C6 datasheet](https://documentation.espressif.com/esp32-c6_datasheet_en.pdf) and [technical reference manual](https://documentation.espressif.com/esp32-c6_technical_reference_manual_en.pdf#riscvcpu) in excerpts.

ESP32C6 actually consists of two processors: a low-power and a high-performance cpu.

## High-Performance CPU (HP)

The ESP-RISC-V CPU is a 32-bit core that is baed on the RISC-V instruction set architecture comprising base integer (I), multiplication/division (M), atomic (A) and compressed (C)
standard extensions.

Some of the features include:

- four-stage pipeline that supports an operating clock frequency up to 160 MHz
- branch target buffer (BTB) with static branch prediction
- user mode support along with interrupt delegation
- debug module with external debugger support over an industry-standard JTAG/USB port
- physical memory protection (PMP) and attributes (PMA) for up to 16 configurable regions

## Low-Power CPU (LP)

The ESP32-C6 Low-Power CPU (LP CPU) is a 32-bit processor based on the RISC-V instruction set architecture comprising integer (I), multiplication/division (M), atomic (A), and compressed (C) standard extensions. It is designed for ultra-low power consumption and is capable of staying powered on during Deep-sleep mode when the HP CPU is powered down.

Some of the features include:

- two-stage pipeline that supports a clock frequency of up to 20 MHz
- debug module with external debugger support over an industry-standard JTAG/USB port

The LP CPU is in sleep mode by default. It can stay powered on when the chip enters Deep-sleep mode and can access most peripherals and memories. It has two application scenarios:

- Power insensitive scenario: When the High-Performance CPU (HP CPU) is active, the LP CPU can assist the HP CPU with some speed- and efficiency-insensitive controls and computations.
- Power sensitive scenario: When the HP CPU is in the power-down state to save power, the LP CPU can be woken up to handle some external wake-up events.

![LP CPU Overview](images/LP%20CPU%20Overwie.png)
