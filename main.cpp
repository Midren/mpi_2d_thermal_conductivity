#include <boost/mpi.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <utility>
#include "conf_attributes.h"
#include "multiarray.h"

void read_initial_data(multiArray &T, const std::string &path, size_t from, size_t max_num) {
    std::ifstream fin(path);
    if (!fin.is_open())
        throw std::invalid_argument("Invalid path to file with initial data");
    size_t num = 0;
    for (std::string line; from && std::getline(fin, line); --from) {}
    while (fin >> T.data()[num++] && (num != max_num));
}

bool check_neumann_criteria(ConductivityAttributes &args) {
    auto alpha = args.conductivity / (args.density * args.heat_capacity);
    return args.delta_t <= (std::pow(std::max(args.delta_x, args.delta_y), 2) / (4 * alpha));
}

void update_conductivity(const ConductivityAttributes &args, multiArray &arr) {
    auto alpha = args.conductivity / (args.density * args.heat_capacity);
    static multiArray old_arr = arr;
    arr.swap(old_arr);

    for (size_t i = 1; i < arr.width() - 1; i++) {
        for (size_t j = 1; j < arr.height() - 1; j++) {
            arr(i, j) = old_arr(i, j) + args.delta_t * alpha * (
                    (old_arr(i - 1, j) - 2 * old_arr(i, j) + old_arr(i + 1, j)) / (args.delta_x * args.delta_x) +
                    (old_arr(i, j - 1) - 2 * old_arr(i, j) + old_arr(i, j + 1)) / (args.delta_y * args.delta_y)
            );
        }
    }
}

std::pair<size_t, size_t> range_calc(const size_t &world_size, const size_t &current_proc, const size_t &height) {
    if (current_proc == 0) {
        return {0, height - 1};
    }
    size_t start_calc = std::ceil((height / (world_size - 1.0)) * (current_proc - 1));
    size_t end_calc = std::ceil((height / (world_size - 1.0)) * (current_proc));
    start_calc = start_calc == 0 ? 0 : start_calc - 1;
    end_calc = end_calc >= height ? height - 1 : end_calc;
    return {start_calc, end_calc};
}

int main(int argc, char *argv[]) {
    boost::mpi::environment env{argc, argv};
    boost::mpi::communicator world;
    int w_rank = world.rank();
    int w_size = world.size();

    auto args = getArgs("../conf.txt");
    if (!check_neumann_criteria(args)) {
        throw std::invalid_argument("Arguments doesn't fulfill Neumann criteria");
    }
    auto[from, to] = range_calc(world.size(), world.rank(), args.height);
    auto T = multiArray(to - from + 1, args.width);
    read_initial_data(T, args.input_file, from, T.height() * T.width());

    if (world.rank() == 0) {

    } else {
        auto *from_row = new double[T.width()];
        auto *to_row = new double[T.width()];

        for (auto i = 0; i < args.iteration_max; i++) {
            update_conductivity(args, T);
            if (w_rank != 1) {
                world.send(w_rank - 1, 0, T.get_row(0), T.width());
                world.recv(w_rank - 1, 0, from_row, T.width());
                T.set_row(0, from_row);
            }
            if (w_rank != (w_size - 1)) {
                world.send(w_rank + 1, 0, T.get_row(T.height() - 1), T.width());
                world.recv(w_rank + 1, 0, to_row, T.width());
                T.set_row(T.height() - 1, to_row);
            }
        }
        delete[] from_row;
        delete[] to_row;
    }

    if (world.rank() == 1) {
        std::cout << T.height() << " " << T.width() << std::endl;
        for (int j = 0; j < T.height(); j++) {
            for (int i = 0; i < T.width(); i++) {
                std::cout << T(i, j) << "\t";
            }
            std::cout << std::endl;
        }
    }
    return 0;
}