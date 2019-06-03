#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <fstream>
#include <stdint.h>
#include <cmath>

using namespace std;

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
    BMPInfo bmpInfo = {data, size};

    return bmpInfo;
}

void array_sum(u_int64_t const & result, unsigned char* array, unsigned long start, unsigned long end) {
    u_int64_t & res = const_cast<u_int64_t &>(result);
    res = 0;
    for (unsigned long i = start; i < end; i++) {
        res += array[i];
    }
}

u_int64_t multithread_array_sum(unsigned char* array, unsigned long size) {
    unsigned int threads_number = thread::hardware_concurrency() - 3;
    if (threads_number < 1) {
        threads_number = 1;
    }

    unsigned long items_per_one_thread = ceil(size / (double)threads_number);
    
    u_int64_t *results = (u_int64_t *)malloc(threads_number * sizeof(u_int64_t));
    thread threads[threads_number];

    for (int i = 0; i < threads_number - 1; i++) {
        threads[i] = thread(
            array_sum, 
            ref(results[i]), 
            ref(array),
            i * items_per_one_thread,
            (i + 1) * items_per_one_thread
        );
    }
    threads[threads_number - 1] = thread(
        array_sum,
        ref(results[threads_number - 1]),
        ref(array),
        (threads_number - 1) * items_per_one_thread,
        size
    );

    u_int64_t result = 0;
    for (int i = 0; i < threads_number; i++){
        threads[i].join();
        result += results[i];
    }
    free(results);
    return result;
}

int main(int argc, char *argv[]) {
    const char *filename = "2.bmp";

    BMPInfo bmpInfo = readBMP(filename);

    unsigned long one_color_channel_data_size = bmpInfo.size / 3;
    unsigned char* one_color_channel_data = new unsigned char[one_color_channel_data_size];

    for(int i = 0; i < one_color_channel_data_size; i++)
    {
        one_color_channel_data[i] = bmpInfo.data[3 * i];
    }

    auto t_start = std::chrono::high_resolution_clock::now();
    u_int64_t sum = multithread_array_sum(one_color_channel_data, one_color_channel_data_size);
    auto t_end = std::chrono::high_resolution_clock::now();

    printf("Sum: \t %lu \t\n", sum);
    printf("CPU sum: \t %.6f ms \t\n", std::chrono::duration<double, std::milli>(t_end-t_start).count());

    free(one_color_channel_data);
}
