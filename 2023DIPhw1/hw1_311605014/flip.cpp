#include<iostream>
#include<cstdio>
#include<cstdlib>

using namespace std;

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
// RGB struct for 24-bit image
struct RGB
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

// RGBA struct for 32-bit image
struct RGBA
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

void flipHorizontal(RGBA** pixelData, int width, int height, int bytesPerPixel) {
    /*
    Flip the image horizontally
    input pixelData: the pixel data of the image
    input width: the width of the image
    input height: the height of the image
    input bytesPerPixel: the bytes per pixel of the image
    */
    for (int x = 0; x < height; ++x) {
        for (int y = 0; y < width / 2; ++y) {
            swap(pixelData[x][y].b, pixelData[x][width - y - 1].b);
            swap(pixelData[x][y].g, pixelData[x][width - y - 1].g);
            swap(pixelData[x][y].r, pixelData[x][width - y - 1].r);
            swap(pixelData[x][y].a, pixelData[x][height - y - 1].a);
        }
    }
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

    cout << "bfType: " << fileHeader.bfType << endl;
    cout << "bfSize: " << fileHeader.bfSize << endl;
    cout << "bfReserved1: " << fileHeader.bfReserved1 << endl;
    cout << "bfReserved2: " << fileHeader.bfReserved2 << endl;
    cout<< "bfOffBits: " << fileHeader.bfOffBits << endl;

    cout << "biSize: " << infoHeader.biSize << endl;
    cout << "biWidth: " << infoHeader.biWidth << endl;
    cout << "biHeight: " << infoHeader.biHeight << endl;
    cout << "biPlanes: " << infoHeader.biPlanes << endl;
    cout << "biBitCount: " << infoHeader.biBitCount << endl;
    cout << "biCompression: " << infoHeader.biCompression << endl;
    cout << "biSizeImage: " << infoHeader.biSizeImage << endl; 
    cout << "biXPelsPerMeter: " << infoHeader.biXPelsPerMeter << endl;
    cout << "biYPelsPerMeter: " << infoHeader.biYPelsPerMeter << endl;
    cout << "biClrUsed: " << infoHeader.biClrUsed << endl;
    cout << "biClrImportant: " << infoHeader.biClrImportant << endl;

    imgWidth = infoHeader.biWidth;
    imgHeight = infoHeader.biHeight;
    bitDepth = infoHeader.biBitCount;

    RGBA **img;

    img = new RGBA*[imgHeight];
    for(int i = 0; i < imgHeight; i++){
        img[i] = new RGBA[imgWidth];
    }

    for(int i = 0; i <imgHeight; i++){
        for(int j = 0; j < imgWidth; j++){
            
            // Read the image data 
            if(bitDepth == 24){
                RGB tmp;
                fread(&tmp, sizeof(RGB), 1, fpr);
                RGBA tmp_2;
                tmp_2.b = tmp.b;
                tmp_2.g = tmp.g;
                tmp_2.r = tmp.r;
                tmp_2.a = 0;
                img[i][j] = tmp_2;
            }
            if (bitDepth == 32){
                RGBA tmp;
                fread(&tmp, sizeof(RGBA), 1, fpr);
                img[i][j] = tmp;
            }
        }
    }
    
    

    // Flip the image

    flipHorizontal(img, imgWidth, imgHeight, infoHeader.biBitCount / 8);

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
            fwrite(&img[i][j], sizeof(RGBA), 1, fpw);
        }
    }
    

    fclose(fpr);
    fclose(fpw);
}
