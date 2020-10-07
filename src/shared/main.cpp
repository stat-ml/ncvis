#include <iostream>
#include <algorithm>
#include "strideiter.hpp"

int main(int argc, char* argv[]){
    int d = 2;
    int N = 100;
    int a[N];
    for (int i=0; i<N; ++i) a[i]=N-i;

    for(int e : a) std::cout << e << ", ";
    std::cout << std::endl;

    StrideIter<int*> first(a, d);
    StrideIter<int*> last(a+(N-2), d);
    
    std::sort(first,last);

    for(int e : a) std::cout << e << ", ";
    std::cout << std::endl;

    return 0;
}