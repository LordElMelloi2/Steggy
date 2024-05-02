#ifndef stegy_bmp_h
#define stegy_bmp_h

#include "common.h"

#include <stdio.h>

#define BMP_HEADER 14

#define BYTES_TO_INT32(array, offset) array[offset] | \
    array[offset + 1] << 8 | \
    array[offset + 2] << 16 | \
    array[offset + 3] << 24

#define BYTES_TO_INT16(array, offset) array[offset] | array[offset + 1] << 8

typedef enum {
    SIZE_IN_BYTES = 0x02,
    PIXEL_ARRAY_OFFSET = 0x0A,
    SIZE_OF_HEADER = 0x0E,
    WIDTH_IN_PIXELS = 0x12,
    HEIGHT_IN_PIXELS = 0x16,
    BITS_PER_PIXEL = 0x1C,
} headerFlag;

typedef struct {
    const char* name;

    uint32_t width;
    uint32_t height;
    uint32_t imageSize;

    uint32_t headerSize;

    uint32_t pixelOffset;
    uint32_t pixelCount;
    uint16_t bitsPerPixel;
    /*HEADER and PIXELS*/
    uint8_t* header;
    FILE* pixels;
} Bmp;

void initBmp(Bmp* image);
void freeBmp(Bmp* image);

//loadBmp uses loadBmpHead to get the header from the file that loadBmp loads
void loadBmpHead(Bmp* image, FILE* fp);
void loadBmp(Bmp* image, const char* filename);

void createBmp(Bmp* image, const char* name);
void writeHeader(Bmp* image);

void printbmp(Bmp* image);
#endif // stegy_bmp_h
