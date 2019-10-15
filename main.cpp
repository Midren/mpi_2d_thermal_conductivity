#include <boost/mpi.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "conf_attributes.h"
#include "multiarray.h"

void read_initial_data(multiArray &T, const std::string &path) {
    std::ifstream fin(path);
    size_t num = 0;
    while (fin >> T.data()[num++]);
}

bool check_neumann_criteria(ConductivityAttributes &args) {
    auto alpha = args.conductivity / (args.density * args.heat_capacity);
    return args.delta_t <= std::max(args.delta_x, args.delta_y) / (4 * alpha);
}

void update_conductivity(ConductivityAttributes &args, multiArray &arr) {
    auto alpha = args.conductivity / (args.density * args.heat_capacity);
    static multiArray old_arr = arr;
    arr.swap(old_arr);

    for (size_t i = 1; i < arr.width() - 1; i++) {
        for (size_t j = 1; j < arr.height() - 1; j++) {
            arr(i, j) = old_arr(i, j) + args.delta_t * alpha * (
                    (arr(i - 1, j) - 2 * arr(i, j) + arr(i + 1, j)) / (args.delta_x * args.delta_x) +
                    (arr(i, j - 1) - 2 * arr(i, j) + arr(i, j + 1)) / (args.delta_y * args.delta_y)
            );
        }
    }
}

int main(int argc, char *argv[]) {
    boost::mpi::environment env{argc, argv};
    boost::mpi::communicator world;
    auto args = getArgs("../conf.txt");
    if (!check_neumann_criteria(args)) {
        std::cerr << "Doesn't fulfill Neumann criteria" << std::endl;
        exit(1);
    }
    auto T = multiArray(args.height, args.width);
    read_initial_data(T, args.input_file);
    update_conductivity(args, T);
    if (world.rank() == 0) {
        std::cout << "Master" << std::endl;
    } else {
        std::cout << "Slave" << std::endl;
    }

    for (int i = 0; i < T.height(); i++) {
        for (int j = 0; j < T.width(); j++) {
            std::cout << T(i, j) << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}