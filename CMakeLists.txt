cmake_minimum_required(VERSION 3.14)
project(mpi_2d_thermal_conductivity)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)
find_package(Boost COMPONENTS mpi REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${MPI_CXX_INCLUDE_PATH})
add_executable(mpi_2d_thermal_conductivity main.cpp)
target_link_libraries(mpi_2d_thermal_conductivity ${Boost_LIBRARIES} ${MPI_CXX_LIBRARIES})