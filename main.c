#include "steg.h"

int main()
{
    // Read image data

    BmpHeader *header = get_BmpHeader("test_img.bmp");
    unsigned char *image = get_BmpImage("test_img.bmp", header);
    unsigned char *result = sub_lsb(header, image, "Makefile");
    write_bmp_image("result.bmp", header, result);
    unsigned char *data = get_lsb_data(result, 370);
    write_data("extracted_data", data, 370);

    free(image);
    free(result);
    free(data);
    free(header);

    return 0;
}