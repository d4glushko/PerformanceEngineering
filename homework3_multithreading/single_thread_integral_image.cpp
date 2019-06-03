#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>

using namespace std;

const float msec_const = 1000.0;

struct BMPInfo 
{ 
   unsigned char* data; 
   int size;
   int width;
   int height;
};

BMPInfo readBMP(const char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

    for(i = 0; i < size; i += 3)
    {
            unsigned char tmp = data[i];
            data[i] = data[i+2];
            data[i+2] = tmp;
    }
    BMPInfo bmpInfo = {data, size, width, height};

    return bmpInfo;
}

unsigned char* cpu_integral_image(unsigned char* array, unsigned long size, int width, int height) {
    unsigned char *result = (unsigned char *)malloc(size * sizeof(unsigned char));
    for (unsigned long i = 0; i < size; i++) {
        result[i] = array[i];
    }
    for (int i = 0; i < height; i++) {
        for (int j = 1; j < width; j++) {
            result[i * width + j] += result[i * width + j - 1];
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 1; j < height; j++) {
            result[j * width + i] += result[(j - 1) * width + i];
        }
    }
    return result;
}



int main(int argc, char *argv[]) {
    clock_t start_t;
    clock_t end_t;
    clock_t clock_delta;
    double clock_delta_msec;

    const char *filename = "1.bmp";
    BMPInfo bmpInfo = readBMP(filename);

    unsigned long one_color_channel_data_size = bmpInfo.size / 3;
    unsigned char* one_color_channel_data = new unsigned char[one_color_channel_data_size];

    for(unsigned long i = 0; i < one_color_channel_data_size; i++)
    {
        one_color_channel_data[i] = bmpInfo.data[3 * i];
    }

    start_t = clock();
    unsigned char* result = cpu_integral_image(one_color_channel_data, one_color_channel_data_size, bmpInfo.width, bmpInfo.height);
    end_t = clock();

    clock_delta = end_t - start_t;
    clock_delta_msec = (double) (clock_delta / msec_const);

    printf("Last elements: %d, %d, %d\n", result[one_color_channel_data_size-3], result[one_color_channel_data_size-2], result[one_color_channel_data_size-1]);
    printf("CPU integral image: \t %.6f ms \t\n", clock_delta_msec);

    free(one_color_channel_data);
}
