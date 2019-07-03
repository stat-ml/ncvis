#include "ncvis.h"
#include <stdio.h>
#include <iostream>

NCVis::NCVis(){
    std::cout << "NCVis::NCVis()\n";
}

NCVis::~NCVis(){
    
}

void NCVis::add_one(double* X, int N, int D){
    printf("NCVis::add_one\n");
    for (int i=0; i < N; i++){
        for (int j=0; j< D; j++){
            X[i*D+j] += 1;
        }
    }
}