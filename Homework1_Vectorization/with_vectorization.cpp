#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>

using namespace std;

#define N 300

const float sec_const = 1000000.0;

void add_intrinsic(double** a, double** b, double** res) {
    int pack_size = 4;
    int packs_count = (int)(N / pack_size);
    int packed_length = pack_size * packs_count;
    int iterations = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < packed_length; j += pack_size) {
            iterations++;
            __m256d veca = _mm256_setr_pd(a[i][j+0], a[i][j+1], a[i][j+2], a[i][j+3]);
            __m256d vecb = _mm256_setr_pd(b[i][j+0], b[i][j+1], b[i][j+2], b[i][j+3]);
            __m256d sum = _mm256_add_pd(veca, vecb);
            res[i][j+0] = sum[0];
            res[i][j+1] = sum[1];
            res[i][j+2] = sum[2];
            res[i][j+3] = sum[3];
        }
        for (int j = packed_length; j < N; j++) {
            iterations++;
            res[i][j] = a[i][j] + b[i][j];
        }
    }
    printf("Add iterations: %d\n", iterations);
}

void mult_intrinsic(double** a, double** b, double** res) {
    int pack_size = 4;
    int packs_count = (int)(N / pack_size);
    int packed_length = pack_size * packs_count;
    int iterations = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < packed_length; j += pack_size) {
            __m256d sum = _mm256_setr_pd(0,0,0,0);
            for (int k = 0; k < N; k++) {
                iterations++;
                __m256d veca = _mm256_setr_pd(a[i][k], a[i][k], a[i][k], a[i][k]);
                __m256d vecb = _mm256_setr_pd(b[k][j+0], b[k][j+1], b[k][j+2], b[k][j+3]);
                sum = _mm256_fmadd_pd(veca, vecb, sum);
            }
            res[i][j+0] = sum[0];
            res[i][j+1] = sum[1];
            res[i][j+2] = sum[2];
            res[i][j+3] = sum[3];
        }
        for (int j = packed_length; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < N; k++) {
                iterations++;
                sum += a[i][k] * b[k][j];
            }
            res[i][j] = sum;
        }
    }
    printf("Mult iterations: %d\n", iterations);
}

void mult_and_add_intrinsic(double** a, double** b, double** c, double** d, double** res) {
    double **res1 = (double **)malloc(N * sizeof(double *));
    double **res2 = (double **)malloc(N * sizeof(double *)); 
    for (size_t i = 0; i < N; i++) {
        res1[i] = (double *)malloc(N * sizeof(double));
        res2[i] = (double *)malloc(N * sizeof(double));
    }
    mult_intrinsic(a, b, res1);
    mult_intrinsic(c, d, res2);
    add_intrinsic(res1, res2, res);
}

double get_random(double min, double max) {
    return (max - min) * ((double)rand() / (double)RAND_MAX) + min;
}

void print_matrix(double** a) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("[%d][%d] %f\n", i, j, a[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    clock_t start_t;
    clock_t end_t;
    clock_t clock_delta;
    double clock_delta_sec;
    srand(1);

    double **a = (double **)malloc(N * sizeof(double *));
    double **b = (double **)malloc(N * sizeof(double *)); 
    double **c = (double **)malloc(N * sizeof(double *)); 
    double **d = (double **)malloc(N * sizeof(double *)); 
    for (size_t i = 0; i < N; i++) {
        a[i] = (double *)malloc(N * sizeof(double));
        b[i] = (double *)malloc(N * sizeof(double));
        c[i] = (double *)malloc(N * sizeof(double));
        d[i] = (double *)malloc(N * sizeof(double));
    }

    double min = 0, max = 100;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            a[i][j] = get_random(min,max);
            b[i][j] = get_random(min,max);
            c[i][j] = get_random(min,max);
            d[i][j] = get_random(min,max);
        }
    }
    // printf("a:\n");
    // print_matrix(a);
    // printf("b:\n");
    // print_matrix(b);
    // printf("c:\n");
    // print_matrix(c);
    // printf("d:\n");
    // print_matrix(d);

    double **res2 = (double **)malloc(N * sizeof(double *));
    for (size_t i = 0; i < N; i++) {
        res2[i] = (double *)malloc(N * sizeof(double));
    }
    start_t = clock();
    mult_and_add_intrinsic(a, b, c, d, res2);

    end_t = clock();
    clock_delta = end_t - start_t;
    clock_delta_sec = (double) (clock_delta / sec_const);

    // printf("res2:\n");
    // print_matrix(res2);

    printf("Intrinsic mult and add: \t %.6f \t\n", clock_delta_sec);
}
