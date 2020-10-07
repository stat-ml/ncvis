#include <iostream>
#include "simplicialset.hpp"

int main(int argc, char* argv[]){
    long N = 4, M = 4;
    const long E[] = {0, 1,
                0, 3,
                2, 1,
                2, 3
    };

    SimplicialSet S(4, 4, E, 2);
    for(auto e : S.get_simplices(1)) std::cout << e << ", ";
    std::cout << std::endl;
    
    return 0;
}