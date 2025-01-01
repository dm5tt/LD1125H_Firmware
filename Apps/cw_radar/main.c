#include "main.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "setup.h"

#include "gd32f30x.h"

volatile uint16_t buffer[ADC_SAMPLES] = {0};


/* Task nor RTOS really needed here. Did it anyway because it was cheaply adapted from hello_world */
void vTask1(void *pvParameters) {
  (void)pvParameters;

  /* Set DAC to a static value */
  dac_data_set(DAC0, DAC_OUT0, DAC_ALIGN_12B_R, 2000);
  dac_software_trigger_enable(DAC0, DAC_OUT0);

  /* Everything happens in DMA and interrupts. Do nothing here.*/
  for (;;) {
    vTaskDelay(100);
  }
}

int main(void) {
  setup_systick_config();
  setup_pins();

  setup_uart();
  setup_adc();
  setup_dac();
  setup_timer();
  setup_dma();

  /* That's very ugly - but we use this printf() as a preamble within our Python application */
  printf("XXXXXXX");  
  timer_enable(ADC_PWM_TMER);

  setvbuf(stdout, NULL, _IONBF, 0);

  xTaskCreate(vTask1,   // pointer to the task function
              "Task 1", // task name
              512,      // stack size in words
              NULL,     // stack parameter (not used here)
              1,        // priority
              NULL);    // task handle (not used)

  vTaskStartScheduler();

  while (1) {
  }
}
