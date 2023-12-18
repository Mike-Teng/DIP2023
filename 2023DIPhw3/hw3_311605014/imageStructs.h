#pragma once
#include<cstdlib>

struct RGB {
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

struct RGBA {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

// file header 
struct BMPFILEHEADER
{
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
};

// info header
struct BMPINFOHEADER
{
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
};

void readBMPHeaders(FILE* file, BMPFILEHEADER& fileHeader, BMPINFOHEADER& infoHeader) {
    // Read the file header
    fread(&fileHeader.bfType, sizeof(unsigned short), 1, file);
    fread(&fileHeader.bfSize, sizeof(unsigned int), 1, file);
    fread(&fileHeader.bfReserved1, sizeof(unsigned short), 1, file);
    fread(&fileHeader.bfReserved2, sizeof(unsigned short), 1, file);
    fread(&fileHeader.bfOffBits, sizeof(unsigned int), 1, file);

    // Read the info header
    fread(&infoHeader.biSize, sizeof(unsigned int), 1, file);
    fread(&infoHeader.biWidth, sizeof(int), 1, file);
    fread(&infoHeader.biHeight, sizeof(int), 1, file);
    fread(&infoHeader.biPlanes, sizeof(unsigned short), 1, file);
    fread(&infoHeader.biBitCount, sizeof(unsigned short), 1, file);
    fread(&infoHeader.biCompression, sizeof(unsigned int), 1, file);
    fread(&infoHeader.biSizeImage, sizeof(unsigned int), 1, file);
    fread(&infoHeader.biXPelsPerMeter, sizeof(int), 1, file);
    fread(&infoHeader.biYPelsPerMeter, sizeof(int), 1, file);
    fread(&infoHeader.biClrUsed, sizeof(unsigned int), 1, file);
    fread(&infoHeader.biClrImportant, sizeof(unsigned int), 1, file);
};

void writeBMPHeaders(FILE* file, BMPFILEHEADER& fileHeader, BMPINFOHEADER& infoHeader) {
    // Write the file header
    fwrite(&fileHeader.bfType, sizeof(unsigned short), 1, file);
    fwrite(&fileHeader.bfSize, sizeof(unsigned int), 1, file);
    fwrite(&fileHeader.bfReserved1, sizeof(unsigned short), 1, file);
    fwrite(&fileHeader.bfReserved2, sizeof(unsigned short), 1, file);
    fwrite(&fileHeader.bfOffBits, sizeof(unsigned int), 1, file);

    // Write the info header
    fwrite(&infoHeader.biSize, sizeof(unsigned int), 1, file);
    fwrite(&infoHeader.biWidth, sizeof(int), 1, file);
    fwrite(&infoHeader.biHeight, sizeof(int), 1, file);
    fwrite(&infoHeader.biPlanes, sizeof(unsigned short), 1, file);
    fwrite(&infoHeader.biBitCount, sizeof(unsigned short), 1, file);
    fwrite(&infoHeader.biCompression, sizeof(unsigned int), 1, file);
    fwrite(&infoHeader.biSizeImage, sizeof(unsigned int), 1, file);
    fwrite(&infoHeader.biXPelsPerMeter, sizeof(int), 1, file);
    fwrite(&infoHeader.biYPelsPerMeter, sizeof(int), 1, file);
    fwrite(&infoHeader.biClrUsed, sizeof(unsigned int), 1, file);
    fwrite(&infoHeader.biClrImportant, sizeof(unsigned int), 1, file);
};