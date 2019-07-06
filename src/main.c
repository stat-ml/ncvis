#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "ncvis.h"

double doubleRand() {
  return ((double)rand()) / ((double)RAND_MAX + 1.0);
}

int main(int argc, char** argv){
  srand(time(NULL));
  int N=10, d=2;
  double* X = (double*)malloc(N*d*sizeof(*X));

  for (int i=0; i<N*d; i++){
    X[i] = doubleRand();
  }

  add_one(X, N, d);
  printf("-----------------\n");
  for (int i=0; i<N; i++){
    printf("[");
    for (int j=0; j<d; j++){
      printf("%lf ", X[d*i+j]);
    }
    printf("]\n");
  }
  printf("-----------------\n");
  
  free(X);
  return 0;
}