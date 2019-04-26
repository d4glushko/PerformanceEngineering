// fibo.c
// Simple exmple for profilig workshop
#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>

using namespace std;

#define N 100

const float sec_const = 1000000.0;

void add_simple(double** a, double** b, double** res) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            res[i][j] = a[i][j] + b[i][j];
        }
    }
}

void mult_simple(double** a, double** b, double** res) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < N; k++) {
                sum += a[i][k] * b[k][j];
            }
            res[i][j] = sum;
        }
    }
}

void mult_and_add_simple(double** a, double** b, double** c, double** d, double** res) {
    double **res1 = (double **)malloc(N * sizeof(double *));
    double **res2 = (double **)malloc(N * sizeof(double *)); 
    for (size_t i = 0; i < N; i++) {
        res1[i] = (double *)malloc(N * sizeof(double));
        res2[i] = (double *)malloc(N * sizeof(double));
    }
    mult_simple(a, b, res1);
    mult_simple(c, d, res2);
    add_simple(res1, res2, res);
}

__m256d add_intrinsic_elem(double* a, double* b) {
    __m256d veca = _mm256_setr_pd(a[0], a[1], a[2], a[3]);
    __m256d vecb = _mm256_setr_pd(b[0], b[1], b[2], b[3]);
    __m256d result = _mm256_add_pd(veca, vecb);

    return result;
}

void add_intrinsic(double** a, double** b, double** res) {
    int pack_size = 4;
    int packs_count = (int)(N / pack_size);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < packs_count; j += pack_size) {
            double a_pack[pack_size] = {a[i][j+0], a[i][j+1], a[i][j+2], a[i][j+3]};
            double b_pack[pack_size] = {b[i][j+0], b[i][j+1], b[i][j+2], b[i][j+3]};
            __m256d sum = add_intrinsic_elem(a_pack, b_pack);
            double* sum_result = (double*)&sum;
            res[i][j+0] = sum_result[0];
            res[i][j+1] = sum_result[1];
            res[i][j+2] = sum_result[2];
            res[i][j+3] = sum_result[3];
        }
        int start = pack_size * packs_count;
        for (int j = start; j < N; j++) {
            res[i][j] = a[i][j] + b[i][j];
        }
    }
}

__m256d mult_intrinsic_elem(double* a, double* b, __m256d sum) {
    __m256d veca = _mm256_setr_pd(a[0], a[1], a[2], a[3]);
    __m256d vecb = _mm256_setr_pd(b[0], b[1], b[2], b[3]);
    __m256d result = _mm256_fmadd_pd(veca, vecb, sum);

    return result;
}

void mult_intrinsic(double** a, double** b, double** res) {
    int pack_size = 4;
    int packs_count = (int)(N / pack_size);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < packs_count; j += pack_size) {
            __m256d sum = _mm256_setr_pd(0,0,0,0);
            for (int k = 0; k < N; k++) {
                double a_pack[pack_size] = {a[i][k], a[i][k], a[i][k], a[i][k]};
                double b_pack[pack_size] = {b[k][j+0], b[k][j+1], b[k][j+2], b[k][j+3]};
                sum = mult_intrinsic_elem(a_pack, b_pack, sum);
            }
            double* sum_result = (double*)&sum;
            res[i][j+0] = sum_result[0];
            res[i][j+1] = sum_result[1];
            res[i][j+2] = sum_result[2];
            res[i][j+3] = sum_result[3];
        }
        int start = pack_size * packs_count;
        for (int j = start; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < N; k++) {
                sum += a[i][k] * b[k][j];
            }
            res[i][j] = sum;
        }
    }
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

    double **res1 = (double **)malloc(N * sizeof(double *));
    for (size_t i = 0; i < N; i++) {
        res1[i] = (double *)malloc(N * sizeof(double));
    }
    start_t = clock();
    mult_and_add_simple(a, b, c, d, res1);
    end_t = clock();
    clock_delta = end_t - start_t;
    clock_delta_sec = (double) (clock_delta / sec_const);
    printf("Simple mult and add: \t %.6f \t\n", clock_delta_sec);
    // printf("res1:\n");
    // print_matrix(res1);

    double **res2 = (double **)malloc(N * sizeof(double *));
    for (size_t i = 0; i < N; i++) {
        res2[i] = (double *)malloc(N * sizeof(double));
    }
    start_t = clock();
    mult_and_add_intrinsic(a, b, c, d, res2);
    end_t = clock();
    clock_delta = end_t - start_t;
    clock_delta_sec = (double) (clock_delta / sec_const);
    printf("Intrinsic mult and add: \t %.6f \t\n", clock_delta_sec);
    // printf("res2:\n");
    // print_matrix(res2);
}