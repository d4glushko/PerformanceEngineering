#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>

using namespace std;

#define N 10000000

const float sec_const = 1000000.0;

double mult_simple(double* a, double* b) {
    int iterations = 0;
    double sum = 0;
    for (int i = 0; i < N; i++) {
        iterations++;
        sum += a[i] * b[i];
    }
    printf("Mult iterations: %d\n", iterations);
    return sum;
}

double mult_and_add_simple(double* a, double* b, double* c, double* d) {
    return mult_simple(a, b) + mult_simple(c, d);
}

double get_random(double min, double max) {
    return (max - min) * ((double)rand() / (double)RAND_MAX) + min;
}

void print_vector(double* a) {
    for (int i = 0; i < N; i++) {
        printf("[%d] %f\n", i, a[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    clock_t start_t;
    clock_t end_t;
    clock_t clock_delta;
    double clock_delta_sec;
    srand(2);

    double *a = (double *)malloc(N * sizeof(double));
    double *b = (double *)malloc(N * sizeof(double)); 
    double *c = (double *)malloc(N * sizeof(double)); 
    double *d = (double *)malloc(N * sizeof(double)); 

    double min = 0, max = 100;
    for (int i = 0; i < N; i++) {
        a[i] = get_random(min,max);
        b[i] = get_random(min,max);
        c[i] = get_random(min,max);
        d[i] = get_random(min,max);
    }
    // printf("a:\n");
    // print_vector(a);
    // printf("b:\n");
    // print_vector(b);
    // printf("c:\n");
    // print_vector(c);
    // printf("d:\n");
    // print_vector(d);

    start_t = clock();
    double res = mult_and_add_simple(a, b, c, d);
    end_t = clock();
    clock_delta = end_t - start_t;
    clock_delta_sec = (double) (clock_delta / sec_const);

    printf("Simple mult and add. Result: %.2f, time: %.6f \n", res, clock_delta_sec);
}
