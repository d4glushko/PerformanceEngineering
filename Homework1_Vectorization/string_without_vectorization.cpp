#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <string.h>
#include <string>

using namespace std;

#define N 1000000

const float sec_const = 1000000.0;

int find_first_occurence(char const *str, char const *substr) {
    int len_str = strlen(str);
    int len_substr = strlen(substr);
    int first_occurence = -1;
    for (int i = 0; i < len_str; i++) {
        if (str[i] == substr[0]) {
            bool found = true;
            for (int j = 1; j < len_substr; j++) {
                if (str[i+j] != substr[j]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                first_occurence = i;
                break;
            }
        } 
    }
    return first_occurence;
} 

int main(int argc, char *argv[]) {
    clock_t start_t;
    clock_t end_t;
    clock_t clock_delta;
    double clock_delta_sec;
    srand(1);
    
    std::string str_template = "This is usually the best way to declare and initialize a string. The character array is declared explicitly. \
        There is no size declaration for the array; just enough is allocated for the string, because the compiler knows \
        how long the string constant is. The compiler stores the string constant in the character array and adds a null character \
        to the end.";
    
    char const *substr = "memory";
    std::string s;

    for (int i = 0; i < 10000; i++) {
        s.append(str_template);
        if (i == 7000) {
            s.append(substr);
        }
    }

    char str[s.size() + 1];
	strcpy(str, s.c_str());

    start_t = clock();
    int first_occurence = find_first_occurence(str, substr);
    end_t = clock();
    clock_delta = end_t - start_t;
    clock_delta_sec = (double) (clock_delta / sec_const);

    // printf("Str: %s\n", str);
    printf("Substr: %s\n", substr);
    printf("First occurence: %d\n", first_occurence);

    printf("Simple find first occurence: \t %.6f \t\n", clock_delta_sec);
}
