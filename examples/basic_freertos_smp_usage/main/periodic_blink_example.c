#include "basic_freertos_smp_usage.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static volatile bool timed_out;
const static char *TAG = "periodic notify example";

void vFastTask(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(10); // 10ms

  while (!timed_out) {
    // Fast periodic work (100 Hz)
    ESP_LOGI(TAG, "fast_task");
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  ESP_LOGI(TAG, "fast_task finished");
  vTaskDelete(NULL);
}

void vSlowTask(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1000ms

  while (!timed_out) {
    // Slow periodic work (1 Hz)
    ESP_LOGI(TAG, "slow_task");
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  ESP_LOGI(TAG, "slow_task finished");
  vTaskDelete(NULL);
}

int comp_periodic_blink_entry_func(int argc, char **argv) {

  timed_out = false;
  xTaskCreate(vFastTask, "Fast", 2048, NULL, 5, NULL);
  xTaskCreate(vSlowTask, "Slow", 2048, NULL, 3, NULL);

  // time out and stop running after 5 seconds
  vTaskDelay(pdMS_TO_TICKS(COMP_LOOP_PERIOD));
  timed_out = true;
  // delay to let tasks finish the last loop
  vTaskDelay(500 / portTICK_PERIOD_MS);
  return 0;
}