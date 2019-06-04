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
   int width;
   int height;
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
    BMPInfo bmpInfo = {data, size, width, height};

    return bmpInfo;
}

void integral_image_rows(unsigned char* array, int  width, int start_row, int end_row) {
    for (int i = start_row; i < end_row; i++) {
        for (int j = 1; j < width; j++) {
            array[i * width + j] += array[i * width + j - 1];
        }
    }
}

void integral_image_columns(unsigned char* array, int width, int  height, int start_col, int end_col) {
    for (int i = start_col; i < end_col; i++) {
        for (int j = 1; j < height; j++) {
            array[j * width + i] += array[(j - 1) * width + i];
        }
    }
}

unsigned char* multithread_integral_image(unsigned char* array, unsigned long size, int width, int height) {
    unsigned char *result = (unsigned char *)malloc(size * sizeof(unsigned char));
    for (unsigned long i = 0; i < size; i++) {
        result[i] = array[i];
    }

    unsigned int threads_number = thread::hardware_concurrency() - 3;
    if (threads_number < 1) {
        threads_number = 1;
    }

    unsigned long rows_per_one_thread = int(height / threads_number);
    
    thread threads[threads_number];

    threads[threads_number - 1] = thread(
        integral_image_rows,
        ref(result),
        width,
        (threads_number - 1) * rows_per_one_thread,
        height
    );
    for (int i = 0; i < threads_number - 1; i++) {
        threads[i] = thread(
            integral_image_rows,
            ref(result),
            width,
            i * rows_per_one_thread,
            (i + 1) * rows_per_one_thread
        );
    }

    for (int i = 0; i < threads_number; i++){
        threads[i].join();
    }

    unsigned long cols_per_one_thread = int(width / threads_number);

    threads[threads_number - 1] = thread(
        integral_image_columns,
        ref(result),
        width,
        height,
        (threads_number - 1) * cols_per_one_thread,
        width
    );
    for (int i = 0; i < threads_number - 1; i++) {
        threads[i] = thread(
            integral_image_columns,
            ref(result),
            width,
            height,
            i * cols_per_one_thread,
            (i + 1) * cols_per_one_thread
        );
    }

    for (int i = 0; i < threads_number; i++){
        threads[i].join();
    }

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
    unsigned char* result = multithread_integral_image(one_color_channel_data, one_color_channel_data_size, bmpInfo.width, bmpInfo.height);
    auto t_end = std::chrono::high_resolution_clock::now();

    printf("Last elements: %d, %d, %d\n", result[one_color_channel_data_size-3], result[one_color_channel_data_size-2], result[one_color_channel_data_size-1]);
    printf("CPU integral image: \t %.6f ms \t\n", std::chrono::duration<double, std::milli>(t_end-t_start).count());

    free(one_color_channel_data);
}
