#include "steggy.h"
#include "charlist.h"

void initStegyHeader(StegyHeader* header) {
    header->hiddenFileSize = 0;
    header->outputFormat[4] = '\0';
}

int getFileExtension(char* extension, char* name) {
    if (name == NULL || extension == NULL) return 1;

    int name_lenght = 0;
    while(name[name_lenght] != '\0') name_lenght++;

    int extension_length = 0;
    int extension_index = 0;
    for (extension_index = name_lenght; extension_index > 0; extension_index--) {
        if (name[extension_index] == '.') break;
        extension_length++;
    }
    extension_index++;
    extension_length--;

    if(extension_length > 4) return 1;

    for (int i = 0; i < extension_length; i++) {
        extension[i] = name[extension_index + i];
    }

    if (extension_length < 4) extension[extension_length] = ' ';
    return 0;
}

int fileByteSize(uint32_t* result, FILE* fp){
    int sucess = fseek(fp, 0L, SEEK_END);
    if (sucess != 0) return 1;

    *result = ftell(fp);

    //Secure to go to beginig
    fseek(fp, 0L, SEEK_SET);
    rewind(fp);

    return 0;
}

int writeStegyByte(Bmp* result, Bmp* original, uint8_t* originalBuffer, uint8_t byte) {
    int error = fread(originalBuffer, BYTE_SIZE, 2, original->pixels);
    if (error != 2) return error;

    uint8_t fbyte = mergeByte(originalBuffer[0], byte, 1);
    uint8_t sbyte = mergeByte(originalBuffer[1], byte, 0);

    uint8_t fberror = fputc(fbyte, result->pixels);
    uint8_t sberror = fputc(sbyte, result->pixels);

    if ((fberror == EOF) || (sberror == EOF)) return 1;
    return 0;
}

int putFileInBmp(Bmp* result, Bmp* original, FILE* fp, const char* fpname){
    uint32_t fpSize;
    int error = 0;

    error = fileByteSize(&fpSize, fp);
    if (error) return error;

//    printf("Checking size:\n\t-[targetFile: %d][fileToHidde: %d]\n", original->pixelCount, SIZE_TO_HIDDE(fpSize));
    printf("Checking size:\n\t-[targetFile: ");
    printf(GRN "%d" COLOR_RESET, original->pixelCount);
    printf("][fileToHidde: ");
    printf(GRN "%d" COLOR_RESET, SIZE_TO_HIDDE(fpSize));
    printf("]\n");
    if (original->pixelCount < SIZE_TO_HIDDE(fpSize)) return 1;

    //Buffers to read and write
    uint8_t originalBuffer[2];
    uint8_t fpBuffer;
    //Custom stegy header
    StegyHeader headerToWrite;
    initStegyHeader(&headerToWrite);

    headerToWrite.hiddenFileSize = fpSize;
    getFileExtension(headerToWrite.outputFormat, fpname);

    //Change if header is bigger now
    uint32_t header_offset = 16;
    //Putting header in image
    //Writing file size
    uint8_t byteFromFileSize = (headerToWrite.hiddenFileSize & 0xFF000000) >> 24;
    error = writeStegyByte(result, original, originalBuffer, byteFromFileSize);
    if (error) return error;

    byteFromFileSize = (headerToWrite.hiddenFileSize & 0x00FF0000) >> 16;
    error = writeStegyByte(result, original, originalBuffer, byteFromFileSize);
    if (error) return error;

    byteFromFileSize = (headerToWrite.hiddenFileSize & 0x0000FF00) >> 8;
    error = writeStegyByte(result, original, originalBuffer, byteFromFileSize);
    if (error) return error;

    byteFromFileSize = headerToWrite.hiddenFileSize & 0x000000FF;
    error = writeStegyByte(result, original, originalBuffer, byteFromFileSize);
    if (error) return error;

    //Writing extension
    error = writeStegyByte(result, original, originalBuffer, (uint8_t)headerToWrite.outputFormat[0]);
    if (error) return error;

    error = writeStegyByte(result, original, originalBuffer, (uint8_t)headerToWrite.outputFormat[1]);
    if (error) return error;

    error = writeStegyByte(result, original, originalBuffer, (uint8_t)headerToWrite.outputFormat[2]);
    if (error) return error;

    error = writeStegyByte(result, original, originalBuffer, (uint8_t)headerToWrite.outputFormat[3]);
    if (error) return error;
    //End Custom stegy header

    //Iterate original until size of fp
    //I would write code like if orginal is on pixel_array already and result is after header and fp at origin
    printf("Hidding file...\n");
    //+ 16 Bytes of header
    for (uint32_t idx = header_offset; idx < fpSize + header_offset; idx++) {
        error = fread(&fpBuffer, BYTE_SIZE, 1, fp);
        if (error != 1) return error;

        error = fread(originalBuffer, BYTE_SIZE, 2, original->pixels);
        if (error != 2) return error;

        uint8_t fbyte = mergeByte(originalBuffer[0], fpBuffer, 1);
        uint8_t sbyte = mergeByte(originalBuffer[1], fpBuffer, 0);

        uint8_t fberror = fputc(fbyte, result->pixels);
        uint8_t sberror = fputc(sbyte, result->pixels);

        if ((fberror == EOF) || (sberror == EOF)) return 1;
    }

    uint8_t miniBuffer;
    //+ 16 Bytes of header
    for (uint32_t idx = fpSize + header_offset; idx < original->imageSize - fpSize - original->headerSize; idx++) {
        error = fread(&miniBuffer, BYTE_SIZE, 1, original->pixels);
        if (error != 1) return error;

        uint8_t mberror = fputc(miniBuffer, result->pixels);
        if(mberror == EOF) return 1;
    }

    printf("Success hidding file in %s\n", result->name);
    return 0;
}

int readFileFromBmp(Bmp* origin, const char* filename) {
    //Pointer should be in begin of offset for pixel_array
    uint8_t sizeBuffer[8];
    uint8_t magicFileSize[4];
    uint32_t fileSize = 0;

    uint8_t extensionBuffer[8];
    char extension[5];
    extension[4] = '\0';

    uint8_t magicByteBuffer[2];

    int error = 0;

    error = fread(sizeBuffer, BYTE_SIZE, 8, origin->pixels);
    if (error != 8) return error;
    //Transform sizeBuffer to magicFileSize
    for (uint8_t i = 0; i < 4; i++) magicFileSize[i] = getMagicByte(sizeBuffer[i * 2], sizeBuffer[(i * 2)+1]);
    //magicFileSize to fileSize
    fileSize = (magicFileSize[0] << 24) | (magicFileSize[1] << 16) | (magicFileSize[2] << 8) | (magicFileSize[3]);

    error = fread(extensionBuffer, BYTE_SIZE, 8, origin->pixels);
    if (error != 8) return error;
    //Transform extensionBuffer to extension
    for (uint8_t i = 0; i < 4; i++) extension[i] = getMagicByte(extensionBuffer[i * 2], extensionBuffer[(i * 2)+1]);

    //Making the file of output
    CharList name;
    initCharList(&name);
    copyConstantCharArray(&name, filename);

    addToCharList(&name, '.');
    for (uint8_t idx = 0; extension[idx] != '\0'; idx++) {
        if (extension[idx] != ' ') addToCharList(&name, extension[idx]);
    }
    addToCharList(&name, '\0');

    FILE* fp = fopen(name.chars, "wb+");
    if (fp == NULL) return 1;

    uint8_t byteToWrite = 0;
    for (uint32_t i = 0; i < fileSize; i++) {
        error = fread(magicByteBuffer, BYTE_SIZE, 2, origin->pixels);
        if(error != 2) return error;

        byteToWrite = getMagicByte(magicByteBuffer[0], magicByteBuffer[1]);
        error = fputc(byteToWrite, fp);
        if (error == EOF) return 1;
    }

    fclose(fp);
    return 0;
}
