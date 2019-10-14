#ifndef LAB3_CONF_ATTRIBUTES_H
#define LAB3_CONF_ATTRIBUTES_H

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

struct ConductivityAttributes {
    std::map<std::string, std::string> params;
    double density;
    double heat_capacity;
    double conductivity;
    double height;
    double width;
    double delta_x;
    double delta_y;
    double delta_t;
    double picture_t;
    double iteration_max;
};

std::string strip(const std::string &str);

ConductivityAttributes *getArgs(const std::string &filename);

#endif //LAB3_CONF_ATTRIBUTES_H
