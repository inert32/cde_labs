// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <cmath>
#include <fstream>
#include "../build.h"
#include "../common.h"

double func1(const double x) {
    if (x > 2.0 && x < 5.0) return 1.0;
    else return 0.0;
}

// Сетка
class mesh_t {
public:
    mesh_t(const size_t x_size, const size_t t_size) {
        _x_size = x_size; _t_size = t_size;
        mesh = new double*[t_size];
        for (size_t t = 0; t < t_size; t++) {
            mesh[t] = new double[x_size];
            for (size_t x = 0; x < x_size; x++) mesh[t][x] = 0.0;
        }
        std::cout << "mesh: debug: spawned!" << std::endl;
    }
    ~mesh_t() {
        for (size_t t = 0; t < _t_size; t++) delete[] mesh[t];
        delete[] mesh;
        std::cout << "mesh: debug: removed!" << std::endl;
    }

    double** get_mesh() const {
        return mesh;
    }

    struct mesh_size_t {const size_t x; const size_t t; };

    mesh_size_t get_size() const {
        return { _x_size, _t_size };
    }

private:
    double** mesh;
    size_t _x_size, _t_size;
};

int main(int argc, char** argv) {
    std::cout << "Lab2 " << build_version << " " << build_git << std::endl;
    const auto cli = parse_cli(argc, argv);
    const double x_start = double_from_cli_map(cli, "x_start", 0.0);
    const double x_end = double_from_cli_map(cli, "x_end", 10.0);
    const double t_start = double_from_cli_map(cli, "t_start", 0.0);
    const double t_end = double_from_cli_map(cli, "t_end", 10.0);
    const double x_step = double_from_cli_map(cli, "x_step", 0.1);
    const double t_step = double_from_cli_map(cli, "t_step", 0.1);
    const double speed = double_from_cli_map(cli, "speed", 0.9);

    std::cout << "Courant: " << speed * x_step / t_step << std::endl;
    size_t x_size = (size_t)((x_end - x_start) / x_step);
    size_t t_size = (size_t)((t_end - t_start) / t_step);

    // Создаем сетку
    try {
        mesh_t mesg(x_size, t_size);
    }
    catch (const std::bad_alloc&) { // Слишком большая сетка
        unsigned long long int req_bytes = sizeof(double) * x_size * t_size;
        unsigned long long int req_gbytes = req_bytes / 1024 / 1024 / 1024;
        std::cerr << "mesh: Out of memory. Mesh size [" << x_size << " X " << t_size << "]"
        << " will require " << req_gbytes << " GB RAM" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "mesh: Error: " << e.what() << std::endl;
    }

    return 0;
}
