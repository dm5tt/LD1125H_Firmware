#include "main.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "setup.h"

#include "gd32f30x.h"


volatile uint16_t buffer[ADC_SAMPLES];



void vTask1(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    printf("\n\ra usart transmit test example!\n\r");
    vTaskDelay(1000);
  }
}

int main(void) {
  setup_systick_config();
  setup_pins();

  setup_uart();
  setup_dma();
  setup_adc();
  setup_timer();

  timer_enable(ADC_PWM_TMER);

  setvbuf(stdout, NULL, _IONBF, 0);

  xTaskCreate(vTask1,   // pointer to the task function
              "Task 1", // task name
              40,       // stack size in words
              NULL,     // stack parameter (not used here)
              1,        // priority
              NULL);    // task handle (not used)

  vTaskStartScheduler();

  while (1) {
  }
}
