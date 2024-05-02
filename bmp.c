#include "bmp.h"
#include "memory.h"

/*Init and Free*/
void initBmp(Bmp* image){
    image->name = NULL;
    image->width = 0;
    image->height = 0;
    image->imageSize = 0;
    image->headerSize = 0;
    image->pixelOffset = 0;
    image->pixelCount = 0;
    image->bitsPerPixel = 0;
    image->header = NULL;
    image->pixels = NULL;
}

void freeBmp(Bmp* image){
    fclose(image->pixels);
    FREE_ARRAY(uint8_t, image->header, image->headerSize);
    initBmp(image);
}
/*End Init and Free*/

void loadBmpHead(Bmp* image, FILE* fp) {
    //Getting size of header
    uint8_t headerSizePtr[4];
    fseek(fp, SIZE_OF_HEADER, SEEK_SET);
    int sucess = fread(headerSizePtr, sizeof(uint8_t), 4, fp);
    fseek(fp, 0L, SEEK_SET);
    rewind(fp);

    if (sucess == 0){
        printf("Error: Can't not read from file: %s\n", image->name);
        exit(1);
    }

    //Adjust header to include 1st 14 bytes
    image->headerSize = BYTES_TO_INT32(headerSizePtr, 0);
    image->headerSize += BMP_HEADER;

    //Copying header in memory
    image->header = GROW_ARRAY(uint8_t, image->header, 0, image->headerSize);
    sucess = fread(image->header, sizeof(uint8_t), image->headerSize, fp);

    if (sucess == 0){
        printf("Error: Can't read header");
        exit(1);
    }

    //Getting info from header
    image->width = BYTES_TO_INT32(image->header, WIDTH_IN_PIXELS);
    image->height = BYTES_TO_INT32(image->header, HEIGHT_IN_PIXELS);
    image->imageSize = BYTES_TO_INT32(image->header, SIZE_IN_BYTES);
    image->bitsPerPixel = BYTES_TO_INT16(image->header, BITS_PER_PIXEL);

    image->pixelOffset = BYTES_TO_INT32(image->header, PIXEL_ARRAY_OFFSET);
    /*pixel count tells you how many bytes there are for all pixels in the image*/
    image->pixelCount = ((image->width * image->height * image->bitsPerPixel) / 8);

    //Making file pointer being at begin of file
    fseek(fp, 0L, SEEK_SET);
    rewind(fp);
}

void loadBmp(Bmp* image, const char* filename) {

    image->name = filename;

    //Open file in pixels to point in the pixels and to put it in memory
    if((image->pixels = fopen(image->name, "rb+")) == NULL) {
        printf("Error: File can not be open %s\n", image->name);
        exit(1);
    }

    loadBmpHead(image, image->pixels);

    if (image->header == NULL) {
        // Unreachable?
        printf("Error: Header is NULL");
        exit(1);
    }

    //¨Point file to the begin of the pixel array
    fseek(image->pixels, image->pixelOffset, SEEK_SET);
}

void createBmp(Bmp* image, const char* outputname){
    image->name = outputname;
    image->pixels = fopen(image->name, "wb+");

    if(image->pixels == NULL) {
        printf("Error: Can't write in %s\n", image->name);
        exit(1);
    }
}

void writeHeader(Bmp* image) {

    for (int i = 0; i < image->headerSize; i++) {
        int error = fputc(image->header[i], image->pixels);
        if (error == EOF || error != image->header[i]) {
            printf("Error: Can't write the header.\n");
            exit(1);
        }
    }
}

void printbmp(Bmp* image){
    printf("=== FileName: %s ===\n", image->name);
    printf("Width: %d\n", image->width);
    printf("Height: %d\n", image->height);
    printf("Size of file: %d\n", image->imageSize);
    printf("Size on bits: %d\n", image->pixelCount);
    printf("Header size: %d\n", image->headerSize);
    printf("Offset Pixel Array: %d\n", image->pixelOffset);
    printf("BitsPerPixel: %d\n", image->bitsPerPixel);
}
