#include <stdio.h>
#include "ncvis.h"

int main(int argc, char** argv){
    NCVis vis;
    double x = 0;
    vis.add_one(&x, 1, 1);
    printf("New x: %lf\n", x);

    return 0;
}