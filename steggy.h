#ifndef stegy_stegy_h
#define stegy_stegy_h

#include "common.h"
#include "bmp.h"

#define BYTE_SIZE sizeof(uint8_t)
#define SIZE_TO_HIDDE(szinbytes) ((szinbytes * 2) + 16)

typedef struct {
    uint32_t hiddenFileSize;
    char outputFormat[5];
} StegyHeader;

void initStegyHeader(StegyHeader* header);
int getFileExtension(char* extension, char* name);

int fileByteSize(uint32_t* result, FILE* fp);
int writeStegyByte(Bmp* result, Bmp* original, uint8_t* originalBuffer, uint8_t byte);
int putFileInBmp(Bmp* result, Bmp* original, FILE* fp, const char* fpname);
int readFileFromBmp(Bmp* origin, const char* filename);

#endif // stegy_stegy_h
