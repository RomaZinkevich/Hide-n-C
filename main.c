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
    uint32_t biClrImportant;  // Important colors
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
void createImage(const char *filename, int width, int height, unsigned char *pixelData);
unsigned char *insertText(PixelsData pixelsData, char *text);
char **encodeText(char *text);
char *dragText(PixelsData pixelsData);
int dragMessageLength(PixelsData pixelsData);

void clearInputBuffer(){
    while (getchar() != '\n');
}

int isValidFilename(const char *filename) {
    if (strlen(filename) < 5) {
        return 0;
    }

    if (strstr(filename, ".png") == filename + strlen(filename) - 4 ||
        strstr(filename, ".jpg") == filename + strlen(filename) - 4 ||
        strstr(filename, ".bmp") == filename + strlen(filename) - 4) {
        return 1;
    } else {
        printf("Error: Filename must end with .png, .jpg, or .bmp.\n");
        return 0;
    }
}

void safeFgets(char *buffer, size_t size) {
    if (fgets(buffer, size, stdin) == NULL) {
        printf("Error reading input.\n");
        clearInputBuffer();
        return;
    }

    size_t len = strlen(buffer);

    if (len > 0 && buffer[len - 1] != '\n') {
        printf("Input is too long! Maximum length allowed is %zu characters.\n", size - 1);
        clearInputBuffer();
        buffer[0] = '\0';
    } else {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}

int main (int argc, char *argv[]) {
    char filename[100];
    char text[170];
    char newFilename[100];
    PixelsData pixelsData;
    printf("Welcome to Hide-n-C! A simple tool for hiding messages in BMP images.\nYou can either hide a message in an image or extract a hidden message from an image.\n");
    while (1) {
        int ans;
        printf("\nChoose an action:\n1: Hide text message in an image\n2: Retrieve text message from an image\n0: Quit\n");
        if (scanf("%d", &ans) != 1) {
            printf("Invalid input! Please enter a valid command\n");
            clearInputBuffer();
        }
        clearInputBuffer();
        if (ans == 0) {
            printf("Exiting Hide-n-C! Goodbye!");
            break;
        }
        switch(ans) {
            case 1:
                printf("Enter the path of the image to hide a message in (length 1-100): ");
                safeFgets(filename, sizeof(filename));
                pixelsData = imageLoader(filename);
                if (pixelsData.data == NULL) {
                    printf("Something went wrong! Try Again!\n");
                    break;
                }

                printf("Enter the message to hide (length 1-170): ");
                safeFgets(text, sizeof(text));
                if (strlen(text) <= 0 || strlen(text) >= 170) {
                    printf("Invalid message format!\n");
                    break;
                }
                printf("%d", strlen(text));
                pixelsData.data = insertText(pixelsData, text);

                printf("Enter the path of the image to save the new image (length 1-100): ");
                safeFgets(newFilename, sizeof(newFilename));
                if (!isValidFilename(newFilename)) {
                    printf("Invalid filename! Please try again.\n");
                    break;
                }

                createImage(newFilename, pixelsData.width, pixelsData.height, pixelsData.data);
                break;

            case 2:
                printf("Enter the path of the image to extract the hidden message from: ");
                safeFgets(filename, sizeof(filename));
                pixelsData = imageLoader(filename);
                if (pixelsData.data == NULL) {
                    printf("Something went wrong! Try Again!\n");
                    break;
                }

                char *text = dragText(pixelsData);
                printf("\nThe hidden message is: %s\n", text);
                free(text);
                break;

            default:
                printf("Invalid input! Please enter a valid command\n");
                break;
        }
    }
}

char *dragText(PixelsData pixelsData) {
    int messageLength = dragMessageLength(pixelsData);
    char message[messageLength/3][10];
    memset(message, '\0', sizeof(message));
    int index = 0;
    char *strMessage = malloc((messageLength / 3) + 1);
    for (int i = 0; i<pixelsData.height; i++) {
        for (int j = 3; j<pixelsData.width; j++) {
            int pixelsIndex = (i * pixelsData.width + j) * 3 - 6;
            int r = (int)pixelsData.data[pixelsIndex]; // Red channel
            int g = (int)pixelsData.data[pixelsIndex + 1]; // Green channel
            int b = (int)pixelsData.data[pixelsIndex + 2]; // Blue channel
            char *rBin = decToBin(r);
            char *gBin = decToBin(g);
            char *bBin = decToBin(b);
            strncpy(message[index], &rBin[6], 3);
            strncat(message[index], &gBin[6], 3);
            strncat(message[index], &bBin[6], 3);
            message[index][9] = '\0';
            index++;
            free(rBin);
            free(gBin);
            free(bBin);

            if (index==messageLength/3)
                break;
        }
        if (index==messageLength/3)
            break;
    }

    for (int i=0; i<messageLength/3; i++) {
        int num = binToDec(message[i]);
        char ch = (char)num;
        strMessage[i] = ch;
    }
    strMessage[messageLength / 3] = '\0';
    return strMessage;
}

int dragMessageLength(PixelsData pixelsData) {
    char headerCodes[10] = "";
    int r = (int)pixelsData.data[0]; // Red channel
    int g = (int)pixelsData.data[1]; // Green channel
    int b = (int)pixelsData.data[2]; // Blue channel
    char *rBin = decToBin(r);
    char *gBin = decToBin(g);
    char *bBin = decToBin(b);
    strncpy(headerCodes, &rBin[6], 3);
    strncat(headerCodes, &gBin[6], 3);
    strncat(headerCodes, &bBin[6], 3);
    strncat(headerCodes, "\0", 1);

    free(rBin);
    free(gBin);
    free(bBin);

    return binToDec(headerCodes);
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

void createImage(const char *filename, int width, int height, unsigned char *pixelData) {
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    int rowSize = (3 * width + 3) & ~3;
    int pixelArraySize = rowSize * height;
    int fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + pixelArraySize;

    // Populate file header
    fileHeader.bfType = 0x4D42; // 'BM'
    fileHeader.bfSize = fileSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    // Populate info header
    infoHeader.biSize = sizeof(BMPInfoHeader);
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24; // RGB
    infoHeader.biCompression = 0;
    infoHeader.biSizeImage = pixelArraySize;
    infoHeader.biXPelsPerMeter = 2835; // ~72 DPI
    infoHeader.biYPelsPerMeter = 2835; // ~72 DPI
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    // Write to file
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    fwrite(&fileHeader, sizeof(BMPFileHeader), 1, file);
    fwrite(&infoHeader, sizeof(BMPInfoHeader), 1, file);

    // Write pixel data (with padding)
    uint8_t padding[3] = {0, 0, 0};
    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width; j++) {
            unsigned char r = pixelData[(i * width + j) * 3 + 2]; // Red channel
            unsigned char g = pixelData[(i * width + j) * 3 + 1]; // Green channel
            unsigned char b = pixelData[(i * width + j) * 3 + 0]; // Blue channel

            fwrite(&r, 1, 1, file);
            fwrite(&g, 1, 1, file);
            fwrite(&b, 1, 1, file);
        }

        // Write padding
        fwrite(padding, 1, rowSize - (width * 3), file);
    }

    fclose(file);
    printf("Image file created: %s\n", filename);
}

PixelsData imageLoader(const char *filename) {
    PixelsData data = {NULL, 0, 0};;
    int x, y, n;
    data.data = stbi_load(filename, &x, &y, &n, 0);
    data.width = x;
    data.height = y;

    if (data.data == NULL || n != 3) {
        printf("Failed to load image\n");
        data.data = NULL;
        return data;
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