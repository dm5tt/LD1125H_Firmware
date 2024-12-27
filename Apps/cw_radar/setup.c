#include "setup.h"

#include "gd32f30x.h"

/* retarget the gcc's C library printf function to the USART */
#include <errno.h>
#include <sys/unistd.h>

int _write(int file, char *data, int len) {
  if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
    errno = EBADF;
    return -1;
  }

  for (int i = 0; i < len; i++) {
    usart_data_transmit(USART0, (uint8_t)data[i]);
    while (RESET == usart_flag_get(USART0, USART_FLAG_TBE))
      ;
  }

  return len;
}

void setup_systick_config(void) {
  if (SysTick_Config(SystemCoreClock / 1000U)) {
    while (1) {
    }
  }
  NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void setup_uart(void) {
  rcu_periph_clock_enable(RCU_USART0);

  usart_deinit(USART0);
  usart_baudrate_set(USART0, 4000000U);
  usart_receive_config(USART0, USART_RECEIVE_ENABLE);
  usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
  usart_enable(USART0);
}

void setup_pins() {

  /* enable GPIOA clock */
  rcu_periph_clock_enable(RCU_GPIOA);

  /* PA0 -> ADC2, CH0*/
  gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0);

  /* PA9 ->  USARTx_Tx */
  gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

  /* PA10 -> USARTx_Rx */
  gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
}

dma_parameter_struct dma_init_struct;
extern volatile uint16_t bufferA[ADC_SAMPLES];
extern volatile uint16_t bufferB[ADC_SAMPLES]; 

extern volatile uint16_t *currentBuffer;
extern volatile uint16_t *nextBuffer;

void setup_dma() {
  rcu_periph_clock_enable(RCU_DMA1);

  dma_deinit(DMA1, DMA_CH4);
  dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_addr = (uint32_t)currentBuffer;
  dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
  dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
  dma_init_struct.number = ADC_SAMPLES;
  dma_init_struct.periph_addr = (uint32_t)(&ADC_RDATA(ADC_NUM_CH));
  dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
  dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
  dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
  dma_init(DMA1, DMA_CH4, &dma_init_struct);

  dma_circulation_enable(DMA1, DMA_CH4);

  nvic_irq_enable(DMA1_Channel3_Channel4_IRQn, 0, 1);
  dma_interrupt_enable(DMA1, DMA_CH4, DMA_INT_FTF);

  dma_channel_enable(DMA1, DMA_CH4);
}

void setup_adc() {
  rcu_periph_clock_enable(ADC_RCU);

  rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);

  adc_deinit(ADC_NUM_CH);

  adc_special_function_config(ADC_NUM_CH, ADC_CONTINUOUS_MODE, DISABLE);
  adc_special_function_config(ADC_NUM_CH, ADC_SCAN_MODE, DISABLE);

  adc_channel_length_config(ADC_NUM_CH, ADC_REGULAR_CHANNEL, 1);

  adc_regular_channel_config(ADC_NUM_CH, 0, ADC_CHANNEL_0,
                             ADC_SAMPLETIME_55POINT5);

  adc_external_trigger_config(ADC_NUM_CH, ADC_REGULAR_CHANNEL, ENABLE);

  adc_external_trigger_source_config(ADC_NUM_CH, ADC_REGULAR_CHANNEL,
                                     ADC_EXTTRIG_CH);

  adc_resolution_config(ADC_NUM_CH, ADC_RESOLUTION_12B);

  adc_data_alignment_config(ADC_NUM_CH, ADC_DATAALIGN_RIGHT);

  adc_enable(ADC_NUM_CH);

  adc_calibration_enable(ADC_NUM_CH);

  adc_dma_mode_enable(ADC_NUM_CH);
}

void setup_timer() {
  rcu_periph_clock_enable(ADC_PWM_TMER_RCU);

  timer_parameter_struct timer_initpara;
  timer_oc_parameter_struct timer_ocintpara;

  timer_deinit(ADC_PWM_TMER);

  /* ADC frequency: 50kHz */
  timer_initpara.prescaler = 23;
  timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection = TIMER_COUNTER_UP;
  timer_initpara.period = 100 - 1;
  timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(ADC_PWM_TMER, &timer_initpara);

  timer_channel_output_struct_para_init(&timer_ocintpara);
  timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
  timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
  timer_channel_output_config(ADC_PWM_TMER, ADC_PWM_CH, &timer_ocintpara);

  timer_channel_output_pulse_value_config(ADC_PWM_TMER, ADC_PWM_CH, 50);
  timer_channel_output_mode_config(ADC_PWM_TMER, ADC_PWM_CH,
                                   TIMER_OC_MODE_PWM0);
  timer_channel_output_shadow_config(ADC_PWM_TMER, ADC_PWM_CH,
                                     TIMER_OC_SHADOW_DISABLE);

  timer_auto_reload_shadow_enable(ADC_PWM_TMER);
}