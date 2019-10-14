#include "conf_attributes.h"
#include <iostream>

std::string strip(const std::string &str) {
    size_t st, en = 0;
    st = str.find("\"");
    en = str.find("\"", st + 1);
    if (st == std::string::npos || en == std::string::npos) {
        throw std::invalid_argument("Invalid number of double quotes in params");
    }
    return str.substr(st + 1, st - en - 2);
}

ConductivityAttributes *getArgs(const std::string &filename) {
    auto *attributes = new ConductivityAttributes;
    bool until_params = true;
    std::map<std::string, double *> conf_attributes{{"density",       &attributes->density},
                                                    {"heat_capacity", &attributes->heat_capacity},
                                                    {"conductivity",  &attributes->conductivity},
                                                    {"height",        &attributes->height},
                                                    {"width",         &attributes->width},
                                                    {"delta_x",       &attributes->delta_x},
                                                    {"delta_y",       &attributes->delta_y},
                                                    {"delta_t",       &attributes->delta_t},
                                                    {"picture_t",     &attributes->picture_t},
                                                    {"iteration_max", &attributes->iteration_max}};

    std::ifstream f(filename);
    std::string line;
    while (
            getline(f, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if (line[0] == '#' || line.empty()) {
            continue;
        }
        size_t half = line.find('=');
        if (half == -1) {
            until_params = false;
            continue;
        } else if (until_params) {
            std::string name = line.substr(0, half);
            double value = std::stod(line.substr(half + 1));

            try {
                *conf_attributes[name] =
                        value;
            }
            catch (const std::exception &e) {
                throw std::invalid_argument("Invalid config argument!");
            }
        } else {
            std::string name = line.substr(0, half);
            std::string value = line.substr(half + 1);
            attributes->params[name] = strip(value);
        }
    }
    return attributes;
}
