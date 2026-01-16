#pragma once

#include "freertos/FreeRTOS.h"

#define PROMPT_STR CONFIG_IDF_TARGET

typedef struct {
  uint32_t data_length;
  uint8_t data[128]; // Potential for overflow
} QueueMessage_t;

int comp_attack_entry_func(int argc, char **argv);
void vVictimTask(void *pvParameters);
void vAttackerTask(void *pvParameters);
void processMessage(QueueMessage_t *msg);
void maliciousFunction(void);
static void config_console(void);