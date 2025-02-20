#include "frame.h"

#include <stdint.h>

#define FRAME_START    0x7E
#define FRAME_STOP     0x7F
#define FRAME_ESCAPE   0x7D
#define XOR_VALUE      0x20

void frame_build(uint8_t *payload, uint16_t payload_len, uint8_t *frame, uint16_t fixed_size) {
    uint16_t i, j = 0;
    uint8_t checksum = 0;
    
    frame[j++] = FRAME_START;
    
    for (i = 0; i < payload_len; i++) {
        uint8_t data = payload[i];
        checksum += data;
        
        if (data == FRAME_START || data == FRAME_STOP || data == FRAME_ESCAPE) {
            frame[j++] = FRAME_ESCAPE;
            frame[j++] = data ^ XOR_VALUE;
        } else {
            frame[j++] = data;
        }
    }
    
    if (checksum == FRAME_START || checksum == FRAME_STOP || checksum == FRAME_ESCAPE) {
        frame[j++] = FRAME_ESCAPE;
        frame[j++] = checksum ^ XOR_VALUE;
    } else {
        frame[j++] = checksum;
    }
    
    if (FRAME_STOP == FRAME_START || FRAME_STOP == FRAME_STOP || FRAME_STOP == FRAME_ESCAPE) {
        frame[j++] = FRAME_ESCAPE;
        frame[j++] = FRAME_STOP ^ XOR_VALUE;
    } else {
        frame[j++] = FRAME_STOP;
    }
    
    while (j < fixed_size) {
        frame[j++] = 0;
    }
}
