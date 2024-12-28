#ifndef _SETUP_H
#define _SETUP_H

void setup_systick_config(void);
void setup_uart(void);
void setup_adc(void);
void setup_dac(void);
void setup_dma(void);
void setup_pins(void);
void setup_timer(void);

// PWM
#define ADC_PWM_TMER_RCU RCU_TIMER4
#define ADC_PWM_TMER TIMER4
#define ADC_PWM_CH TIMER_CH_0

// ADC
#define ADC_NUM_CH ADC2
#define ADC_RCU RCU_ADC2
#define ADC_EXTTRIG_CH ADC2_EXTTRIG_REGULAR_T4_CH0

#define ADC_SAMPLES 8192

#endif 
