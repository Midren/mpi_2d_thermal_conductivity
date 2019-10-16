#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <utility>
#include <thread>
#include <mutex>
#include <queue>

#include <boost/mpi.hpp>
#include "gnuplot-iostream.h"

#include "concurrent_queue.h"
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
    size_t start_calc = std::ceil((height / (world_size - 1.0)) * (current_proc - 1));
    size_t end_calc = std::ceil((height / (world_size - 1.0)) * (current_proc));
    start_calc = start_calc == 0 ? 0 : start_calc - 1;
    end_calc = end_calc >= height ? height - 1 : end_calc;
    return {start_calc, end_calc};
}

void gatherImages(ConcurrentQueue<multiArray> &concurrentQueue, ConductivityAttributes &args,
                  boost::mpi::communicator &world) {
    std::vector<size_t> szs;
    for (int i = 1; i < world.size(); i++) {
        auto[a, b] = range_calc(world.size(), i, args.height);
        size_t sz = (b - a + 1) * args.width +
                    (a == 0 ? 0 : -args.width) +
                    (b == args.height - 1 ? 0 : -args.width);
        szs.push_back(sz);
    }

    for (auto i = 0; i < args.iteration_max; i += args.picture_t) {
        auto multiArr = multiArray(args.height, args.width);
        size_t sum_size = 0;
        for (auto r = 1; r < world.size(); r++) {
            world.recv(r, 0, multiArr.data() + sum_size, szs[r - 1]);
            sum_size += szs[r - 1];
        }
        concurrentQueue.push(multiArr);
    }
    concurrentQueue.push(multiArray(0, 0));
}

int main(int argc, char *argv[]) {
    boost::mpi::environment env{argc, argv};
    boost::mpi::communicator world;
    int w_rank = world.rank();
    int w_size = world.size();
    if (w_size <= 2) {
        throw std::invalid_argument("Too small amount of processes");
    }
    std::string config_path;
    if (argc < 2) {
        config_path = "conf.txt";
    } else {
        config_path = argv[1];
    }
    auto args = getArgs(config_path);
    if (!check_neumann_criteria(args)) {
        throw std::invalid_argument("Arguments doesn't fulfill Neumann criteria");
    }

    if (world.rank() == 0) {
        ConcurrentQueue<multiArray> concurrentQueue{};
        auto thread = std::thread(gatherImages, std::ref(concurrentQueue), std::ref(args), std::ref(world));
        size_t cnt = 0;
        while (true) {
            auto T = concurrentQueue.pop();
            if (T.height() == 0)
                break;
            T.print(args.output_files_dir + "/image_" + std::to_string(cnt++) + ".txt");
        }
        thread.join();
    } else {
        auto[from, to] = range_calc(world.size(), world.rank(), args.height);
        auto T = multiArray(to - from + 1, args.width);
        read_initial_data(T, args.input_file, from, T.height() * T.width());

        auto *from_row = new double[T.width()];
        auto *to_row = new double[T.width()];

        for (auto i = 0; i < args.iteration_max; i++) {
            update_conductivity(args, T);
            if (w_rank != 1) {
                world.isend(w_rank - 1, 0, T.get_row(0), T.width());
                world.recv(w_rank - 1, 0, from_row, T.width());
                T.set_row(0, from_row);
            }
            if (w_rank != (w_size - 1)) {
                world.isend(w_rank + 1, 0, T.get_row(T.height() - 1), T.width());
                world.recv(w_rank + 1, 0, to_row, T.width());
                T.set_row(T.height() - 1, to_row);
            }
            if (i % args.picture_t == 0) {
                size_t sz = T.height() * T.width() +
                            (w_rank == world.size() - 1 ? 0 : -T.width()) +
                            (w_rank == 1 ? 0 : -T.width());
                world.isend(0, 0, T.data() + (w_rank == 1 ? 0 : T.width()), sz);
            }
        }
        delete[] from_row;
        delete[] to_row;
    }
    return 0;
}