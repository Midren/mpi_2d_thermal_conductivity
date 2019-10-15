#include <boost/mpi.hpp>
#include <iostream>
#include <fstream>

#include "conf_attributes.h"
#include "multiarray.h"

void read_initial_data(multiArray &T, const std::string &path) {
    std::ifstream fin(path);
    size_t num = 0;
    while (fin >> T.data()[num++]);
}

int main(int argc, char *argv[]) {
//    boost::mpi::environment env{argc, argv};
//    boost::mpi::communicator world;
    auto args = getArgs("../conf.txt");
    auto T = multiArray(args.height, args.width);
    read_initial_data(T, args.input_file);
//    if (world.rank() == 0) {
//        std::cout << "Master" << std::endl;
//    } else {
//        std::cout << "Slave" << std::endl;
//    }

    for (int i = 0; i < T.height(); i++) {
        for (int j = 0; j < T.width(); j++) {
            std::cout << T(i, j) << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}