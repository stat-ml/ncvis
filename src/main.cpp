#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include "ncvis.h"

double doubleRand() {
  return ((double)rand()) / ((double)RAND_MAX + 1.0);
}

int main(int argc, char** argv){
  // srand(time(NULL));
  if (argc < 3){
    std::cout << "Usage: ncvis [number of points] [number of threads]";
    return 1;
  }
  srand(42);
  size_t N=atoi(argv[1]), d=2;
  size_t n_threads = atoi(argv[2]);
  float* X = (float*)malloc(N*d*sizeof(*X));

  for (size_t i=0; i<N*d; i++){
    X[i] = doubleRand();
  }

  ncvis::NCVis vis(d, n_threads, 15);

  vis.fit(X, N, d);

  // printf("-----------------\n");
  // for (size_t i=0; i<N; i++){
  //   printf("[");
  //   for (size_t j=0; j<d; j++){
  //     printf("%lf ", X[d*i+j]);
  //   }
  //   printf("]\n");
  // }
  // printf("-----------------\n");
  
  free(X);
  return 0;
}