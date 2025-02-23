/*!
    \file    gd32f30x_it.c
    \brief   main interrupt service routines

    \version 2020-08-01, V3.0.0, firmware for GD32F30x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "gd32f30x_it.h"
#include "frame.h"
#include "setup.h"

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void) {}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void) {
  /* if Hard Fault exception occurs, go to infinite loop */
  while (1)
    ;
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void) {
  /* if Memory Manage exception occurs, go to infinite loop */
  while (1)
    ;
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void) {
  /* if Bus Fault exception occurs, go to infinite loop */
  while (1)
    ;
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void) {
  /* if Usage Fault exception occurs, go to infinite loop */
  while (1)
    ;
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void) {}

volatile uint16_t buffer[ADC_SAMPLES] = {0};
uint8_t frameBuffer[FRAME_BUFFER_SIZE] = {0};

void DMA1_Channel3_4_IRQHandler(void) {

  // Check if DMA transfer is complete
  if (dma_interrupt_flag_get(DMA1, DMA_CH4, DMA_INT_FLAG_FTF) == SET) {
    dma_flag_clear(DMA1, DMA_CH4, DMA_INT_FLAG_FTF);

    dma_channel_disable(DMA0, DMA_CH3);

    frame_build((uint8_t *)buffer, HALF_PAYLOAD_SIZE * sizeof(uint16_t),
                frameBuffer, FRAME_BUFFER_SIZE);

    dma_memory_address_config(DMA0, DMA_CH3, (uint32_t)frameBuffer);

    dma_channel_enable(DMA0, DMA_CH3);

    return;
  }

  if (dma_interrupt_flag_get(DMA1, DMA_CH4, DMA_INT_FLAG_HTF) == SET) {

    dma_flag_clear(DMA1, DMA_CH4, DMA_INT_FLAG_HTF);
    dma_channel_disable(DMA0, DMA_CH3);

    frame_build((uint8_t *)&buffer[HALF_PAYLOAD_SIZE],
                HALF_PAYLOAD_SIZE * sizeof(uint16_t), frameBuffer,
                FRAME_BUFFER_SIZE);

    dma_memory_address_config(DMA0, DMA_CH3, (uint32_t)frameBuffer);
    dma_channel_enable(DMA0, DMA_CH3);

    return;
  }
}
