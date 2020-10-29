#include <iostream>
// #include "simplicialset.hpp"
#include "table.hpp"
#include <array>

int main(int argc, char* argv[]){
    // long N = 4, M = 4;
    // const long E[] = {0, 1,
    //             0, 3,
    //             2, 1,
    //             2, 3
    // };

    // SimplicialSet S(4, 4, E, 2);
    // for(auto e : S.get_simplices(1)) std::cout << e << ", ";
    // std::cout << std::endl;
    Table<long> table(5, 1);
    long n_rows, n_cols;

    std::tie(n_rows, n_cols) = table.shape();
    std::cout << "shape " << n_rows << ", " << n_cols << std::endl;
    std::cout << table << std::endl;

    // std::vector<int> row = {1,2,3,4,5};
    std::array<long, 5> row = {1,2,3,4,5};

    table.resize(3);
    table[1] = row;

    std::cout << table << std::endl;

    std::cout << table.begin()[1] << std::endl;

    std::sort(table.begin(), table.end(), [](Table<long>::Row a, Table<long>::Row b) {
        return b < a;   
    });

    std::cout << table << std::endl;

    return 0;
}