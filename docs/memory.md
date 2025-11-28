# Memory model

## MMU

Our system, ESP32C6, has a Memory management unit (MMU), that can do translation between physical and virtual memory addresses.
Though this does not include a full MMU capability, e.g. task isolation or paging.

## Shared memory

Since FreeRTOS does support MPU only for a handful of microcontrollers out of the box, and ESP32C6 is not one of them, there is no separate address space for each task. This means that there is no necessity for a shared memory mechanism since all tasks use the same memory anyway.
