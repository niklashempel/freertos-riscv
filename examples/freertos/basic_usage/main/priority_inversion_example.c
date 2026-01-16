#include "basic_usage.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

const static char *TAG = "priority_inversion";

static SemaphoreHandle_t semaphore;

// Low priority task that holds the semaphore
static void low_priority_task(void *pvParameters) {
  TickType_t start = xTaskGetTickCount();
  ESP_LOGI(TAG, "[tick=%u] Low: started", (unsigned)start);

  // Take the semaphore immediately
  xSemaphoreTake(semaphore, portMAX_DELAY);
  ESP_LOGI(TAG, "[tick=%u] Low: acquired semaphore (P1)",
           (unsigned)xTaskGetTickCount());

  // Do some work while holding the semaphore
  TickType_t end = xTaskGetTickCount() + pdMS_TO_TICKS(2500);
  UBaseType_t lastPrio = uxTaskPriorityGet(NULL);

  ESP_LOGI(TAG, "[tick=%u] Low: working with semaphore (P%u)...",
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

  ESP_LOGI(TAG, "[tick=%u] Low: work done, releasing semaphore",
           (unsigned)xTaskGetTickCount());
  xSemaphoreGive(semaphore);

  ESP_LOGI(TAG, "[tick=%u] Low: finished (back to P%u)",
           (unsigned)xTaskGetTickCount(), (unsigned)uxTaskPriorityGet(NULL));
  vTaskDelete(NULL);
}

// Medium priority task (no semaphore)
static void medium_priority_task(void *pvParameters) {
  TickType_t start = xTaskGetTickCount();
  ESP_LOGI(TAG, "[tick=%u] Medium: started", (unsigned)start);

  // Start shortly after Low to preempt Low until High blocks on the semaphore
  vTaskDelay(pdMS_TO_TICKS(50));

  ESP_LOGI(TAG, "[tick=%u] Medium: doing work (P2, no semaphore needed)...",
           (unsigned)xTaskGetTickCount());

  spin_idle(3000);

  ESP_LOGI(TAG, "[tick=%u] Medium: work done, finished",
           (unsigned)xTaskGetTickCount());
  vTaskDelete(NULL);
}

// High priority task that needs the semaphore
static void high_priority_task(void *pvParameters) {
  TickType_t start = xTaskGetTickCount();
  ESP_LOGI(TAG, "[tick=%u] High: started", (unsigned)start);

  // Let Medium preempt Low first, then block on the semaphore
  vTaskDelay(pdMS_TO_TICKS(200));

  ESP_LOGI(TAG, "[tick=%u] High: waiting for semaphore (P3)...",
           (unsigned)xTaskGetTickCount());
  vTaskDelay(pdMS_TO_TICKS(10)); // give UART time to flush

  xSemaphoreTake(semaphore, portMAX_DELAY);
  ESP_LOGI(TAG, "[tick=%u] High: acquired semaphore!",
           (unsigned)xTaskGetTickCount());

  ESP_LOGI(TAG, "[tick=%u] High: doing critical work with semaphore...",
           (unsigned)xTaskGetTickCount());
  spin_idle(100);

  ESP_LOGI(TAG, "[tick=%u] High: work done, releasing semaphore",
           (unsigned)xTaskGetTickCount());
  xSemaphoreGive(semaphore);

  ESP_LOGI(TAG, "[tick=%u] High: finished", (unsigned)xTaskGetTickCount());
  vTaskDelete(NULL);
}

int comp_priority_inversion_entry_func(int argc, char **argv) {
  semaphore = xSemaphoreCreateBinary();
  if (semaphore == NULL) {
    ESP_LOGE(TAG, "Failed to create binary semaphore");
    return -1;
  }
  xSemaphoreGive(semaphore);

  ESP_LOGI(TAG, "[tick=%u] Creating tasks...", (unsigned)xTaskGetTickCount());

  // Create tasks with different priorities (1=low, 2=medium, 3=high)
  xTaskCreate(low_priority_task, "low_prio", 4096, NULL, 1, NULL);
  xTaskCreate(medium_priority_task, "med_prio", 4096, NULL, 2, NULL);
  xTaskCreate(high_priority_task, "high_prio", 4096, NULL, 3, NULL);

  vTaskDelay(pdMS_TO_TICKS(5000));

  vSemaphoreDelete(semaphore);
  return 0;
}