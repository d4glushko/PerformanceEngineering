#include <stdio.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <stdint.h> 

#define cudaCheckErrors(msg) \
    do { \
        cudaError_t __err = cudaGetLastError(); \
        if (__err != cudaSuccess) { \
            fprintf(stderr, "Fatal error at runtime: %s (%s at %s:%d)\n", \
                msg, cudaGetErrorString(__err), \
                __FILE__, __LINE__); \
            fprintf(stderr, "*** FAILED - ABORTING\n"); \
            exit(1); \
        } \
    } while (0)

using namespace std;

typedef uint32_t DWORD;   // DWORD = unsigned 32 bit value
typedef uint16_t WORD;    // WORD = unsigned 16 bit value
typedef unsigned long LONG;   

#pragma pack(push, 1)

typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved; must be 0
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER __attribute__((__packed__));

#pragma pack(pop)

#pragma pack(push, 1)

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;  //specifies the number of bytes required by the struct
    DWORD biWidth;  //specifies width in pixels
    DWORD biHeight;  //species height in pixels
    WORD biPlanes; //specifies the number of color planes, must be 1
    WORD biBitCount; //specifies the number of bit per pixel
    DWORD biCompression;//spcifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    DWORD biXPelsPerMeter;  //number of pixels per meter in x axis
    DWORD biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by th ebitmap
    DWORD biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER __attribute__((__packed__));

#pragma pack(pop)

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr; //our file pointer
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    unsigned char *bitmapImage;  //store image data
    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return NULL;

    //read the bitmap file header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType !=0x4D42)
    {
        fclose(filePtr);
        return NULL;
    }

    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr);

    //move file point to the begging of bitmap data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    //allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    //read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader->biSizeImage, 1, filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return NULL;
    }

    //swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0;imageIdx < bitmapInfoHeader->biSizeImage;imageIdx+=3) // fixed semicolon
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //close file and return bitmap image data
    fclose(filePtr);
    return bitmapImage;
}  

__global__ void sum_reduce_simple(unsigned char *g_ivec,  int *g_ovec){
    extern __shared__ int sdata[];
    int idx = threadIdx.x;
    sdata[idx] = g_ivec[idx];
    for (unsigned int s=1; s <  blockDim.x ; s *= 2) {
        if (idx % (2*s) == 0) {
            sdata[idx] += sdata[idx + s];
        }
        __syncthreads();
    }
    g_ovec[0] = sdata[0];
}



int main()
{
 
    BITMAPINFOHEADER *bitmapInfoHeader;
    unsigned char *bitmapData;
    uint32_t image_size;
    uint32_t image_width;
    uint32_t image_height;  
    
    unsigned char *d_image;
    int *d_result, *h_result ;
    
    h_result = (int *) malloc(sizeof(int));
    
    bitmapInfoHeader = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
    bitmapData = LoadBitmapFile("/content/drive/My Drive/advance_images/GeoEye_GeoEye1_50cm_8bit_RGB_DRA_Mining_2009FEB14_8bits_sub_r_15.bmp",bitmapInfoHeader);
    printf("%x, %x, %x \n", bitmapData[0], bitmapData[1], bitmapData[2]);
    
    image_size = bitmapInfoHeader->biSizeImage;
    image_width = bitmapInfoHeader->biWidth;
    image_height = bitmapInfoHeader->biHeight;
    
    printf("%lu, %d, %d\n", image_size, image_height, image_width);
    //Here we have 3 channels of image
    
    /*****CUDA ********/
    //ALLOCATE MEM
    cudaMalloc(&d_image, 1024);
    cudaMalloc(&d_result, sizeof(int));
    cudaCheckErrors("cudaMalloc fail \n");
    cudaMemcpy(d_image, bitmapData, 1024*sizeof(unsigned char), cudaMemcpyHostToDevice);
        cudaCheckErrors("CudaMEMCPY to DEVICE fail \n");

    
    sum_reduce_simple <<< 1, 1024, 1024*sizeof(int) >>> (d_image, d_result);
    cudaCheckErrors("Kernel sum_reduce_simple CALL fail \n");
    
    cudaMemcpy(h_result, d_result, sizeof(int), cudaMemcpyDeviceToHost);
	  cudaCheckErrors("Memory copying result fail \n");
    
    //FREE MEM
    cudaFree(d_image);
    cudaFree(d_result);
    cudaCheckErrors("cudaFree fail \n");
    
    printf ("SUM is: %d\n",h_result[0]);
    //printf ("All is OK ");
    
    free(bitmapData);
    free(bitmapInfoHeader);
    return(0);
}
