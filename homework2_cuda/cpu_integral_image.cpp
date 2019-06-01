#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>

using namespace std;

const float sec_const = 1000000.0;

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

u_int64_t* cpu_integral_image(u_int64_t* array, int size, int width) {
    u_int64_t *result = (u_int64_t *)malloc(size * sizeof(u_int64_t));
    for (int i = 0; i < size; i++) {
        result += array[i];
    }
    return result;
}



int main(int argc, char *argv[]) {
    clock_t start_t;
    clock_t end_t;
    clock_t clock_delta;
    double clock_delta_sec;
    srand(1);

    const char *filename = "2.bmp";

    BMPInfo bmpInfo = readBMP(filename);

    int r_channel_data_size = bmpInfo.size / 3;
    u_int64_t* r_channel_data = new u_int64_t[r_channel_data_size];

    for(int i = 0; i < r_channel_data_size; i += 1)
    {
        r_channel_data[i] = (u_int64_t)bmpInfo.data[3 * i];
        printf("a[%d]: R: %d G: %d B: %d \n", i, bmpInfo.data[3 * i], bmpInfo.data[2 * i], bmpInfo.data[1 * i]);
    }

    start_t = clock();
    // int sum = cpu_array_sum(r_channel_data, r_channel_data_size);
    end_t = clock();
    clock_delta = end_t - start_t;
    clock_delta_sec = (double) (clock_delta / sec_const);

    // printf("Sum: \t %d \t\n", sum);
    printf("CPU sum: \t %.6f \t\n", clock_delta_sec);
}
