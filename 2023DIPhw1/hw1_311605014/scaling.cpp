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

void bilinearInterpolation(const RGBA** src, RGBA** dest, int srcWidth, 
                            int srcHeight, int destWidth, int destHeight) {
    /* 
    Bilinear interpolation
    input src: source image
    input srcWidth: width of source image
    input srcHeight: height of source image
    input destWidth: width of destination image
    input destHeight: height of destination image
    output dest: destination image
    */ 
    for (int x = 0; x < destHeight; ++x) {
        for (int y = 0; y < destWidth; ++y) {
            // Calculate the coordinates of the source image
            float srcX = x * static_cast<float>(srcHeight - 1) / (destHeight - 1);
            float srcY = y * static_cast<float>(srcWidth - 1) / (destWidth - 1);
            int x0 = static_cast<int>(srcX);
            int y0 = static_cast<int>(srcY);
            int x1 = min(x0 + 1, srcHeight - 1);
            int y1 = min(y0 + 1, srcWidth - 1);
            // alpha and beta are interpolation parameters
            float alpha = srcX - x0;
            float beta = srcY - y0;

            dest[x][y].r = static_cast<uint8_t>((1 - alpha) * (1 - beta) * src[x0][y0].r +
                                                alpha * (1 - beta) * src[x0][y1].r +
                                                (1 - alpha) * beta * src[x1][y0].r +
                                                alpha * beta * src[x1][y1].r);
            dest[x][y].g = static_cast<uint8_t>((1 - alpha) * (1 - beta) * src[x0][y0].g +
                                                alpha * (1 - beta) * src[x0][y1].g +
                                                (1 - alpha) * beta * src[x1][y0].g +
                                                alpha * beta * src[x1][y1].g);
            dest[x][y].b = static_cast<uint8_t>((1 - alpha) * (1 - beta) * src[x0][y0].b +
                                                alpha * (1 - beta) * src[x0][y1].b +
                                                (1 - alpha) * beta * src[x1][y0].b +
                                                alpha * beta * src[x1][y1].b);
            dest[x][y].a = 0; // Assuming alpha is not used in source image.
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
    
    // Scaling Bilinear Interpolation
    int destWidth = static_cast<int>(imgWidth * 2);
    int destHeight = static_cast<int>(imgHeight * 2);
    
    RGBA **scaleImg = new RGBA*[destHeight];
    for(int i = 0; i < destHeight; i++){
        scaleImg[i] = new RGBA[destWidth];
    }

    bilinearInterpolation((const RGBA**)img, scaleImg, imgWidth, imgHeight, destWidth, destHeight);

    fpw = fopen(argv[2], "wb");
    if(fpw == NULL){
        cout << "Wrong File!" << endl;
        return 0;
    }
    // Write the file header

    fileHeader.bfSize = static_cast<int>(destWidth * destHeight * 4 + 54);
    fwrite(&fileHeader.bfType, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfSize, sizeof(unsigned int), 1, fpw);
    fwrite(&fileHeader.bfReserved1, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfReserved2, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfOffBits, sizeof(unsigned int), 1, fpw);
    
    // Write the info header
    infoHeader.biWidth = destWidth;
    infoHeader.biHeight = destHeight;
    infoHeader.biBitCount = 32;
    fwrite(&infoHeader.biSize, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biWidth, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biHeight, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biPlanes, sizeof(unsigned short), 1, fpw);
    fwrite(&infoHeader.biBitCount, sizeof(unsigned short), 1, fpw);
    fwrite(&infoHeader.biCompression, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biSizeImage, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biXPelsPerMeter, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biYPelsPerMeter, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biClrUsed, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biClrImportant, sizeof(unsigned int), 1, fpw);

    for(int i = 0; i < destHeight; i++){
        for(int j = 0; j < destWidth; j++){
            fwrite(&scaleImg[i][j], sizeof(RGBA), 1, fpw);
        }
    }
    

    fclose(fpw);

    // Scaling Bilinear Interpolation
    int destWidthDown = static_cast<int>(imgWidth / 1.5);
    int destHeightDown = static_cast<int>(imgHeight / 1.5);
    

    RGBA **scaleDownImg = new RGBA*[destHeightDown];
    for(int i = 0; i < destHeight; i++){
        scaleDownImg[i] = new RGBA[destWidthDown];
    }

    bilinearInterpolation((const RGBA**)img, scaleDownImg, imgWidth, imgHeight, destWidthDown, destHeightDown);

    fpw = fopen(argv[3], "wb");
    if(fpw == NULL){
        cout << "Wrong File!" << endl;
        return 0;
    }
    // Write the file header

    fileHeader.bfSize = static_cast<int>(destWidthDown * destHeightDown * 4 + 54);
    fwrite(&fileHeader.bfType, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfSize, sizeof(unsigned int), 1, fpw);
    fwrite(&fileHeader.bfReserved1, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfReserved2, sizeof(unsigned short), 1, fpw);
    fwrite(&fileHeader.bfOffBits, sizeof(unsigned int), 1, fpw);
    
    // Write the info header
    infoHeader.biWidth = destWidthDown;
    infoHeader.biHeight = destHeightDown;
    infoHeader.biBitCount = 32;
    fwrite(&infoHeader.biSize, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biWidth, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biHeight, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biPlanes, sizeof(unsigned short), 1, fpw);
    fwrite(&infoHeader.biBitCount, sizeof(unsigned short), 1, fpw);
    fwrite(&infoHeader.biCompression, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biSizeImage, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biXPelsPerMeter, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biYPelsPerMeter, sizeof(int), 1, fpw);
    fwrite(&infoHeader.biClrUsed, sizeof(unsigned int), 1, fpw);
    fwrite(&infoHeader.biClrImportant, sizeof(unsigned int), 1, fpw);

    for(int i = 0; i < destHeightDown; i++){
        for(int j = 0; j < destWidthDown; j++){
            fwrite(&scaleDownImg[i][j], sizeof(RGBA), 1, fpw);
        }
    }
    
    fclose(fpw);


    fclose(fpr);
}
