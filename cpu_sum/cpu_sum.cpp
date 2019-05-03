#include <stdio.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <stdint.h> 
#include <time.h>
#include <stdlib.h>

#define BLOCK 1024


int main()
{
    
    unsigned char *d_image, *h_image;
    int *d_result, *h_result ;
         
    size_t dszp = BLOCK*1024*1024;
    
    //ALLOCATE HOST MEM
    h_image = (unsigned char*)malloc(dszp);
    h_result = (int*)malloc(1024*1024*sizeof(int));    
    //DO it with MEMSET faster
    for (int i = 0; i<1024*1024*1024; i++){
        h_image[i] = 1;
    }
    
    //measure time for cpu calc.
    clock_t t1, t2;
    
	  t1 = clock();
    h_result[0] = 0;
    for (int i = 0; i<1024*1024*1024; i++){
        h_result[0] = h_result[0] + h_image[i];
    }
    
    t2 = clock();
    double time_taken  = ((double)(t2 - t1) / CLOCKS_PER_SEC *1000);
    
    printf("%.3f", time_taken);
    printf("%d", h_result[0]);
    
    free(h_image);
    free(h_result);
    return(0);
}