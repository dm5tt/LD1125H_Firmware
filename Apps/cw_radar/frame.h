#ifndef _FRAMING_H
#define _FRAMING_H

#include <stdint.h> 

void frame_build(uint8_t *payload, uint16_t payload_len, uint8_t *frame, uint16_t fixed_size);

#endif
