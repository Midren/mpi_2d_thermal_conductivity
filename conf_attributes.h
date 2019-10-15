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
    double picture_t;
    double iteration_max;
    std::string input_file;
};

ConductivityAttributes getArgs(const std::string &filename);

#endif //LAB3_CONF_ATTRIBUTES_H
