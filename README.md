# Steganography Tool

A simple steganography tool implemented in C that allows you to hide and extract messages in BMP, PNG and JPG images.

## Features

- Hide a secret message inside an image.
- Extract a hidden message from an image.
- Command-line interface for easy use.
- Uses LSB (Least Significant Bit) encoding for steganography.

## Requirements

- A C compiler (e.g., GCC)
- Image files (.png, .bmp, .jpg)

## Installation

To compile the program, run:

```sh
gcc -o main main.c -lm
```

To run the program:
- **Windows**: Run `main.exe`
- **Linux**: Run `./main`

## Dependencies

This project uses the [stb_image](https://github.com/nothings/stb) library for loading image files:

- `stb_image.h` for reading PNG, JPG, and BMP images.

Make sure to include this header in your project.
