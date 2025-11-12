#include <inttypes.h>
#include <stdio.h>
#include "esp_clk_tree.h"
#include "esp_cpu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

uint32_t cct_ElapsedTimeMs(uint32_t start, uint32_t end, uint32_t cpu_hz) {
  uint32_t cycles = end - start;
  uint32_t cycles_per_ms = cpu_hz / 1000u;
  if (cycles_per_ms == 0) cycles_per_ms = 1;
  return (cycles + (cycles_per_ms / 2)) / cycles_per_ms; // "add half and divide" rounding technique
}

static const char *TAG_TM = "time_measure";

int comp_time_measurement_entry_func(int argc, char **argv) {
    uint32_t cpu_hz = 0;
    esp_err_t err = esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &cpu_hz);
    if (err != ESP_OK) {
        fprintf(stderr, "Unable to read CPU frequency\n");
        return -1;
    }
    printf("[%s] CPU freq: %" PRIu32 " Hz\n", TAG_TM, cpu_hz);
    
    esp_cpu_cycle_count_t start = esp_cpu_get_cycle_count();
    TickType_t xTimeOnEntering = xTaskGetTickCount();
    while (xTaskGetTickCount() - xTimeOnEntering < pdMS_TO_TICKS(2000))
    {
        // Do nothing
    }
    esp_cpu_cycle_count_t end = esp_cpu_get_cycle_count();

    uint32_t cycles_delta = (uint32_t)(end - start);
    uint32_t elapsed_time_ms = cct_ElapsedTimeMs((uint32_t)start, (uint32_t)end, cpu_hz);
    printf("[%s] Elapsed cycles: %" PRIu32 ", Elapsed time: %" PRIu32 " ms (expected ~2000)\n",
      TAG_TM, cycles_delta, elapsed_time_ms);
    return 0;
}