#include "pixel.h"

uint8_t mergeByte(uint8_t ms, uint8_t ls, int ls_ms) {
    uint8_t ls_t = ls_ms != 0 ? ((ls & 0xF0) >> 4) : (ls & 0x0F);
    return (ms & 0xF0) | ls_t;
}
uint8_t getMagicByte(uint8_t ms, uint8_t ls){
    uint8_t ms_ls = (ms & 0x0F) << 4;
    uint8_t ls_ls = (ls & 0x0F);
    return ms_ls | ls_ls;
}
