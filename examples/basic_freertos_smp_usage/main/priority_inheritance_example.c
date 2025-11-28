#include "basic_freertos_smp_usage.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

const static char *TAG = "priority_inheritance";

static SemaphoreHandle_t mutex;

// Low priority task that holds the mutex
static void low_priority_task(void *pvParameters) {
  TickType_t start = xTaskGetTickCount();
  ESP_LOGI(TAG, "[tick=%u] Low: started", (unsigned)start);

  // Take the mutex immediately
  xSemaphoreTake(mutex, portMAX_DELAY);
  ESP_LOGI(TAG, "[tick=%u] Low: acquired mutex (P1)",
           (unsigned)xTaskGetTickCount());

  // Do some work while holding the mutex
  TickType_t end = xTaskGetTickCount() + pdMS_TO_TICKS(2500);
  UBaseType_t lastPrio = uxTaskPriorityGet(NULL);

  ESP_LOGI(TAG, "[tick=%u] Low: working with mutex (P%u)...",
           (unsigned)xTaskGetTickCount(), (unsigned)lastPrio);

  while (xTaskGetTickCount() < end) {
    UBaseType_t currentPrio = uxTaskPriorityGet(NULL);
    if (currentPrio != lastPrio) {
      ESP_LOGI(TAG, "[tick=%u] Low: priority inherited P%u -> P%u",
               (unsigned)xTaskGetTickCount(), (unsigned)lastPrio,
               (unsigned)currentPrio);
      lastPrio = currentPrio;
    }
  }

  ESP_LOGI(TAG, "[tick=%u] Low: work done, releasing mutex",
           (unsigned)xTaskGetTickCount());
  xSemaphoreGive(mutex);

  ESP_LOGI(TAG, "[tick=%u] Low: finished (back to P%u)",
           (unsigned)xTaskGetTickCount(), (unsigned)uxTaskPriorityGet(NULL));
  vTaskDelete(NULL);
}

// Medium priority task (no mutex)
static void medium_priority_task(void *pvParameters) {
  TickType_t start = xTaskGetTickCount();
  ESP_LOGI(TAG, "[tick=%u] Medium: started", (unsigned)start);

  // Start shortly after Low to preempt Low until High blocks on the mutex
  vTaskDelay(pdMS_TO_TICKS(50));

  ESP_LOGI(TAG, "[tick=%u] Medium: doing work (P2, no mutex needed)...",
           (unsigned)xTaskGetTickCount());

  vTaskDelay(pdMS_TO_TICKS(3000));

  ESP_LOGI(TAG, "[tick=%u] Medium: work done, finished",
           (unsigned)xTaskGetTickCount());
  vTaskDelete(NULL);
}

// High priority task that needs the mutex
static void high_priority_task(void *pvParameters) {
  TickType_t start = xTaskGetTickCount();
  ESP_LOGI(TAG, "[tick=%u] High: started", (unsigned)start);

  // Let Medium preempt Low first, then block on the mutex
  vTaskDelay(pdMS_TO_TICKS(200));

  ESP_LOGI(TAG, "[tick=%u] High: waiting for mutex (P3)...",
           (unsigned)xTaskGetTickCount());
  vTaskDelay(pdMS_TO_TICKS(10)); // give UART time to flush

  xSemaphoreTake(mutex, portMAX_DELAY);
  ESP_LOGI(TAG, "[tick=%u] High: acquired mutex!",
           (unsigned)xTaskGetTickCount());

  ESP_LOGI(TAG, "[tick=%u] High: doing critical work with mutex...",
           (unsigned)xTaskGetTickCount());
  vTaskDelay(pdMS_TO_TICKS(100));

  ESP_LOGI(TAG, "[tick=%u] High: work done, releasing mutex",
           (unsigned)xTaskGetTickCount());
  xSemaphoreGive(mutex);

  ESP_LOGI(TAG, "[tick=%u] High: finished", (unsigned)xTaskGetTickCount());
  vTaskDelete(NULL);
}

int comp_priority_inheritance_entry_func(int argc, char **argv) {
  mutex = xSemaphoreCreateMutex();
  if (mutex == NULL) {
    ESP_LOGE(TAG, "Failed to create mutex");
    return -1;
  }

  ESP_LOGI(TAG, "[tick=%u] Creating tasks...", (unsigned)xTaskGetTickCount());

  // Create tasks with different priorities (1=low, 2=medium, 3=high)
  xTaskCreate(low_priority_task, "low_prio", 4096, NULL, 1, NULL);
  xTaskCreate(medium_priority_task, "med_prio", 4096, NULL, 2, NULL);
  xTaskCreate(high_priority_task, "high_prio", 4096, NULL, 3, NULL);

  vTaskDelay(pdMS_TO_TICKS(5000));

  vSemaphoreDelete(mutex);
  return 0;
}