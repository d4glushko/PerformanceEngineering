#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <fstream>
#include <stdint.h>
#include <cmath>
#include <mpi.h>

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
    unsigned char* data = (unsigned char *)malloc(size * sizeof(unsigned char)); // allocate 3 bytes per pixel
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

unsigned char array_min(unsigned char* array, unsigned long start, unsigned long end) {
    unsigned char res = 255;
    for (unsigned long i = start; i < end; i++) {
        if (array[i] < res) {
            res = array[i];
        }
    }
    return res;
}

unsigned char mpi_array_min(unsigned char* array, unsigned long size, int my_id, int master_id, int num_procs) {
    unsigned char min = 255;
    unsigned char res = 255;
    MPI_Status status;
    unsigned long items_per_one_device = int(size / num_procs);
    if (my_id == num_procs - 1) {
        min = array_min(array, my_id * items_per_one_device, size);
    } else {
        min = array_min(array, my_id * items_per_one_device, (my_id + 1) * items_per_one_device);
    }
    res = min;
    if (my_id != master_id) {
        MPI_Send (&min, 1, MPI_UINT64_T, master_id, 1, MPI_COMM_WORLD);
    } else {
        for (int i = 1; i < num_procs; i++) {
            MPI_Recv(&min, 1, MPI_UINT64_T, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            if (min < res) {
                res = min;
            }
        }
    }

    return res;
}

int main(int argc, char *argv[]) {
    clock_t start_t;
    clock_t end_t;
    clock_t clock_delta;
    double clock_delta_msec;
    int master = 0;
    int my_id;
    int numprocs;
    BMPInfo bmpInfo;
    unsigned long one_color_channel_data_size;
    unsigned char* one_color_channel_data;

    MPI_Init ( &argc, &argv );
    MPI_Comm_size ( MPI_COMM_WORLD, &numprocs );
    MPI_Comm_rank ( MPI_COMM_WORLD, &my_id );
    if (my_id == master)
    {
        const char *filename = "1.bmp";

        bmpInfo = readBMP(filename);

        one_color_channel_data_size = bmpInfo.size / 3;
    }

    MPI_Bcast(&one_color_channel_data_size, 1, MPI_UNSIGNED_LONG, master, MPI_COMM_WORLD);
    one_color_channel_data = (unsigned char *)malloc(one_color_channel_data_size * sizeof(unsigned char));

    if (my_id == master)
    {
        for(unsigned long i = 0; i < one_color_channel_data_size; i++)
        {
            one_color_channel_data[i] = bmpInfo.data[3 * i];
        }
    }
    if (my_id == master) {
        start_t = clock();
    }
    MPI_Bcast(one_color_channel_data, one_color_channel_data_size, MPI_UNSIGNED_CHAR, master, MPI_COMM_WORLD);
    unsigned char min = mpi_array_min(one_color_channel_data, one_color_channel_data_size, my_id, master, numprocs);

    if (my_id == master) {
        end_t = clock();

        clock_delta = end_t - start_t;
        clock_delta_msec = (double) (clock_delta / msec_const);

        printf("Min: \t %d \t\n", min);
        printf("CPU min: \t %.6f ms \t\n", clock_delta_msec);
    }

    free(one_color_channel_data);
}
