#include "gd32f30x.h"

/* retarget the gcc's C library printf function to the USART */
#include  <errno.h>
#include  <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO
int _write(int file, char *data, int len)
{
   if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
   {
      errno = EBADF;
      return -1;
   }

   // arbitrary timeout 1000
    for(int i=0; i < len; i++) {
        usart_data_transmit(USART0, (uint8_t)data[i]);
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    }

   // return # of bytes written - as best we can tell
   return len;
}

void setup_systick_config(void) {
  /* setup systick timer for 1000Hz interrupts */
  if (SysTick_Config(SystemCoreClock / 1000U)) {
    /* capture error */
    while (1) {
    }
  }
  /* configure the systick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x00U);
}


void setup_uart(void) {
  /* enable GPIO clock */
  rcu_periph_clock_enable(RCU_GPIOA);

  /* enable USART clock */
  rcu_periph_clock_enable(RCU_USART0);

  /* connect port to USARTx_Tx */
  gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

  /* connect port to USARTx_Rx */
  gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

  /* USART configure */
  usart_deinit(USART0);
  usart_baudrate_set(USART0, 4000000U);
  usart_receive_config(USART0, USART_RECEIVE_ENABLE);
  usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
  usart_enable(USART0);    
}