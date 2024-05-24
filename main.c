#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"
#include "steggy.h"
#include "token.h"

int checkIfFlag(char* ptr) {
    if (ptr == NULL && ptr[1] != '-') return 0;
    return 1;
}

int main(int argc, char **argv) {
    Bmp image;
    initBmp(&image);

    switch (argc) {
        case 8: {
            int tflags[3] = {1,1,1};
            FILE* file;

            Bmp output;
            FILE* outheader;
            initBmp(&output);

            uint8_t fileIdx = 0;

            Token token[6];

            for (uint8_t i = 2; i < argc; i++) {
                if (checkIfFlag(argv[i])) {
                    token[i - 2].value = argv[i];
                    token[i - 2].type = TOKEN_FLAG;
                    if (!checkIfFlag(argv[++i])) {
                        printf("Error: Syntax [-flag value].\n");
                        exit(1);
                    }
                    token[i - 2].value = argv[i];
                    token[i - 2].type = TOKEN_STRING;
                }
            }

            for (uint8_t i = 0; i < 6; i++) {
                if (token[i].type == TOKEN_FLAG) {
                    switch (token[i].value[1]) {
                        case 'i':
                            outheader = fopen(token[++i].value, "rb+");
                            if (outheader == NULL) {
                                printf("Unable to open file: %s\n", token[i].value);
                                exit(1);
                            }
                            loadBmp(&image, token[i].value);
                            tflags[0] = 0;
                            break;
                        case 'f':
                            fileIdx = ++i;
                            file = fopen(token[i].value, "rb+");
                            if (file == NULL) {
                                printf("Unable to open file: %s\n", token[i].value);
                                exit(1);
                            }
                            tflags[1] = 0;
                            break;
                        case 'o':
                            createBmp(&output, token[++i].value);
                            tflags[2] = 0;
                            break;
                    }
                }
            }

            if (tflags[0] || tflags[1] || tflags[2]) {
                printf("Error: encode -i [bmp] -f [file] -o [output]\n");
                exit(1);
            }

            loadBmpHead(&output, outheader);
            writeHeader(&output);

            int error = putFileInBmp(&output, &image, file, token[fileIdx].value);
            if (error) {
                printf("Error: Can't put file %s in %s\n", token[fileIdx].value, image.name);
                exit(1);
            }

            fclose(outheader);
            fclose(file);
            freeBmp(&output);
            break;
        }
        case 6: {
            uint8_t output_idx = 0;

            int tflags2[2] = {1,1};
            Token token2[4];

            for (uint8_t i = 2; i < argc; i++) {
                if (checkIfFlag(argv[i])) {
                    token2[i - 2].value = argv[i];
                    token2[i - 2].type = TOKEN_FLAG;
                    if (!checkIfFlag(argv[++i])) {
                        printf("Error: Syntax [-flag value].\n");
                        exit(1);
                    }
                    token2[i - 2].value = argv[i];
                    token2[i - 2].type = TOKEN_STRING;
                }
            }

            for (uint8_t i = 0; i < 4; i++) {
                if (token2[i].type == TOKEN_FLAG) {
                    switch (token2[i].value[1]) {
                        case 'i':
                            loadBmp(&image, token2[++i].value);
                            tflags2[0] = 0;
                            break;
                        case 'o':
                            output_idx = ++i;
                            tflags2[1] = 0;
                            break;
                    }
                }
            }

            if (tflags2[0] || tflags2[1]) {
                printf("Error: encode -i [bmp] -o [output]\n");
                exit(1);
            }

            printf("Getting file from %s...\n", image.name);
            int error = readFileFromBmp(&image, token2[output_idx].value);
            if (error) {
                printf("Could not read file from %s\n", image.name);
                exit(1);
            }
            printf("Hidden file output as %s.[extension]...\n", token2[output_idx].value);
            break;
        }
        default: {
            printf(RED "No valid command provided\n" COLOR_RESET);
            exit(1);
            break;
        }

    }
    freeBmp(&image);
    return 0;
}

