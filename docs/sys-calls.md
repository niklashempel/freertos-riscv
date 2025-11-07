# FreeRTOS Kernel API Overview

## Table of Contents

1. [Glossary](#glossary)
2. [Task Management](#task-management)
3. [Kernel Control](#kernel-control)
4. [Task Notifications](#task-notifications)
5. [Queue Management](#queue-management)
6. [Queue Sets](#queue-sets)
7. [Semaphores](#semaphores)
8. [Software Timers](#software-timers)
9. [Event Groups](#event-groups)
10. [Stream Buffers](#stream-buffers)
11. [Message Buffers](#message-buffers)
12. [Co-routines (Legacy)](#co-routines-legacy)

## Content

## Glossary

- **ISR** - Interrupt Service Routine - A function that executes in response to a hardware interrupt
- **MPU** - Memory Protection Unit - Hardware that enforces memory access restrictions
- **RTOS** - Real-Time Operating System
- **TCB** - Task Control Block - Data structure that holds information about a task
- **Tick** - The periodic interrupt that drives the RTOS scheduler (typically 1ms-10ms)
- **Context Switch** - The process of saving one task's state and restoring another's
- **Priority Inheritance** - Mechanism where a low-priority task temporarily inherits higher priority to prevent priority inversion
- **Mutex** - Mutual Exclusion semaphore used to protect shared resources
- **Semaphore** - Synchronization primitive used for task coordination
- **Queue** - FIFO data structure for passing messages between tasks
- **Blocking** - When a task waits (without consuming CPU) for an event or resource
- **Critical Section** - Code region where interrupts are disabled to protect shared data
- **Daemon Task** - Background task that provides services (e.g., timer daemon)
- **Idle Task** - Lowest priority task that runs when no other tasks are ready

## Task Management

### Task Creation and Deletion

- **xTaskCreate** - Create a new task using dynamically allocated stack and TCB
- **xTaskCreateStatic** - Create a new task using statically allocated stack and TCB
- **xTaskCreateRestrictedStatic** - Create a new task with MPU restrictions (ARM Cortex-M only)
- **vTaskDelete** - Delete a task, freeing its allocated memory

### Task Scheduling Control

- **vTaskDelay** - Delay a task for a specified number of ticks (relative delay)
- **vTaskDelayUntil** - Delay a task until a specified absolute time (periodic execution)
- **xTaskDelayUntil** - Same as vTaskDelayUntil but returns a value indicating if deadline was missed
- **vTaskSuspend** - Suspend a task (task will not run until resumed)
- **vTaskResume** - Resume a suspended task
- **xTaskResumeFromISR** - Resume a suspended task from an ISR
- **xTaskAbortDelay** - Force a task out of the Blocked state

### Task Priority Management

- **uxTaskPriorityGet** - Query a task's priority
- **uxTaskPriorityGetFromISR** - Query a task's priority from an ISR
- **uxTaskBasePriorityGet** - Get a task's base priority (before any inheritance)
- **uxTaskBasePriorityGetFromISR** - Get a task's base priority from an ISR
- **vTaskPrioritySet** - Change a task's priority at runtime

### Task Information and Diagnostics

- **xTaskGetCurrentTaskHandle** - Get handle of currently running task
- **xTaskGetIdleTaskHandle** - Get handle of the idle task
- **uxTaskGetStackHighWaterMark** - Get minimum free stack space (for stack overflow detection)
- **eTaskGetState** - Get the current state of a task (Running, Ready, Blocked, Suspended, Deleted)
- **pcTaskGetName** - Get the name of a task from its handle
- **xTaskGetHandle** - Get a task's handle from its name
- **uxTaskGetNumberOfTasks** - Get the number of tasks in the system
- **vTaskGetInfo** - Get detailed information about a task
- **uxTaskGetSystemState** - Get status information for all tasks

### Task Timing and Statistics

- **xTaskGetTickCount** - Get the current tick count
- **xTaskGetTickCountFromISR** - Get the current tick count from an ISR
- **xTaskGetSchedulerState** - Query if scheduler is running, suspended, or not started
- **vTaskGetRunTimeStats** - Get formatted run-time statistics for all tasks
- **ulTaskGetRunTimeCounter** - Get the total run time for a task
- **ulTaskGetIdleRunTimeCounter** - Get the idle task run time

### Task Utilities

- **vTaskList** - Generate a formatted table of task status information
- **vTaskSetApplicationTaskTag** - Set a tag value for a task
- **xTaskGetApplicationTaskTag** - Get a task's tag value
- **xTaskCallApplicationTaskHook** - Call a hook function with a task's tag value
- **pvTaskGetThreadLocalStoragePointer** - Get a thread local storage pointer
- **vTaskSetThreadLocalStoragePointer** - Set a thread local storage pointer
- **vTaskSetTimeOutState** - Initialize a timeout structure
- **xTaskCheckForTimeOut** - Check if a timeout has expired

## Kernel Control

### Scheduler Control

- **vTaskStartScheduler** - Start the RTOS scheduler (never returns if successful)
- **vTaskEndScheduler** - Stop the RTOS scheduler
- **vTaskSuspendAll** - Suspend the scheduler (tasks don't switch but ISRs still execute)
- **xTaskResumeAll** - Resume the scheduler after suspension
- **vTaskStepTick** - Manually advance the tick count (used when scheduler is suspended)

### Critical Sections and Interrupt Management

- **taskYIELD** - Force a context switch to the highest priority ready task
- **taskENTER_CRITICAL** - Enter a critical section (disables interrupts)
- **taskEXIT_CRITICAL** - Exit a critical section (re-enables interrupts)
- **taskENTER_CRITICAL_FROM_ISR** - Enter a critical section from an ISR
- **taskEXIT_CRITICAL_FROM_ISR** - Exit a critical section from an ISR
- **taskDISABLE_INTERRUPTS** - Disable interrupts up to a configured priority
- **taskENABLE_INTERRUPTS** - Re-enable interrupts


## Task Notifications

Task notifications provide a light-weight alternative to semaphores, queues, and event groups for task-to-task and ISR-to-task synchronization.

- **xTaskNotifyGive** / **xTaskNotifyGiveIndexed** - Send a notification to a task (increment its notification value)
- **vTaskNotifyGiveFromISR** / **vTaskNotifyGiveIndexedFromISR** - Send a notification from an ISR
- **ulTaskNotifyTake** / **ulTaskNotifyTakeIndexed** - Wait for a notification (with optional clear/decrement)
- **xTaskNotify** / **xTaskNotifyIndexed** - Send a notification with action (set bits, increment, or overwrite)
- **xTaskNotifyAndQuery** / **xTaskNotifyAndQueryIndexed** - Send notification and query previous value
- **xTaskNotifyAndQueryFromISR** / **xTaskNotifyAndQueryFromISRIndexed** - Send notification from ISR and query previous value
- **xTaskNotifyFromISR** / **xTaskNotifyFromISRIndexed** - Send notification from ISR
- **xTaskNotifyWait** / **xTaskNotifyWaitIndexed** - Wait for a notification with bit clearing options
- **xTaskNotifyStateClear** / **xTaskNotifyStateClearIndexed** - Clear a task's notification state
- **ulTaskNotifyValueClear** / **ulTaskNotifyValueClearIndexed** - Clear specific bits in a task's notification value

## Queue Management

Queues are the primary form of inter-task communication. They allow data to be sent between tasks and ISRs safely.

### Queue Creation and Configuration

- **xQueueCreate** - Create a queue using dynamically allocated memory
- **xQueueCreateStatic** - Create a queue using statically allocated memory
- **xQueueGetStaticBuffers** - Retrieve the buffers used by a statically allocated queue
- **vQueueDelete** - Delete a queue and free its memory

### Queue Operations

- **xQueueSend** - Send an item to the back of the queue (blocks if full)
- **xQueueSendToBack** - Explicitly send to the back of the queue
- **xQueueSendToFront** - Send an item to the front of the queue (priority send)
- **xQueueReceive** - Receive an item from the queue (removes it from queue)
- **xQueuePeek** - Receive an item without removing it from the queue
- **xQueueOverwrite** - Overwrite the queue (for queues of length 1)

### Queue Operations from ISR

- **xQueueSendFromISR** - Send an item from an ISR
- **xQueueSendToBackFromISR** - Send to back from an ISR
- **xQueueSendToFrontFromISR** - Send to front from an ISR
- **xQueueReceiveFromISR** - Receive from an ISR
- **xQueuePeekFromISR** - Peek from an ISR
- **xQueueOverwriteFromISR** - Overwrite from an ISR

### Queue Status and Utilities

- **uxQueueMessagesWaiting** - Query number of items in the queue
- **uxQueueMessagesWaitingFromISR** - Query number of items from an ISR
- **uxQueueSpacesAvailable** - Query available space in the queue
- **xQueueIsQueueEmptyFromISR** - Check if queue is empty (from ISR)
- **xQueueIsQueueFullFromISR** - Check if queue is full (from ISR)
- **xQueueReset** - Reset a queue to its empty state
- **pcQueueGetName** - Get the name assigned to a queue
- **vQueueAddToRegistry** - Add a queue to the queue registry (for debugging)
- **vQueueUnregisterQueue** - Remove a queue from the registry

## Queue Sets

Queue sets allow a task to block on reading from multiple queues or semaphores simultaneously.

- **xQueueCreateSet** - Create a queue set using dynamically allocated memory
- **xQueueCreateSetStatic** - Create a queue set using statically allocated memory
- **xQueueAddToSet** - Add a queue or semaphore to a queue set
- **xQueueRemoveFromSet** - Remove a queue or semaphore from a queue set
- **xQueueSelectFromSet** - Block waiting on multiple queues/semaphores in a set
- **xQueueSelectFromSetFromISR** - Non-blocking select from a queue set (from ISR)

## Semaphores

Semaphores are used for synchronization and mutual exclusion. Binary semaphores are ideal for synchronization, mutexes for resource protection, and counting semaphores for resource management.

### Semaphore Creation

- **xSemaphoreCreateBinary** - Create a binary semaphore (dynamically allocated)
- **xSemaphoreCreateBinaryStatic** - Create a binary semaphore (statically allocated)
- **xSemaphoreCreateCounting** - Create a counting semaphore (dynamically allocated)
- **xSemaphoreCreateCountingStatic** - Create a counting semaphore (statically allocated)
- **xSemaphoreCreateMutex** - Create a mutex (with priority inheritance)
- **xSemaphoreCreateMutexStatic** - Create a mutex (statically allocated)
- **xSemaphoreCreateRecursiveMutex** - Create a recursive mutex (can be taken multiple times by same task)
- **xSemaphoreCreateRecursiveMutexStatic** - Create a recursive mutex (statically allocated)
- **vSemaphoreDelete** - Delete a semaphore and free its memory

### Semaphore Operations

- **xSemaphoreTake** - Take/acquire a semaphore (blocks if unavailable)
- **xSemaphoreGive** - Give/release a semaphore
- **xSemaphoreTakeRecursive** - Take a recursive mutex
- **xSemaphoreGiveRecursive** - Give a recursive mutex
- **xSemaphoreTakeFromISR** - Take a semaphore from an ISR (must not block)
- **xSemaphoreGiveFromISR** - Give a semaphore from an ISR

### Semaphore Utilities

- **uxSemaphoreGetCount** - Query the count value of a semaphore
- **xSemaphoreGetMutexHolder** - Get the task handle of the mutex holder


## Software Timers

Software timers allow functions to be called at a set time in the future. They run in the context of the timer daemon task.

### Timer Creation and Configuration

- **xTimerCreate** - Create a software timer (dynamically allocated)
- **xTimerCreateStatic** - Create a software timer (statically allocated)
- **pvTimerGetTimerID** - Get the ID assigned to a timer
- **vTimerSetTimerID** - Set the ID assigned to a timer
- **pcTimerGetName** - Get the name of a timer
- **xTimerGetPeriod** - Query the period of a timer
- **xTimerGetExpiryTime** - Get the time at which the timer will expire
- **xTimerGetReloadMode** - Query if timer is one-shot or auto-reload
- **vTimerSetReloadMode** - Set the reload mode of a timer

### Timer Control

- **xTimerStart** - Start a timer (from a task)
- **xTimerStop** - Stop a timer (from a task)
- **xTimerReset** - Reset a timer (restart from current time)
- **xTimerChangePeriod** - Change the period of a timer
- **xTimerDelete** - Delete a timer
- **xTimerIsTimerActive** - Query if a timer is active

### Timer Control from ISR

- **xTimerStartFromISR** - Start a timer from an ISR
- **xTimerStopFromISR** - Stop a timer from an ISR
- **xTimerResetFromISR** - Reset a timer from an ISR
- **xTimerChangePeriodFromISR** - Change timer period from an ISR

### Timer Utilities

- **xTimerGetTimerDaemonTaskHandle** - Get the handle of the timer daemon task
- **xTimerPendFunctionCall** - Execute a function in the timer daemon task context
- **xTimerPendFunctionCallFromISR** - Execute a function in the timer daemon task (from ISR)

## Event Groups

Event groups provide a way for tasks to synchronize with multiple events or wait for multiple conditions. Each event group has 24 usable event bits (8 bits reserved).

> **TIP:** [Task Notifications](#task-notifications) can provide a light-weight alternative to event groups in many situations.

### Event Group Creation

- **xEventGroupCreate** - Create an event group (dynamically allocated)
- **xEventGroupCreateStatic** - Create an event group (statically allocated)
- **xEventGroupGetStaticBuffer** - Retrieve the buffer used by a static event group
- **vEventGroupDelete** - Delete an event group

### Event Group Operations

- **xEventGroupSetBits** - Set bits in an event group (from a task)
- **xEventGroupSetBitsFromISR** - Set bits in an event group (from an ISR)
- **xEventGroupClearBits** - Clear bits in an event group (from a task)
- **xEventGroupClearBitsFromISR** - Clear bits in an event group (from an ISR)
- **xEventGroupGetBits** - Query the current value of event bits
- **xEventGroupGetBitsFromISR** - Query event bits from an ISR
- **xEventGroupWaitBits** - Wait for a combination of event bits to be set
- **xEventGroupSync** - Synchronize multiple tasks using event bits (set bits and wait)

## Stream Buffers

Stream buffers allow a stream of bytes to be passed from an interrupt to a task, or from one task to another. Optimized for single reader and single writer.

### Stream Buffer Creation

- **xStreamBufferCreate** - Create a stream buffer (dynamically allocated)
- **xStreamBufferCreateStatic** - Create a stream buffer (statically allocated)
- **xStreamBufferGetStaticBuffers** - Retrieve buffers used by a static stream buffer
- **vStreamBufferDelete** - Delete a stream buffer

### Stream Buffer Operations

- **xStreamBufferSend** - Send bytes to a stream buffer (from a task)
- **xStreamBufferSendFromISR** - Send bytes to a stream buffer (from an ISR)
- **xStreamBufferReceive** - Receive bytes from a stream buffer (from a task)
- **xStreamBufferReceiveFromISR** - Receive bytes from a stream buffer (from an ISR)

### Stream Buffer Status and Control

- **xStreamBufferBytesAvailable** - Query the number of bytes in the buffer
- **xStreamBufferSpacesAvailable** - Query the free space in the buffer
- **xStreamBufferIsEmpty** - Check if the stream buffer is empty
- **xStreamBufferIsFull** - Check if the stream buffer is full
- **xStreamBufferSetTriggerLevel** - Set the trigger level for unblocking
- **xStreamBufferReset** - Reset a stream buffer to empty state
- **xStreamBufferResetFromISR** - Reset a stream buffer from an ISR
- **uxStreamBufferGetStreamBufferNotificationIndex** - Get the notification index used
- **vStreamBufferSetStreamBufferNotificationIndex** - Set the notification index to use

## Message Buffers

Message buffers are built on top of stream buffers and pass variable-length discrete messages rather than a continuous stream. Each message has a 4-byte length header.

### Message Buffer Creation

- **xMessageBufferCreate** - Create a message buffer (dynamically allocated)
- **xMessageBufferCreateStatic** - Create a message buffer (statically allocated)
- **xMessageBufferGetStaticBuffers** - Retrieve buffers used by a static message buffer
- **vMessageBufferDelete** - Delete a message buffer

### Message Buffer Operations

- **xMessageBufferSend** - Send a message to a message buffer (from a task)
- **xMessageBufferSendFromISR** - Send a message to a message buffer (from an ISR)
- **xMessageBufferReceive** - Receive a message from a message buffer (from a task)
- **xMessageBufferReceiveFromISR** - Receive a message from a message buffer (from an ISR)

### Message Buffer Status and Control

- **xMessageBufferSpacesAvailable** - Query the free space in the buffer
- **xMessageBufferIsEmpty** - Check if the message buffer is empty
- **xMessageBufferIsFull** - Check if the message buffer is full
- **xMessageBufferReset** - Reset a message buffer to empty state
- **xMessageBufferResetFromISR** - Reset a message buffer from an ISR

## Co-routines (Legacy)

Co-routines are a legacy feature intended for very small microcontrollers. For new designs, use tasks instead.

- **xCoRoutineCreate** - Create a co-routine
- **vCoRoutineSchedule** - Schedule co-routines for execution
- **crDELAY** - Delay a co-routine for a specified number of ticks
- **crQUEUE_SEND** - Send to a queue from a co-routine
- **crQUEUE_RECEIVE** - Receive from a queue in a co-routine
- **crQUEUE_SEND_FROM_ISR** - Send to a queue from an ISR (for co-routines)
- **crQUEUE_RECEIVE_FROM_ISR** - Receive from a queue in an ISR (for co-routines)
- **CoRoutineHandle_t** - Type used to reference a co-routine
