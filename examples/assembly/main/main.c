#include "esp_log.h"

static const char *TAG = "assembly";

// Declare external assembly functions from asm_functions.S
extern void hello_world(void);
extern uint32_t multiply(uint32_t a, uint32_t b);

uint32_t add(uint32_t a, uint32_t b) {
  uint32_t result;
  __asm__ volatile("add %0, %1, %2" // result = a + b
                   : "=r"(result)   // output: result in register
                   : "r"(a), "r"(b) // inputs: a and b in registers
  );
  return result;
}

uint32_t atomic_add(uint32_t *ptr, uint32_t value) {
  uint32_t result;
  // atomic fetch-add: returns old *ptr and adds value
  // AMOADD.W atomically loads a data value from the address in 'ptr', places
  // the value into register 'result', then adds the loaded value and the
  // original value in 'value', then stores the result back to the address in
  // 'result'.
  __asm__ volatile("amoadd.w %0, %2, (%1)\n" // atomically add word
                   : "=r"(result)
                   : "r"(ptr), "r"(value)
                   : "memory");
  return result;
}

int app_main(void) {
  hello_world();

  uint32_t sum = add(5, 3);
  ESP_LOGI(TAG, "Add: 5 + 3 = %" PRIu32 "", sum);

  uint32_t product = multiply(7, 6);
  ESP_LOGI(TAG, "Multiply: 7 * 6 = %" PRIu32 "", product);

  uint32_t summand = 10;
  sum = atomic_add(&summand, 5);
  ESP_LOGI(TAG, "Atomic add: %" PRIu32 " + 5 = %" PRIu32 "", sum, summand);

  return 0;
}