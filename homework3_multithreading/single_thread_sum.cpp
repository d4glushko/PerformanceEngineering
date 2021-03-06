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
};

BMPInfo readBMP(const char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    size_t a = fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
    a = fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

    for(i = 0; i < size; i += 3)
    {
            unsigned char tmp = data[i];
            data[i] = data[i+2];
            data[i+2] = tmp;
    }
    BMPInfo bmpInfo = {data, size};

    return bmpInfo;
}

u_int64_t cpu_array_sum(unsigned char* array, unsigned long size) {
    u_int64_t result = 0;
    for (unsigned long i = 0; i < size; i++) {
        result += array[i];
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
    u_int64_t sum = cpu_array_sum(one_color_channel_data, one_color_channel_data_size);
    end_t = clock();

    clock_delta = end_t - start_t;
    clock_delta_msec = (double) (clock_delta / msec_const);

    printf("Sum: \t %lu \t\n", sum);
    printf("CPU sum: \t %.6f ms \t\n", clock_delta_msec);

    free(one_color_channel_data);
}
