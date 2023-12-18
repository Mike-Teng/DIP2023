#include<iostream>
#include<cstdio>
#include<cstdlib>
#include "imageStructs.h"

using namespace std;

void readBMPHeaders(FILE* file, BMPFILEHEADER& fileHeader, BMPINFOHEADER& infoHeader);
void writeBMPHeaders(FILE* file, BMPFILEHEADER& fileHeader, BMPINFOHEADER& infoHeader);

// enhance the sharpness of the image
void sharpnessEnhance(const RGBA** src, RGBA** dest, int srcWidth, int srcHeight, int factor) {
    // 3x3 拉普拉斯濾波器
    if(factor == 1){
        int laplacianFilter[3][3] = {
        {-1, -1, -1},
        {-1,  9, -1},
        {-1, -1, -1}
        };
        for (int i = 1; i < srcHeight - 1; ++i) {
            for (int j = 1; j < srcWidth - 1; ++j) {
                int sumR = 0, sumG = 0, sumB = 0;
                // 對每個像素應用拉普拉斯濾波器
                for (int k = -1; k <= 1; ++k) {
                    for (int l = -1; l <= 1; ++l) {
                        sumR += src[i + k][j + l].r * laplacianFilter[k + 1][l + 1];
                        sumG += src[i + k][j + l].g * laplacianFilter[k + 1][l + 1];
                        sumB += src[i + k][j + l].b * laplacianFilter[k + 1][l + 1];
                    }
                }
                // 將結果設為目標像素
                dest[i][j].r = int(min(float(255), max(float(0), float(sumR ))));
                dest[i][j].g = int(min(float(255), max(float(0), float(sumG ))));
                dest[i][j].b = int(min(float(255), max(float(0), float(sumB ))));
                dest[i][j].a = src[i][j].a;
            }
        }
    }
    else if (factor == 2){
        int laplacianFilter[3][3] = {
            {1, -2, 1},
            {-2,  5, -2},
            {1, -2, 1}
        };
        for (int i = 1; i < srcHeight - 1; ++i) {
            for (int j = 1; j < srcWidth - 1; ++j) {
                int sumR = 0, sumG = 0, sumB = 0;
                // 對每個像素應用拉普拉斯濾波器
                for (int k = -1; k <= 1; ++k) {
                    for (int l = -1; l <= 1; ++l) {
                        sumR += src[i + k][j + l].r * laplacianFilter[k + 1][l + 1];
                        sumG += src[i + k][j + l].g * laplacianFilter[k + 1][l + 1];
                        sumB += src[i + k][j + l].b * laplacianFilter[k + 1][l + 1];
                    }
                }
                // 將結果設為目標像素
                dest[i][j].r = int(min(float(255), max(float(0), float(sumR ))));
                dest[i][j].g = int(min(float(255), max(float(0), float(sumG ))));
                dest[i][j].b = int(min(float(255), max(float(0), float(sumB ))));
                dest[i][j].a = src[i][j].a;
            }
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
    int factor = std::stoi(factorStr);
    sharpnessEnhance((const RGBA**)img, outImg, imgWidth, imgHeight, factor);

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