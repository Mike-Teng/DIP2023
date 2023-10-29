#include<iostream>
#include<cstdio>
#include<cstdlib>
#include "imageStructs.h"

using namespace std;

void readBMPHeaders(FILE* file, BMPFILEHEADER& fileHeader, BMPINFOHEADER& infoHeader);
void writeBMPHeaders(FILE* file, BMPFILEHEADER& fileHeader, BMPINFOHEADER& infoHeader);

// denoise the image
void denoise(const RGBA** src, RGBA** dest, int srcWidth, int srcHeight, int factor = 1) {
    int f = factor;
    for (int i = f; i < srcHeight - f; ++i) {
        for (int j = f; j < srcWidth - f; ++j) {
            int sumR = 0, sumG = 0, sumB = 0;

            // Apply a simple averaging filter
            for (int k = -f; k <= f; ++k) {
                for (int l = -f; l <= f; ++l) {
                    sumR += src[i + k][j + l].r;
                    sumG += src[i + k][j + l].g;
                    sumB += src[i + k][j + l].b;
                }
            }

            int pixels = (2 * f + 1) * (2 * f + 1);
            // Calculate the average pixel value
            dest[i][j].r = static_cast<uint8_t>(sumR / pixels);
            dest[i][j].g = static_cast<uint8_t>(sumG / pixels);
            dest[i][j].b = static_cast<uint8_t>(sumB / pixels);
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
    denoise((const RGBA**)img, outImg, imgWidth, imgHeight, factor);

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