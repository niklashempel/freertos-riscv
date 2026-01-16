#include "basic_usage.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"

const static char *TAG = "deadline_miss";

static void periodic_task(void *arg) {
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 500; // ms
  BaseType_t xDeadlineMissed;
  TickType_t workDuration = 100; // ms

  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    // xTaskDelayUntil returns true if the task was delayed. As task is only
    // delayed if the deadline has not been missed.
    xDeadlineMissed =
        !xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(xFrequency));
    spin_idle(workDuration);
    workDuration += 100;
    ESP_LOGI(TAG, "Delayed: %d", xDeadlineMissed);
    if (xDeadlineMissed) {
      ESP_LOGI(TAG, "DEADLINE MISSED");
      vTaskDelete(NULL);
    }
  }
}

int comp_deadline_miss_entry_func(int argc, char **argv) {
  xTaskCreate(periodic_task, "periodic_task", 4096, NULL, TASK_PRIO_3, NULL);
  vTaskDelay(pdMS_TO_TICKS(4000));
  return 0;
}
