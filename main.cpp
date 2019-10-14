#include <boost/mpi.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
    boost::mpi::environment env{argc, argv};
    boost::mpi::communicator world;
    if (world.rank() == 0) {
        std::cout << "Master" << std::endl;
    } else {
        std::cout << "Slave" << std::endl;
    }
    return 0;
}