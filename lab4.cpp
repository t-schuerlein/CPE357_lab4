#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>


using namespace std;


// defining a Byte
typedef unsigned char Byte;

// bigSize according to teacher: 16
struct BITMAPFILEHEADER {
  short  bfType; //specifies the file type
   int bfSize; //specifies the bigSize in bytes of the bitmap file
   short  bfReserved1; //reserved; must be 0
   short  bfReserved2; //reserved; must be 0
   int bfOffBits; //species the offset in bytes from the bitmapfileheader to the bitmap bits
};

struct BITMAPINFOHEADER {
   int biSize; //specifies the number of bytes required by the struct
   int  biWidth; //specifies width in pixels
   int  biHeight; //species height in pixels
   short  biPlanes; //specifies the number of color planes, must be 1
   short  biBitCount; //specifies the number of bit per pixel
   int biCompression; //spcifies the type of compression
   int biSizeImage; //bigSize of image in bytes
   int  biXPelsPerMeter; //number of pixels per meter in x axis
   int  biYPelsPerMeter; //number of pixels per meter in y axis
   int biClrUsed; //number of colors used by th ebitmap
   int biClrImportant; //number of colors that are important
};

int ByWidthPadding(int pixWidth){
    int bigByWidth = pixWidth * 3;
    if(bigByWidth %4 == 0){

    } else{
        int extra = bigByWidth % 4;
        int padding = 4 - extra;
        bigByWidth += padding;
    }

    return bigByWidth;
}

unsigned char get_green(unsigned char *arr, int x, int y, int byWidth, int arrSize){
    if( arrSize <= (x*3) + 2 + y * byWidth ){
        return 255/2;
    }
    return arr[x*3 + y * byWidth];
}

unsigned char get_blue(unsigned char *arr, int x, int y, int byWidth, int arrSize){
    if( arrSize <= (x*3) + 2 + y * byWidth ){
        return 255/2;
    }
    return arr[(x*3)+1 + y * byWidth];
}

unsigned char get_red(unsigned char *arr, int x, int y, int byWidth, int arrSize){
 
    if( arrSize <= (x*3) + 2 + y * byWidth ){
        return 255/2;
    }
    return arr[(x*3)+2 + y * byWidth];
}



void readHeader(BITMAPFILEHEADER &bfh, BITMAPINFOHEADER &bih, FILE *file){
    // reading in bitmap file header
    fread(&bfh.bfType, 2, 1, file);
    fread(&bfh.bfSize, 4, 1, file);
    fread(&bfh.bfReserved1, sizeof(short), 1, file);
    fread(&bfh.bfReserved2, sizeof(short), 1, file);
    fread(&bfh.bfOffBits, sizeof(int), 1, file);
    // reading in bitmap info header
    fread(&bih, sizeof(bih), 1, file);

}



// arguments: programName, img1, img2, ratio, outputFileName
int main(int argc, char *argv[]){


    char* path = "lion.bmp";
   
    float brightness = 0.3;
    int isParallel = 1;
    char* pathRes = "res.bmp";

    
    FILE *file = fopen(path, "rb");
    // FILE *file2 = fopen(path2, "rb");
    FILE *res = fopen(pathRes, "wb");


   

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    readHeader(bfh, bih, file);

 

    int piWidth = bih.biWidth;
    int width = ByWidthPadding(bih.biWidth);
    int height = bih.biHeight;


    int size = width * height;
    Byte *idata = new Byte[size];
    fread(idata, size, 1, file);
    Byte *result = new Byte[size];


// if parallel = 0, do NOT fork
if (isParallel == 0){



    for(int y = 0; y < height; y++){
        for(int x = 0; x < piWidth; x++){
            
            Byte g = get_green(idata, x, y, width, width*height);
            Byte b = get_blue(idata, x, y, width, width *height);
            Byte r = get_red(idata,x, y, width, width * height);

            g += (int)(brightness * 255);
            b += (int) (brightness * 255);
            r += (int) (brightness * 255);

            g = min(g, (Byte) 255);
            b = min(b, (Byte) 255);
            r = min(r, (Byte) 255);

            idata[(x*3)+0 + y * width] = g;
            idata[(x*3)+1 + y * width] = b ;
            idata[(x*3)+2 + y * width] = r;
        }
    }
    result = idata;
} // else parallel = 0
else{

    // mmap the idata
    Byte *p = (Byte*) mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
   

    float div = height / 2.0;
    int split = floor(div);
    // make the child word
    if(fork() == 0){

        for(int y = split; y < height; y ++){
            for(int x = 0; x < piWidth; x++){
                Byte g = get_green(idata, x, y, width, size);
                Byte b = get_blue(idata, x, y, width, size);
                Byte r = get_red(idata, x, y,width, size);

                g += (int)(brightness * 255);
                b += (int) (brightness * 255);
                r += (int) (brightness * 255);

                g = min(g, (Byte) 255);
                b = min(b, (Byte) 255);
                r = min(r, (Byte) 255);

                p[(x*3)+0 + y * width] = g;
                p[(x*3)+1 + y * width] = b ;
                p[(x*3)+2 + y * width] = r;


            }
        }
        return 0;
    }


    // parent code here
    // for(int y = 0; y < split; y ++){
    //         for(int x = 0; x < piWidth; x++){
    //             Byte g = get_green(idata, x, y, width, size);
    //             Byte b = get_blue(idata, x, y, width, size);
    //             Byte r = get_red(idata, x, y,width, size);

    //             g += (int)(brightness * 255);
    //             b += (int) (brightness * 255);
    //             r += (int) (brightness * 255);

    //             g = min(g, (Byte) 255);
    //             b = min(b, (Byte) 255);
    //             r = min(r, (Byte) 255);

    //             p[(x*3)+0 + y * width] = g;
    //             p[(x*3)+1 + y * width] = b ;
    //             p[(x*3)+2 + y * width] = r;


    //         }
    //     }

    cout << "parent tingz" << endl;

    wait(0);
    result = p;

}

    fwrite(&bfh.bfType, 2, 1, res);
    fwrite(&bfh.bfSize, 4, 1, res);
    fwrite(&bfh.bfReserved1, sizeof(short), 1 , res);
    fwrite(&bfh.bfReserved2, sizeof(short), 1, res);
    fwrite(&bfh.bfOffBits, sizeof(int), 1, res);

    fwrite(&bih, sizeof(bih), 1, res);

    fwrite(result, size, 1, res);

  
    fclose(file);
 
    fclose(res);
    return 0;
}