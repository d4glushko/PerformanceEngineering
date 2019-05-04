#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>

using namespace std;

#define N 800

const float sec_const = 1000000.0;

void add_simple(double** a, double** b, double** res) {
    int iterations = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            iterations++;
            res[i][j] = a[i][j] + b[i][j];
        }
    }
    printf("Add iterations: %d\n", iterations);
}

void mult_simple(double** a, double** b, double** res) {
    int iterations = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
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
    
    // printf("res1:\n");
    // print_matrix(res1);

    printf("Simple mult and add: \t %.6f \t\n", clock_delta_sec);
}
