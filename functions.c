#include "steg.h"

BmpHeader *get_BmpHeader(const char *fname)
{
    // Allocate memory to story header information

    BmpHeader *header = malloc(sizeof(BmpHeader));
    if (!header)
    {
        printf("Header memory allocation failed.\n");
        return NULL;
    }

    // Open file in read binary mode

    FILE *fptr = fopen(fname, "rb");
    if (!fptr)
    {
        printf("File read failed.\n");
        return NULL;
    }

    // Read the file header

    fread(header, sizeof(BmpHeader), 1, fptr);

    // Close file pointer and return header

    fclose(fptr);
    return header;
}

unsigned char *get_BmpImage(const char *fname, const BmpHeader *header)
{
    // Open file in read binary mode

    FILE *fptr = fopen(fname, "rb");
    if (!fptr)
    {
        printf("File read failed.\n");
        return NULL;
    }

    // Verify that this is a .BMP file by checking bitmap id

    if (header->bfType != 0x4D42)
    {
        printf("The read file is not a .bmp file.\n");
        fclose(fptr);
        return NULL;
    }

    // Prepare to extract image bitmap data

    unsigned char *image; // store image data

    // Move file pointer to the beginning of bitmap data

    fseek(fptr, header->bfOffBits, SEEK_SET);

    // Allocate enough memory for the bitmap image data

    image = malloc(header->biSizeImage);

    // Verify memory allocation

    if (!image)
    {
        printf("BitmapImage memory allocation failed.\n");
        fclose(fptr);
        return NULL;
    }

    // Read in the bitmap image data

    fread(image, header->biSizeImage, 1, fptr);

    // Make sure bitmap image data was read

    if (!image)
    {
        printf("BitmapImage data read failed.\n");
        fclose(fptr);
        return NULL;
    }

    // Close file, free header pointers and return bitmap image data

    fclose(fptr);

    return image;
}

int write_bmp_image(const char *fname, const BmpHeader *header, const unsigned char *image)
{
    // Open file in write binary mode

    FILE *fptr = fopen(fname, "wb");
    if (!fptr)
    {
        printf("File write failed.\n");
        return -1;
    }

    // Write header information

    fwrite(header, sizeof(BmpHeader), 1, fptr);

    // Move file pointer to beginning of bitmap data

    fseek(fptr, header->bfOffBits, SEEK_SET);
    fwrite(image, header->biSizeImage, 1, fptr);

    // Close file and return

    fclose(fptr);
    return 0;
}

int write_data(const char *dname, const unsigned char *dataBytes, int fileSize)
{
    FILE *fptr = fopen(dname, "wb");
    if (!fptr)
    {
        printf("Write file creation failed.\n");
        return -1;
    }

    fwrite(dataBytes, fileSize, 1, fptr);

    fclose(fptr);

    return 0;
}

unsigned char ***create_matrix(int height, int width)
{

    /*
        Creates an empty pixel matrix
    */

    unsigned char ***matrix = malloc(height * sizeof(unsigned char **));
    if (!matrix)
    {
        printf("Matrix memory allocation I failed.\n");
        return NULL;
    }

    for (int i = 0; i < height; i++)
    {
        matrix[i] = malloc(width * sizeof(unsigned char *));
        if (!matrix[i])
        {
            printf("Matrix memory allocation II failed.\n");
            free(matrix);
            return NULL;
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            matrix[i][j] = malloc(3);
            if (!matrix[i][j])
            {
                printf("Matrix memory allocation III failed.\n");
                for (int a = 0; a < height; a++)
                {
                    free(matrix[a]);
                }
                free(matrix);
                return NULL;
            }
        }
    }

    return matrix;
}

unsigned char ***to_matrix(const BmpHeader *header, const unsigned char *image, int extend_padding)
{

    /*
      Converts given array of image pixels into a 2D array of image pixels for ease of use.
    */

    // Get relevant image details

    const int width = header->biWidth, height = header->biHeight;
    int padding = (4 - (width * 3) % 4) % 4;
    const int stride = width * 3 + padding;
    unsigned char ***matrix = create_matrix(height + 2 * extend_padding, width + 2 * extend_padding);

    // Fill matrix with pixels from image

    for (int i = extend_padding; i < extend_padding + height; i++)
    {
        for (int j = extend_padding; j < extend_padding + width; j++)
        {
            matrix[i][j][0] = image[(i - extend_padding) * stride + (j - extend_padding) * 3];     // BLUE
            matrix[i][j][1] = image[(i - extend_padding) * stride + (j - extend_padding) * 3 + 1]; // GREEN
            matrix[i][j][2] = image[(i - extend_padding) * stride + (j - extend_padding) * 3 + 2]; // RED
        }
    }

    // Return matrix if extended padding not required

    if (extend_padding == 0)
    {
        return matrix;
    }

    // Fill padding by copying image border pixels if required

    // Top left corner

    for (int i = 0; i < extend_padding; i++)
    {
        for (int j = 0; j < extend_padding; j++)
        {
            matrix[i][j][0] = matrix[i + extend_padding][j + extend_padding][0];
            matrix[i][j][1] = matrix[i + extend_padding][j + extend_padding][1];
            matrix[i][j][2] = matrix[i + extend_padding][j + extend_padding][2];
        }
    }

    // Top right corner

    for (int i = 0; i < extend_padding; i++)
    {
        for (int j = extend_padding + width; j < 2 * extend_padding + width; j++)
        {
            matrix[i][j][0] = matrix[i + extend_padding][j - extend_padding][0];
            matrix[i][j][1] = matrix[i + extend_padding][j - extend_padding][1];
            matrix[i][j][2] = matrix[i + extend_padding][j - extend_padding][2];
        }
    }

    // Bottom left corner

    for (int i = extend_padding + height; i < 2 * extend_padding + height; i++)
    {
        for (int j = 0; j < extend_padding; j++)
        {
            matrix[i][j][0] = matrix[i - extend_padding][j + extend_padding][0];
            matrix[i][j][1] = matrix[i - extend_padding][j + extend_padding][1];
            matrix[i][j][2] = matrix[i - extend_padding][j + extend_padding][2];
        }
    }

    // Bottom right corner

    for (int i = extend_padding + height; i < 2 * extend_padding + height; i++)
    {
        for (int j = extend_padding + width; j < 2 * extend_padding + width; j++)
        {
            matrix[i][j][0] = matrix[i - extend_padding][j - extend_padding][0];
            matrix[i][j][1] = matrix[i - extend_padding][j - extend_padding][1];
            matrix[i][j][2] = matrix[i - extend_padding][j - extend_padding][2];
        }
    }

    // Top edge

    for (int i = 0; i < extend_padding; i++)
    {
        for (int j = extend_padding; j < extend_padding + width; j++)
        {
            matrix[i][j][0] = matrix[i + extend_padding][j][0];
            matrix[i][j][1] = matrix[i + extend_padding][j][1];
            matrix[i][j][2] = matrix[i + extend_padding][j][2];
        }
    }

    // Bottom edge

    for (int i = extend_padding + height; i < 2 * extend_padding + height; i++)
    {
        for (int j = extend_padding; j < extend_padding + width; j++)
        {
            matrix[i][j][0] = matrix[i - extend_padding][j][0];
            matrix[i][j][1] = matrix[i - extend_padding][j][1];
            matrix[i][j][2] = matrix[i - extend_padding][j][2];
        }
    }

    // Left edge

    for (int i = extend_padding; i < extend_padding + height; i++)
    {
        for (int j = 0; j < extend_padding; j++)
        {
            matrix[i][j][0] = matrix[i][j + extend_padding][0];
            matrix[i][j][1] = matrix[i][j + extend_padding][1];
            matrix[i][j][2] = matrix[i][j + extend_padding][2];
        }
    }

    // Right edge

    for (int i = extend_padding; i < extend_padding + height; i++)
    {
        for (int j = extend_padding + width; j < 2 * extend_padding + width; j++)
        {
            matrix[i][j][0] = matrix[i][j - extend_padding][0];
            matrix[i][j][1] = matrix[i][j - extend_padding][1];
            matrix[i][j][2] = matrix[i][j - extend_padding][2];
        }
    }

    return matrix;
}

unsigned char *to_array(const BmpHeader *header, unsigned char ***matrix)
{

    /*
      Converts given matrix of image pixels into an array of pixels for ease of use.
    */

    // Get relevant image details

    const int numBytes = header->biSizeImage;
    const int width = header->biWidth, height = header->biHeight;
    int padding = (4 - (width * 3) % 4) % 4;

    // Create array to store pixel values

    unsigned char *array = malloc(numBytes);
    if (!array)
    {
        printf("Array memory allocation failed.\n");
        return NULL;
    }

    // Copy pixels from matrix to array

    int k = 0;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            array[k++] = matrix[i][j][0];
            array[k++] = matrix[i][j][1];
            array[k++] = matrix[i][j][2];
        }

        for (int l = 0; l < padding; l++)
        {
            array[k++] = 0;
        }
    }

    return array;
}

unsigned char *sub_lsb(const BmpHeader *header, const unsigned char *image, const char *dname)
{

    /*
        Embeds data bits into n LSB bits of given image.
    */

    // Check if data can be fit in image

    int imgSize = header->biSizeImage;
    FILE *data = fopen(dname, "rb");
    if (!data)
    {
        printf("Data file read failed.\n");
        return NULL;
    }

    fseek(data, 0L, SEEK_END);
    int fileSize = ftell(data);
    int n_bits = 2; // LSBs to be used

    if (fileSize > n_bits * imgSize / 8)
    {
        printf("File to be embedded is too big, choose another file or increase number of LSBs to be used.\n");
        return NULL;
    }

    // Create copy of image array to be modified

    unsigned char *result = malloc(imgSize);
    if (!result)
    {
        printf("Result array memory allocation failed.\n");
        return NULL;
    }

    memcpy(result, image, imgSize);

    // Replace n LSBs in the image with data from the file

    fseek(data, 0L, SEEK_SET); // Point the data pointer back to the TOP
    int idx = 0;

    for (int i = 0; i < fileSize; i++)
    {
        char dataByte = fgetc(data);
        char mask = 3;

        // Divide the dataByte into bits and embed them

        for (int j = 0; j < 4; j++)
        {
            char dataLSBs = dataByte & mask; // Data Bits
            dataLSBs >>= 2 * j;
            result[idx] &= ~3;         // Clear LSBs
            result[idx++] |= dataLSBs; // Set Bits
            mask <<= n_bits;           // Change mask
        }
    }

    fclose(data);

    return result;
}

unsigned char *get_lsb_data(const unsigned char *image, const int fileSize)
{

    /*
        Extracts embedded data from the given image and returns it.
    */

    // Allocate memory for the data array

    unsigned char *dataBytes = calloc(fileSize, sizeof(unsigned char));
    if (!dataBytes)
    {
        printf("Data array memory allocation failed.\n");
        return NULL;
    }

    // Extract data

    int idx = 0;

    for (int i = 0; i < fileSize; i++)
    {

        // Combine the separated bits into single bytes of data

        for (int j = 0; j < 4; j++)
        {
            char dataLSBs = image[idx++] & 3;
            dataLSBs <<= 2 * j;       // Shift bits to correct position
            dataBytes[i] |= dataLSBs; // Set Bits
        }
    }

    return dataBytes;
}