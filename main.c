#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// BMP headers
#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;      // File type ("BM")
    uint32_t bfSize;      // File size in bytes
    uint16_t bfReserved1; // Reserved
    uint16_t bfReserved2; // Reserved
    uint32_t bfOffBits;   // Offset to pixel data
} BMPFileHeader;

typedef struct {
    uint32_t biSize;          // Header size
    int32_t biWidth;          // Image width
    int32_t biHeight;         // Image height
    uint16_t biPlanes;        // Number of color planes
    uint16_t biBitCount;      // Bits per pixel (24 for RGB)
    uint32_t biCompression;   // Compression type (0 for none)
    uint32_t biSizeImage;     // Image size in bytes
    int32_t biXPelsPerMeter;  // Horizontal resolution
    int32_t biYPelsPerMeter;  // Vertical resolution
    uint32_t biClrUsed;       // Number of colors used
    uint32_t biClrImportant;  // Important colorsg
} BMPInfoHeader;
#pragma pack(pop)

typedef struct {
    unsigned char *data;
    int width;
    int height;
} PixelsData;

char *decToBin(int dec);
int binToDec(char *bin);
PixelsData imageLoader();
unsigned char *insertText(PixelsData pixelsData, char *text);
char **encodeText(char *text);

int main (int argc, char *argv[]) {
    PixelsData pixelsData;
    pixelsData = imageLoader("cat.bmp");
    pixelsData.data = insertText(pixelsData, "Hello World");
}

unsigned char *insertText(PixelsData pixelsData, char *text) {
    int textLength = strlen(text);
    char **splittedCodes = encodeText(text);
    int index = 0;
    for (int i=0; i<pixelsData.height; i++){
        for (int j=0; j<pixelsData.width;j++){
            int r = (int)pixelsData.data[(i * pixelsData.width + j) * 3 + 0]; // Red channel
            int g = (int)pixelsData.data[(i * pixelsData.width + j) * 3 + 1]; // Green channel
            int b = (int)pixelsData.data[(i * pixelsData.width + j) * 3 + 2]; // Blue channel
            char *rBin = decToBin(r);
            char *gBin = decToBin(g);
            char *bBin = decToBin(b);
            rBin[6]=splittedCodes[index][0];
            rBin[7]=splittedCodes[index][1];
            rBin[8]=splittedCodes[index][2];
            index++;
            gBin[6]=splittedCodes[index][0];
            gBin[7]=splittedCodes[index][1];
            gBin[8]=splittedCodes[index][2];
            index++;
            bBin[6]=splittedCodes[index][0];
            bBin[7]=splittedCodes[index][1];
            bBin[8]=splittedCodes[index][2];
            index++;

            int newR = binToDec(rBin);
            int newG = binToDec(gBin);
            int newB = binToDec(bBin);

            pixelsData.data[(i * pixelsData.width + j) * 3 + 0] = newR;
            pixelsData.data[(i * pixelsData.width + j) * 3 + 1] = newG;
            pixelsData.data[(i * pixelsData.width + j) * 3 + 2] = newB;

            free(rBin);
            free(gBin);
            free(bBin);

            if (index > (textLength * 3)){
                for (int x = 0; x < textLength * 3 + 3; x++) {
                    free(splittedCodes[x]);
                }
                free(splittedCodes);
                return pixelsData.data;
            }
        }
    }

    for (int i = 0; i < textLength * 3 + 3; i++) {
        free(splittedCodes[i]);
    }
    free(splittedCodes);

    return pixelsData.data;
}

char **encodeText(char *text) {
    int textLength = strlen(text);
    char **splittedCodes = malloc((textLength * 3 + 3) * sizeof(char *));

    //Encoding Header of the text
    char *codesLength = decToBin(textLength*3); //Length of text's codes encoded to binary
    for (int i=0; i < 3; i++) {
        splittedCodes[i] = malloc(4);
        strncpy(splittedCodes[i], &codesLength[i * 3], 3);  // Copy 3 characters starting from i * 3
        splittedCodes[i][3] = '\0';
    }
    free(codesLength);

    //Encoding data of the text
    for (int i=0; i < textLength; i++){
        int code = (int)text[i];
        char *binChar = decToBin(code);
        for (int j = 0; j < 3; j++) {
            splittedCodes[3 + i * 3 + j] = malloc(4);
            strncpy(splittedCodes[3 + i*3 + j], binChar + j * 3, 3);
            splittedCodes[3 + i*3 + j][3] = '\0';
        }
        free(binChar);
    }
    return splittedCodes;
}

PixelsData imageLoader(const char *filename) {
    PixelsData data;
    int x, y, n;
    data.data = stbi_load(filename, &x, &y, &n, 0);
    data.width = x;
    data.height = y;

    if (data.data == NULL) {
        printf("Failed to load image");
    }

    printf("Image loaded: %s\n", filename);
    printf("Dimensions: %dx%d\n", x, y);
    printf("Channels: %d\n", n);

    return data;
}

char *decToBin(int dec) {
    char *answer = malloc(10 * sizeof(char));
    int binaryBase = 256;
    int index=0;
    for (int i=0; i < 9; i++){
        if (dec >= binaryBase) {
            dec -= binaryBase;
            answer[index]='1';
        }
        else {
            answer[index]='0';
        }
        binaryBase = binaryBase / 2;
        index++;
    }
    answer[9]='\0';
    return answer;
}

int binToDec(char *bin) {
    int answer = 0;
    for (int i=0; i<9; i++) {
        int num = bin[i] - '0';
        int power = (int)pow(2, 9-i-1);
        answer += num * power;
    }
    return answer;
}