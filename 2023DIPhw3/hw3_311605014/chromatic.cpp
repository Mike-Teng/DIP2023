#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<string>
#include "imageStructs.h"

using namespace std;

void readBMPHeaders(FILE* file, BMPFILEHEADER& fileHeader, BMPINFOHEADER& infoHeader);
void writeBMPHeaders(FILE* file, BMPFILEHEADER& fileHeader, BMPINFOHEADER& infoHeader);
  
void grayworld(const RGBA** input, RGBA** dest, int width, int height) {
    long double sumR = 0 ,sumG = 0, sumB = 0;
    long double avgR = 0 ,avgG = 0, avgB = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            sumR += input[i][j].r;
            sumG += input[i][j].g;
            sumB += input[i][j].b;
        }
    }

    avgR = sumR / (width * height);
    avgG = sumG / (width * height);
    avgB = sumB / (width * height);

    // Calculate scaling factors
    long double redFactor = (avgR + avgG + avgB) / (3 * avgR);
    long double greenFactor = (avgR + avgG + avgB) / (3 * avgG);
    long double blueFactor = (avgR + avgG + avgB) / (3 * avgB);
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
        // Scale each channel based on the factors
            dest[i][j].r = min(255, static_cast<int>(input[i][j].r * redFactor));
            dest[i][j].g = min(255, static_cast<int>(input[i][j].g * greenFactor));
            dest[i][j].b = min(255, static_cast<int>(input[i][j].b * blueFactor));
        }
    }
}

void applyChromaticAdaptation(const RGBA** input, RGBA** dest, int width, int height, int patchSize, int wX, int wY) {
    int sumR = 0, sumG = 0, sumB = 0;
    const long double perfectWhite[3] = {255.0, 255.0, 255.0}; // Perfect white reference
    
    // input1 Wx = 410, Wy = 220
    // input2 Wx = 103, Wy = 467
    // input3 Wx = 450, Wy = 280 
    for (int i = wX; i < wX+patchSize; ++i) {
        for (int j = wY; j < wY+patchSize; ++j) {
            sumR += input[i][j].r;
            sumG += input[i][j].g;
            sumB += input[i][j].b;
        }
    }
    int numPixels = patchSize * patchSize;
    // Calculate scaling factors
    long double redFactor = perfectWhite[0] / (sumR / numPixels);
    long double greenFactor = perfectWhite[1] / (sumG / numPixels);
    long double blueFactor = perfectWhite[2] / (sumB / numPixels);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            dest[i][j].r = min(255, static_cast<int>(input[i][j].r * redFactor));
            dest[i][j].g = min(255, static_cast<int>(input[i][j].g * greenFactor));
            dest[i][j].b = min(255, static_cast<int>(input[i][j].b * blueFactor));
            dest[i][j].a = 0;


            // if(((wX<i) && (i<wX+patchSize)) && ((wY<j) && (j<wY+patchSize))){
            //     dest[i][j].r = 0;
            //     dest[i][j].g = 0;
            //     dest[i][j].b = 0;
            // }
        }
    }
}



int main(int argc, char** argv) {
    if (argc != 5) {
        cout << "Usage: " << argv[0] << " <input file> <output file> <white x> <white y>" << endl;
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

    std::string inputname(argv[1]);

    img = new RGBA*[imgHeight];
    for(int i = 0; i < imgHeight; i++){
        img[i] = new RGBA[imgWidth];
    }
    for(int i = 0; i < imgHeight; i++){
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

    // RGBA whitePatchColor = findWhitePatch((const RGBA**)img, imgWidth, imgHeight, patchSize);

    std::string factorStr = argv[3];
    int wX = std::stoi(factorStr);
    std::string factorStr2 = argv[4];
    int wY = std::stoi(factorStr2);

    if (inputname == "input4.bmp"){
        std::cout<<"grayworld"<<std::endl;
        grayworld((const RGBA**)img, outImg, imgWidth, imgHeight);
    }
    else{
        std::cout<<"perfect white"<<std::endl;
        int patchSize = 8;
        applyChromaticAdaptation((const RGBA**)img, outImg, imgWidth, imgHeight, patchSize, wX, wY);
    }

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