#include "ncvis.h"
#include <stdio.h>

NCVis::NCVis(){
    printf("Hello!\n");
}

NCVis::~NCVis(){
    
}

void NCVis::add_one(double* X, int N, int D){
    for (int i=0; i < N; i++){
        for (int j=0; j< D; j++){
            X[i*D+j] += 1;
        }
    }
}