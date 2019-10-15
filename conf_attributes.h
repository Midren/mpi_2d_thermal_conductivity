#ifndef LAB3_CONF_ATTRIBUTES_H
#define LAB3_CONF_ATTRIBUTES_H

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

struct ConductivityAttributes {
    size_t height;
    size_t width;
    double density;
    double heat_capacity;
    double conductivity;
    double delta_x;
    double delta_y;
    double delta_t;
    size_t picture_t;
    size_t iteration_max;
    std::string input_file;
    std::string output_files_dir;
};

ConductivityAttributes getArgs(const std::string &filename);

#endif //LAB3_CONF_ATTRIBUTES_H
