#include "main.h"
#include "gd32f30x.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#define LEDPORT GPIOB
#define LEDPIN GPIO_PIN_2
#define LED_CLOCK RCU_GPIOB

void systick_config(void);

void vTask1(void *pvParameters) {
  const char *pcTaskName = "Task 1 is running\n";
  volatile unsigned long ul;

  for (;;) {
    vTaskDelay(1000);
  }
}

int main(void) {
  systick_config;

  rcu_periph_clock_enable(LED_CLOCK);

  gpio_init(LEDPORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LEDPIN);

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

volatile static uint32_t delay;

void systick_config(void) {
  /* setup systick timer for 1000Hz interrupts */
  if (SysTick_Config(SystemCoreClock / 1000U)) {
    /* capture error */
    while (1) {
    }
  }
  /* configure the systick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x00U);
}
