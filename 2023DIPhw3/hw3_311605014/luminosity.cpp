#include<iostream>
#include<cstdio>
#include<cstdlib>
#include "imageStructs.h"

using namespace std;

void readBMPHeaders(FILE* file, BMPFILEHEADER& fileHeader, BMPINFOHEADER& infoHeader);
void writeBMPHeaders(FILE* file, BMPFILEHEADER& fileHeader, BMPINFOHEADER& infoHeader);

// imporve the luminosity of the image
void improveLuminosity(const RGBA** src, RGBA** dest, int srcWidth, int srcHeight, float gamma=0.5){
    for (int i = 0; i < srcHeight; ++i) {
        for (int j = 0; j < srcWidth; ++j) {

            double normalizedRed = src[i][j].r / 255.0;
            double normalizedGreen = src[i][j].g / 255.0;
            double normalizedBlue = src[i][j].b / 255.0;

            double newRed = 255.0 * pow(normalizedRed, gamma);
            double newGreen = 255.0 * pow(normalizedGreen, gamma);
            double newBlue = 255.0 * pow(normalizedBlue, gamma);

            // Ensure that the values do not exceed the maximum (255)
            dest[i][j].r = min(255, static_cast<int>(newRed));
            dest[i][j].g = min(255, static_cast<int>(newGreen));
            dest[i][j].b = min(255, static_cast<int>(newBlue));
            dest[i][j].a = src[i][j].a;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <input file> <output file> <factor>" << endl;
        return 1;
    }

    FILE* inputFile = fopen(argv[1], "rb");
    if (inputFile == NULL) {
        cout << "Error: could not open file " << argv[1] << endl;
        return 1;
    }

    FILE* outputFile = fopen(argv[2], "wb");
    if (outputFile == NULL) {
        cout << "Error: could not open file " << argv[2] << endl;
        return 1;
    }

    BMPFILEHEADER fileHeader;
    BMPINFOHEADER infoHeader;

    readBMPHeaders(inputFile, fileHeader, infoHeader);

    cout << "biSize: " << infoHeader.biSize << endl;
    cout << "biWidth: " << infoHeader.biWidth << endl;
    cout << "biHeight: " << infoHeader.biHeight << endl;
    cout << "biBitCount: " << infoHeader.biBitCount << endl;
    cout << "biSizeImage: " << infoHeader.biSizeImage << endl; 

    int imgWidth = infoHeader.biWidth;
    int imgHeight = infoHeader.biHeight;
    int bitDepth = infoHeader.biBitCount;
    RGBA **img;

    img = new RGBA*[imgHeight];
    for(int i = 0; i < imgHeight; i++){
        img[i] = new RGBA[imgWidth];
    }
    for(int i = 0; i <imgHeight; i++){
        for(int j = 0; j < imgWidth; j++){
            
            if(bitDepth == 24){
                RGB tmp;
                fread(&tmp, sizeof(RGB), 1, inputFile);
                RGBA tmp_2;
                tmp_2.b = tmp.b;
                tmp_2.g = tmp.g;
                tmp_2.r = tmp.r;
                tmp_2.a = 0;
                img[i][j] = tmp_2;
            }
            if(bitDepth == 32){
                RGBA tmp;
                fread(&tmp, sizeof(RGBA), 1, inputFile);
                img[i][j] = tmp;
            }
        }
    }


    RGBA **outImg = new RGBA*[imgHeight];
    for(int i = 0; i < imgHeight; i++){
        outImg[i] = new RGBA[imgWidth];
    }  
    std::string factorStr = argv[3];
    float factor = std::stof(factorStr);
    improveLuminosity((const RGBA**)img, outImg, imgWidth, imgHeight, factor);

    // Write the info header
    fileHeader.bfSize = static_cast<int>(imgWidth * imgHeight * 4 + 54);
    infoHeader.biBitCount = 32;
    writeBMPHeaders(outputFile, fileHeader, infoHeader);
    
    for(int i = 0; i < imgHeight; i++){
        for(int j = 0; j < imgWidth; j++){
            fwrite(&outImg[i][j], sizeof(RGBA), 1, outputFile);
        }
    }
    fclose(outputFile);
    fclose(inputFile);
}