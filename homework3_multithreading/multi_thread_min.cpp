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

void array_min(unsigned char const & result, unsigned char* array, unsigned long start, unsigned long end) {
    unsigned char & min = const_cast<unsigned char &>(result);
    min = 255;
    for (unsigned long i = start; i < end; i++) {
        if (array[i] < min) {
            min = array[i];
        }
    }
}

unsigned char multithread_array_min(unsigned char* array, unsigned long size) {
    unsigned int threads_number = thread::hardware_concurrency() - 3;
    if (threads_number < 1) {
        threads_number = 1;
    }

    unsigned long items_per_one_thread = int(size / threads_number);
    
    unsigned char *results = (unsigned char *)malloc(threads_number * sizeof(unsigned char));
    thread threads[threads_number];

    threads[threads_number - 1] = thread(
        array_min,
        ref(results[threads_number - 1]),
        ref(array),
        (threads_number - 1) * items_per_one_thread,
        size
    );
    for (int i = 0; i < threads_number - 1; i++) {
        threads[i] = thread(
            array_min, 
            ref(results[i]), 
            ref(array),
            i * items_per_one_thread,
            (i + 1) * items_per_one_thread
        );
    }

    unsigned char result = 255;
    for (int i = 0; i < threads_number; i++){
        threads[i].join();
        result += results[i];
        if (results[i] < result) {
            result = results[i];
        }
    }
    free(results);
    return result;
}

int main(int argc, char *argv[]) {
    const char *filename = "1.bmp";

    BMPInfo bmpInfo = readBMP(filename);

    unsigned long one_color_channel_data_size = bmpInfo.size / 3;
    unsigned char* one_color_channel_data = new unsigned char[one_color_channel_data_size];

    for(unsigned long i = 0; i < one_color_channel_data_size; i++)
    {
        one_color_channel_data[i] = bmpInfo.data[3 * i];
    }

    auto t_start = std::chrono::high_resolution_clock::now();
    unsigned char min = multithread_array_min(one_color_channel_data, one_color_channel_data_size);
    auto t_end = std::chrono::high_resolution_clock::now();

    printf("Sum: \t %d \t\n", min);
    printf("CPU min: \t %.6f ms \t\n", std::chrono::duration<double, std::milli>(t_end-t_start).count());

    free(one_color_channel_data);
}
