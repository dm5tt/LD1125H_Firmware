#include "main.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#include "setup.h"

void vTask1(void *pvParameters) {

  for (;;) {
    printf("\n\ra usart transmit test example!\n\r");
    vTaskDelay(1000);
  }
}

int main(void) {
  setup_systick_config();
  setup_uart();

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
