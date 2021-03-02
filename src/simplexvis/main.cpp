#include <iostream>
// #include "simplicialset.hpp"
#include <table.hpp>
#include <array>
#include <numeric>
#include <tuple>
#include <tbb/task_arena.h>
#include "pcg_random.hpp"

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
    std::size_t row_size = 10;

    Table<long> table(row_size, 0);
    long n_rows, n_cols;
    
    std::tie(n_rows, n_cols) = table.shape();
    std::cout << "shape " << n_rows << ", " << n_cols << std::endl;
    // std::cout << table << std::endl;

    pcg64 pcg(0);
    std::uniform_int_distribution<long> dist{1, 5};

    for (std::size_t i=0; i < 10; ++i){
        std::vector<long> v(row_size);
        std::generate(v.begin(), v.end(), [&]{return dist(pcg);});
        table.push_back(v);
    }

    // std::vector<int> row(row_size);
    // std::iota(row.begin(), row.end(), 0);
    // table.push_back(row);
    std::cout << table << std::endl;

    // table.resize(10000);
    // table[1000] = row;

    // std::cout << table << std::endl;

    // std::cout << table.begin()[1] << std::endl;
    
    // tbb::task_arena(tbb::task_arena::attach());

    table.sort();
    // table.parallel_sort();

    // std::vector<std::size_t> pos(std::get<0>(table.shape()));
    // std::cout << std::get<0>(table.shape()) << std::endl;
    // std::iota(pos.begin(), pos.end(), 0);
    // std::swap(pos[0], pos[1000]);
    
    std::cout << table << std::endl;

    // Table<long> new_table(3, 5);
    // new_table[0] = std::vector<long>({1, 3, 2});
    // new_table[1] = std::vector<long>({3, 2, 2});
    // new_table[2] = std::vector<long>({2, 1, 2});
    // new_table[3] = std::vector<long>({3, 3, 2});
    // new_table[4] = std::vector<long>({1, 1, 1});

    // std::cout << new_table << std::endl;
    
    // std::vector<std::size_t> positions = {0, 1, 2, 3, 4};
    // new_table.permute_parallel(positions);

    // std::cout << new_table << std::endl;

    return 0;
}