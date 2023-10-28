#include<iostream>
#include<cstdio>
#include<cstdlib>

using namespace std;

// file header
struct BMPFILEHEADER{
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
};
// info header
struct BMPINFOHEADER{
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
// RGB struct for 24-bit image
struct RGB{
    uint8_t b;
    uint8_t g;
    uint8_t r;
};
// RGBA struct for 32-bit image
struct RGBA{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

RGBA resolution_24(RGB pixel, uint8_t bit){
    /* 
    down resolution for 24-bit image
    input pixel: the pixel data of the image
    input bit: the bit to keep
    output resolution: the pixel data after down resolution
    */
    RGBA resolution;
    resolution.b = pixel.b & bit;
    resolution.g = pixel.g & bit;
    resolution.r = pixel.r & bit;
    resolution.a = 0;
    return resolution;
}

RGBA resolution_32(RGBA pixel, uint8_t bit){
    /*
    down resolution for 32-bit image
    input pixel: the pixel data of the image
    input bit: the bit to keep
    output resolution: the pixel data after down resolution
    */
    RGBA resolution;
    resolution.b = pixel.b & bit;
    resolution.g = pixel.g & bit;
    resolution.r = pixel.r & bit;
    resolution.a = pixel.a & bit;
    return resolution;
}

int main(int argc, char *argv[]){
    FILE *fpr;
    FILE *fpw;
    BMPFILEHEADER fileHeader;
    BMPINFOHEADER infoHeader;

    int imgWidth, imgHeight, bitDepth;
    
    // Read the file
    fpr = fopen(argv[1], "rb");

    if(fpr == NULL){
        cout << "Wrong File!" << endl;
        return 0;
    }

    // Read the file header
    fread(&fileHeader.bfType, sizeof(unsigned short), 1, fpr);
    fread(&fileHeader.bfSize, sizeof(unsigned int), 1, fpr);
    fread(&fileHeader.bfReserved1, sizeof(unsigned short), 1, fpr);
    fread(&fileHeader.bfReserved2, sizeof(unsigned short), 1, fpr);
    fread(&fileHeader.bfOffBits, sizeof(unsigned int), 1, fpr);

    // Read the info header
    fread(&infoHeader.biSize, sizeof(unsigned int), 1, fpr);
    fread(&infoHeader.biWidth, sizeof(int), 1, fpr);
    fread(&infoHeader.biHeight, sizeof(int), 1, fpr);
    fread(&infoHeader.biPlanes, sizeof(unsigned short), 1, fpr);
    fread(&infoHeader.biBitCount, sizeof(unsigned short), 1, fpr);
    fread(&infoHeader.biCompression, sizeof(unsigned int), 1, fpr);
    fread(&infoHeader.biSizeImage, sizeof(unsigned int), 1, fpr);
    fread(&infoHeader.biXPelsPerMeter, sizeof(int), 1, fpr);
    fread(&infoHeader.biYPelsPerMeter, sizeof(int), 1, fpr);
    fread(&infoHeader.biClrUsed, sizeof(unsigned int), 1, fpr);
    fread(&infoHeader.biClrImportant, sizeof(unsigned int), 1, fpr);

    cout << "biBitCount: " << infoHeader.biBitCount << endl;

    imgWidth = infoHeader.biWidth;
    imgHeight = infoHeader.biHeight;
    bitDepth = infoHeader.biBitCount;

    // create 3 images with different resolution
    RGBA **img_6;
    RGBA **img_4;
    RGBA **img_2;

    img_6 = new RGBA*[imgHeight];
    img_4 = new RGBA*[imgHeight];
    img_2 = new RGBA*[imgHeight];
    for(int i = 0; i < imgHeight; i++){
        img_6[i] = new RGBA[imgWidth];
        img_4[i] = new RGBA[imgWidth];
        img_2[i] = new RGBA[imgWidth];
    }

    for(int i = 0; i <imgHeight; i++){
        for(int j = 0; j < imgWidth; j++){
            if(bitDepth == 24){
                RGB pixel;
                fread(&pixel, sizeof(RGB), 1, fpr);
                // store the pixel data after down resolution
                img_6[i][j] = resolution_24(pixel, 0b11111100);
                img_4[i][j] = resolution_24(pixel, 0b11110000);
                img_2[i][j] = resolution_24(pixel, 0b11000000);
            }
            if (bitDepth == 32){
                RGBA pixel;
                fread(&pixel, sizeof(RGBA), 1, fpr);
                // store the pixel data after down resolution
                img_6[i][j] = resolution_32(pixel, 0b11111100);
                img_4[i][j] = resolution_32(pixel, 0b11110000);
                img_2[i][j] = resolution_32(pixel, 0b11000000);
            }
            
        }
    }
    
    
    // Flip the image
    // flipHorizontal(img, imgWidth, imgHeight, infoHeader.biBitCount / 8);

    fpw = fopen(argv[2], "wb");

    if(fpw == NULL){
        cout << "Wrong File!" << endl;
        return 0;
    }
    // Write the file header

    fwrite(&fileHeader.bfType, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfSize, sizeof(unsigned int), 1, fpw);
    fwrite(&fileHeader.bfReserved1, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfReserved2, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfOffBits, sizeof(unsigned int), 1, fpw);
    
    // Write the info header
    fwrite(&infoHeader.biSize, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biWidth, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biHeight, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biPlanes, sizeof(unsigned short), 1, fpw);
    infoHeader.biBitCount = 32;
    fwrite(&infoHeader.biBitCount, sizeof(unsigned short), 1, fpw);
    fwrite(&infoHeader.biCompression, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biSizeImage, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biXPelsPerMeter, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biYPelsPerMeter, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biClrUsed, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biClrImportant, sizeof(unsigned int), 1, fpw);

    for(int i = 0; i < imgHeight; i++){
        for(int j = 0; j < imgWidth; j++){
            fwrite(&img_6[i][j], sizeof(RGBA), 1, fpw);
        }
    }
    fclose(fpw);
    

    fpw = fopen(argv[3], "wb");
     if(fpw == NULL){
        cout << "Wrong File!" << endl;
        return 0;
    }
    // Write the file header

    fwrite(&fileHeader.bfType, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfSize, sizeof(unsigned int), 1, fpw);
    fwrite(&fileHeader.bfReserved1, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfReserved2, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfOffBits, sizeof(unsigned int), 1, fpw);
    
    // Write the info header
    fwrite(&infoHeader.biSize, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biWidth, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biHeight, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biPlanes, sizeof(unsigned short), 1, fpw);
    infoHeader.biBitCount = 32;
    fwrite(&infoHeader.biBitCount, sizeof(unsigned short), 1, fpw);
    fwrite(&infoHeader.biCompression, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biSizeImage, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biXPelsPerMeter, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biYPelsPerMeter, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biClrUsed, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biClrImportant, sizeof(unsigned int), 1, fpw);

    for(int i = 0; i < imgHeight; i++){
        for(int j = 0; j < imgWidth; j++){
            fwrite(&img_4[i][j], sizeof(RGBA), 1, fpw);
        }
    }
    fclose(fpw);


    fpw = fopen(argv[4], "wb");
     if(fpw == NULL){
        cout << "Wrong File!" << endl;
        return 0;
    }
    // Write the file header

    fwrite(&fileHeader.bfType, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfSize, sizeof(unsigned int), 1, fpw);
    fwrite(&fileHeader.bfReserved1, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfReserved2, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfOffBits, sizeof(unsigned int), 1, fpw);
    
    // Write the info header
    fwrite(&infoHeader.biSize, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biWidth, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biHeight, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biPlanes, sizeof(unsigned short), 1, fpw);
    infoHeader.biBitCount = 32;
    fwrite(&infoHeader.biBitCount, sizeof(unsigned short), 1, fpw);
    fwrite(&infoHeader.biCompression, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biSizeImage, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biXPelsPerMeter, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biYPelsPerMeter, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biClrUsed, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biClrImportant, sizeof(unsigned int), 1, fpw);

    for(int i = 0; i < imgHeight; i++){
        for(int j = 0; j < imgWidth; j++){
            fwrite(&img_2[i][j], sizeof(RGBA), 1, fpw);
        }
    }
    fclose(fpw);


    fclose(fpr);
}
