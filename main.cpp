#include <boost/mpi.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <utility>
#include "conf_attributes.h"
#include "multiarray.h"

void read_initial_data(multiArray &T, const std::string &path) {
    std::ifstream fin(path);
    if (!fin.is_open())
        throw std::invalid_argument("Invalid path to file with initial data");
    size_t num = 0;
    while (fin >> T.data()[num++]);
}

bool check_neumann_criteria(ConductivityAttributes &args) {
    auto alpha = args.conductivity / (args.density * args.heat_capacity);
    return args.delta_t <= (std::pow(std::max(args.delta_x, args.delta_y), 2) / (4 * alpha));
}

void update_conductivity(const ConductivityAttributes &args, multiArray &arr, const size_t &from, const size_t &to,
                         boost::mpi::communicator &world) {
    //send recv updt
    double *from_row;
    double *to_row;
    size_t w_rank = world.rank();
    size_t w_size = world.size();
    if(w_rank != 1) {
        world.send(w_rank - 1, 0, arr.get_row(from + 1), arr.width());
        world.recv(w_rank - 1, 0, from_row, arr.width());
        arr.set_row(from, from_row);
    }
    if(w_rank != (w_size - 1)) {
        world.send(w_rank + 1, 0, arr.get_row(to - 1), arr.width());
        world.recv(w_rank + 1, 0, to_row, arr.width());
        arr.set_row(to, to_row);
    }

    auto alpha = args.conductivity / (args.density * args.heat_capacity);
    static multiArray old_arr = arr;
    arr.swap(old_arr);

    for (size_t i = 1; i < arr.width() - 1; i++) {
        for (size_t j = from + 1; j < to - 1; j++) {
            arr(i, j) = old_arr(i, j) + args.delta_t * alpha * (
                    (old_arr(i - 1, j) - 2 * old_arr(i, j) + old_arr(i + 1, j)) / (args.delta_x * args.delta_x) +
                    (old_arr(i, j - 1) - 2 * old_arr(i, j) + old_arr(i, j + 1)) / (args.delta_y * args.delta_y)
            );
        }
    }
}

std::pair<size_t, size_t> range_calc(const size_t &world_size, const size_t &current_proc, const size_t &height ){
    size_t start_calc = std::ceil((height / (world_size - 1.0)) * (current_proc - 1));
    size_t end_calc = std::ceil((height / (world_size - 1.0)) * (current_proc));
    start_calc = start_calc == 0 ? 0 : start_calc - 1;
    end_calc = end_calc >= height ? height : end_calc;
    return {start_calc, end_calc};
}

int main(int argc, char *argv[]) {
    boost::mpi::environment env{argc, argv};
    boost::mpi::communicator world;
    auto args = getArgs("../conf.txt");
    if (!check_neumann_criteria(args)) {
        throw std::invalid_argument("Arguments doesn't fulfill Neumann criteria");
    }
    auto T = multiArray(args.height, args.width);
    read_initial_data(T, args.input_file);
//    for (size_t i = 0; i < args.iteration_max; i++) {
//        update_conductivity(args, T);
    if (world.rank() == 0) {
//        std::cout << "Master" << std::endl;
    } else {
//        std::cout << "Slave" << std::endl;
        auto test_pair = range_calc(world.size(), world.rank(), args.height);
        std::cout << "-----------\n" << std::endl
                  << args.height << "\n" << std::endl
                  << test_pair.first << " | " << test_pair.second << std::endl;
    }
//    }
//    for (int i = 0; i < T.height(); i++) {
//        for (int j = 0; j < T.width(); j++) {
//            std::cout << T(i, j) << "\t";
//        }
//        std::cout << std::endl;
//    }
    return 0;
}