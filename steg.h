#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

// Structure to hold bitmap file and image headers information

#pragma pack(push, 1)

typedef struct tagBmpHeader
{

    // BmpFileHeader info

    short bfType;           // File type
    unsigned int bfSize;    // Bitmap file size in bytes
    short bfReserved1;      // Reserved; must be 0
    short bfReserved2;      // Reserved; must be 0
    unsigned int bfOffBits; // Offset in bytes from the bitmap file header to the bitmap bits

    // BmpInfoHeader info

    unsigned int biSize;          // Number of bytes required by the struct
    int biWidth;                  // Width in pixels
    int biHeight;                 // Height in pixels
    short biPlanes;               // Number of color planes: must be 1
    short biBitCount;             // Number of bits per pixel
    unsigned int biCompression;   // Type of compression
    unsigned int biSizeImage;     // Size of image in bytes
    unsigned int biXPelsPerMeter; // Number of pixels per meter in x axis
    unsigned int biYPelsPerMeter; // Number of pixels per meter in y axis
    unsigned int biClrUsed;       // Number of colors used by the bitmap
    unsigned int biClrImportant;  // Number of colors that are important

} BmpHeader;

#pragma pack(pop)

// File Manipulation functions

BmpHeader *get_BmpHeader(const char *fname);                             // Gets and stores file and bitmap header information
unsigned char *get_BmpImage(const char *fname, const BmpHeader *header); // Gets the pixels from the bitmap data
int write_bmp_image(const char *fname,
                    const BmpHeader *header,
                    const unsigned char *image);                                 // Creates a new bitmap image file
int write_data(const char *dname, const unsigned char *dataBytes, int fileSize); // Write extracted data from image to a file

// Data Manipulation functions

unsigned char ***create_matrix(int height, int width);
unsigned char *to_array(const BmpHeader *header, unsigned char ***matrix);
unsigned char ***to_matrix(const BmpHeader *header, const unsigned char *image, int extend_padding);

// Data Embedding functions

unsigned char *sub_lsb(const BmpHeader *header, const unsigned char *image, const char *dname); // Embeds data from file "dname" into LSBs of given image

// Data Extracting functions

unsigned char *get_lsb_data(const unsigned char *image, int fileSize); // Extracts embedded data from given image and returns a byte array