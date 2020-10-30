#include <iostream>
// #include "simplicialset.hpp"
#include <table.hpp>
#include <array>
#include <numeric>
#include <tuple>

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
    std::size_t row_size = 5;

    Table<long> table(row_size, 1);
    long n_rows, n_cols;

    std::tie(n_rows, n_cols) = table.shape();
    std::cout << "shape " << n_rows << ", " << n_cols << std::endl;
    std::cout << table << std::endl;

    std::vector<int> row(row_size);
    std::iota(row.begin(), row.end(), 0);

    table.resize(3);
    table[1] = row;

    std::cout << table << std::endl;

    std::cout << table.begin()[1] << std::endl;
    
    table.sort();

    std::cout << table << std::endl;

    return 0;
}