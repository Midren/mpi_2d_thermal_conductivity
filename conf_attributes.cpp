#include "conf_attributes.h"
#include <variant>


template<class... Ts>
struct overloaded : Ts ... {
    using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::string strip(const std::string &str) {
    size_t st, en = 0;
    st = str.find("\"");
    en = str.find("\"", st + 1);
    if (st == std::string::npos || en == std::string::npos) {
        throw std::invalid_argument("Invalid number of double quotes in params");
    }
    return str.substr(st + 1, en - st - 1);
}

ConductivityAttributes getArgs(const std::string &filename) {
    auto attributes = ConductivityAttributes{};
    std::map<std::string, double *> double_conf_attributes{{"density",       &attributes.density},
                                                           {"heat_capacity", &attributes.heat_capacity},
                                                           {"conductivity",  &attributes.conductivity},
                                                           {"delta_x",       &attributes.delta_x},
                                                           {"delta_y",       &attributes.delta_y},
                                                           {"delta_t",       &attributes.delta_t}};

    std::map<std::string, size_t *> size_t_conf_attributes{{"height",        &attributes.height},
                                                           {"width",         &attributes.width},
                                                           {"picture_t",     &attributes.picture_t},
                                                           {"iteration_max", &attributes.iteration_max}};

    std::map<std::string, std::string *> string_conf_attributes{{"input_file", &attributes.input_file},
                                                                {"output_files_dir", &attributes.output_files_dir}};

    std::vector<std::variant<
            decltype(double_conf_attributes),
            decltype(size_t_conf_attributes),
            decltype(string_conf_attributes)>> confs{
            double_conf_attributes, size_t_conf_attributes,
            string_conf_attributes
    };

    std::ifstream f(filename);
    std::string line;
    while (getline(f, line)) {
        if (line[0] == '#' || line.empty()) {
            continue;
        }
        size_t half = line.find('=');
        if (half == std::string::npos) {
            continue;
        } else {
            std::string name = line.substr(0, half);
            name.erase(std::remove_if(name.begin(), name.end(), isspace), name.end());
            std::string value = line.substr(half + 1);
            try {
                for (auto &map : confs) {
                    std::visit(overloaded{
                            [name, value](std::map<std::string, double *> conf_map) {
                                if (conf_map.find(name) != conf_map.end())
                                    *conf_map[name] = stod(value);
                            },
                            [name, value](std::map<std::string, size_t *> conf_map) {
                                if (conf_map.find(name) != conf_map.end())
                                    *conf_map[name] = stoul(value);
                            },
                            [name, value](std::map<std::string, std::string *> conf_map) {
                                if (conf_map.find(name) != conf_map.end())
                                    *conf_map[name] = strip(value);
                            }
                    }, map);
                }
            }
            catch (const std::exception &e) {
                throw std::invalid_argument("Invalid config argument!");
            }
        }
    }
    return attributes;
}
