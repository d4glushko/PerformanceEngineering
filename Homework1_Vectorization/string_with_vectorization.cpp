#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <string.h>
#include <string>
#include <cassert>
#include <bitset>

using namespace std;

#define N 100000000

const float sec_const = 1000000.0;

int find_first_occurence_intrinsic(const char* str, const char* substr) {
    int len_str = strlen(str);
    int len_substr = strlen(substr);
    const __m256i first_elem_to_find = _mm256_set1_epi8(substr[0]);
    for (int i = 0; i < len_str; i += 32) {
        const __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(str + i));
        const __m256i eq = _mm256_cmpeq_epi8(first_elem_to_find, block);
        int mask = _mm256_movemask_epi8(eq);
        int bitpos = __builtin_ctz(mask);
        while (mask != 0 && i + bitpos + 1 < len_str && len_str - (i + bitpos) >= len_substr) {
            if (memcmp(str + i + bitpos + 1, substr + 1, len_substr - 1) == 0) {
                return i + bitpos;
            }
            mask = mask & (mask - 1); 
            bitpos = __builtin_ctz(mask);
        }
    }
    return -1;
}

int main(int argc, char *argv[]) {
    clock_t start_t;
    clock_t end_t;
    clock_t clock_delta;
    double clock_delta_sec;

    char const *substr = "memory";
    char *str = (char *)malloc(N * sizeof(char));
    int position = N - strlen(substr) - 1;
    int const char_start = 48;
    int const char_end = 122;
    for (int i = 0, j = char_start; i < position; i++){
        str[i] = (char)j;
        if (j == char_end) {
            j = char_start;
        } 
        else
        {
            j++;
        }
        
    }
    for (int i = 0; i < strlen(substr); i++){
        str[i + position] = substr[i];
    }
    str[N - 1] = '\0';

    start_t = clock();
    int first_occurence = find_first_occurence_intrinsic(str, substr);
    end_t = clock();
    clock_delta = end_t - start_t;
    clock_delta_sec = (double) (clock_delta / sec_const);

    // printf("Str: %s\n", str);  // commented because string is too long
    printf("Substr: %s\n", substr);
    printf("Position: %d\n", position);
    printf("First occurence (must be equal to position): %d\n", first_occurence);

    printf("Simple find first occurence: \t %.6f \t\n", clock_delta_sec);
}
