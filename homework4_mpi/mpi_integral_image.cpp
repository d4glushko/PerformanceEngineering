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
    unsigned char* data = (unsigned char *)malloc(size * sizeof(unsigned char)); // allocate 3 bytes per pixel
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

unsigned char* integral_image_rows(unsigned char* array, int  width, int start_row, int end_row) {
    for (int i = start_row; i < end_row; i++) {
        for (int j = 1; j < width; j++) {
            array[i * width + j] += array[i * width + j - 1];
        }
    }
    return array;
}

unsigned char* integral_image_columns(unsigned char* array, int width, int  height, int start_col, int end_col) {
    for (int i = start_col; i < end_col; i++) {
        for (int j = 1; j < height; j++) {
            array[j * width + i] += array[(j - 1) * width + i];
        }
    }
    return array;
}

unsigned char* mpi_integral_image(unsigned char* array, unsigned long size, int width, int height, int my_id, int master_id, int num_procs) {
    unsigned char *result = (unsigned char *)malloc(size * sizeof(unsigned char));
    unsigned char *integral_image_res = (unsigned char *)malloc(size * sizeof(unsigned char));
    for (unsigned long i = 0; i < size; i++) {
        result[i] = array[i];
    }
    MPI_Status status;

    unsigned long rows_per_one_device = int(height / num_procs);

    int start_row, end_row;
    start_row = my_id * rows_per_one_device;
    if (my_id == num_procs - 1) {
        end_row = height;
    } else {
        end_row = (my_id + 1) * rows_per_one_device;
    }
    result = integral_image_rows(result, width, start_row, end_row);
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < width; j++) {
            integral_image_res[i * width + j] = result[i * width + j];
        }
    }

    if (my_id != master_id) {
        MPI_Send(result, size, MPI_UNSIGNED_CHAR, master_id, 1, MPI_COMM_WORLD);
    } else {
        for (int i = 1; i < num_procs; i++) {
            MPI_Recv(result, size, MPI_UNSIGNED_CHAR, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            start_row = status.MPI_SOURCE * rows_per_one_device;
            if (status.MPI_SOURCE == num_procs - 1) {
                end_row = height;
            } else {
                end_row = (status.MPI_SOURCE + 1) * rows_per_one_device;
            }
            for (int i = start_row; i < end_row; i++) {
                for (int j = 0; j < width; j++) {
                    integral_image_res[i * width + j] = result[i * width + j];
                }
            }
        }
    }

    MPI_Bcast(integral_image_res, size, MPI_UNSIGNED_CHAR, master_id, MPI_COMM_WORLD);
    for (int i = 0; i < size; i++) {
        result[i] = integral_image_res[i];
    }
    
    unsigned long cols_per_one_device = int(width / num_procs);

    int start_col, end_col;
    start_col = my_id * cols_per_one_device;
    if (my_id == num_procs - 1) {
        end_col = width;
    } else {
        end_col = (my_id + 1) * cols_per_one_device;
    }
    result = integral_image_columns(result, width, height, start_col, end_col);

    for (int i = start_col; i < end_col; i++) {
        for (int j = 1; j < height; j++) {
            integral_image_res[j * width + i] = result[j * width + i];
        }
    }

    if (my_id != master_id) {
        MPI_Send(result, size, MPI_UNSIGNED_CHAR, master_id, 1, MPI_COMM_WORLD);
    } else {
        for (int i = 1; i < num_procs; i++) {
            MPI_Recv(result, size, MPI_UNSIGNED_CHAR, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            start_col = status.MPI_SOURCE * cols_per_one_device;
            if (status.MPI_SOURCE == num_procs - 1) {
                end_col = width;
            } else {
                end_col = (status.MPI_SOURCE + 1) * cols_per_one_device;
            }
            for (int i = start_col; i < end_col; i++) {
                for (int j = 1; j < height; j++) {
                    integral_image_res[j * width + i] = result[j * width + i];
                }
            }
        }
    }

    // printf("Device %d, start_col %d, end_col %d, result array ", my_id, start_col, end_col);
    // for (int i = 0; i < size; i++) {
    //     printf("%u,", integral_image_res[i]);
    // }
    // printf("\n");
    return integral_image_res;
}

int main(int argc, char *argv[]) {
    clock_t start_t;
    clock_t end_t;
    clock_t clock_delta;
    double clock_delta_msec;
    int master = 0;
    int my_id;
    int numprocs;
    int width;
    int height;
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
        width = bmpInfo.width;
        height = bmpInfo.height;
    }

    // one_color_channel_data_size = 12;
    // width = 3;
    // height = 4;

    MPI_Bcast(&one_color_channel_data_size, 1, MPI_UNSIGNED_LONG, master, MPI_COMM_WORLD);
    MPI_Bcast(&width, 1, MPI_INT, master, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, master, MPI_COMM_WORLD);
    one_color_channel_data = (unsigned char *)malloc(one_color_channel_data_size * sizeof(unsigned char));

    if (my_id == master)
    {
        for(unsigned long i = 0; i < one_color_channel_data_size; i++)
        {
            // one_color_channel_data[i] = i;
            one_color_channel_data[i] = bmpInfo.data[3 * i];
        }
    }
    if (my_id == master) {
        start_t = clock();
    }
    MPI_Bcast(one_color_channel_data, one_color_channel_data_size, MPI_UNSIGNED_CHAR, master, MPI_COMM_WORLD);
    unsigned char* result = mpi_integral_image(one_color_channel_data, one_color_channel_data_size, width, height, my_id, master, numprocs);

    if (my_id == master) {
        end_t = clock();

        clock_delta = end_t - start_t;
        clock_delta_msec = (double) (clock_delta / msec_const);

        printf("Last elements: %d, %d, %d\n", result[one_color_channel_data_size-3], result[one_color_channel_data_size-2], result[one_color_channel_data_size-1]);
        printf("CPU integral image: \t %.6f ms \t\n", clock_delta_msec);
    }

    free(one_color_channel_data);
}
