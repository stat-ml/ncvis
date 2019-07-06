#include "ncvis.h"
#include <stdio.h>



void add_one(double* X, int N, int d){
    printf("NCVis::add_one\n");
    for (int i=0; i < N; i++){
        for (int j=0; j< d; j++){
            X[i*d+j] += 1;
        }
    }
}