#include "ncvis.h"
#include <stdio.h>

NCVis::NCVis(){
    printf("Helo!\n");
}

NCVis::~NCVis(){
    
}

void NCVis::add_one(double* X, int N, int D){
    printf("Adding one\n");
    for (int i=0; i < N; i++){
        for (int j=0; j< D; j++){
            X[i*D+j] += 1;
        }
    }
}