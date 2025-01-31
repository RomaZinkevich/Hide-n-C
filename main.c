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

PixelsData imageLoader();

int main (int argc, char *argv[]) {
    PixelsData pixelsData;
    pixelsData = imageLoader("cat.bmp");
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