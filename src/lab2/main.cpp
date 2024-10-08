// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <cmath>
#include <fstream>
#include "../build.h"
#include "../common.h"
#include "mesh.h"
#include "display.h"

double func1(const double x) {
    if (x > 2.0 && x < 5.0) return 1.0;
    else return 0.0;
}

// Запись слоя сетки в файл
void write_line(std::fstream& file, const double* line, const size_t size, const size_t layer) {
    file << layer << ":\t";
    for (size_t x = 0; x < size; x++)
        file << line[x] << '\t';
    file << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "Lab2 " << build_version << " " << build_git << std::endl;

    // Разбор параметров
    const auto cli = parse_cli(argc, argv);
    const double x_start = double_from_cli_map(cli, "x_start", 0.0);
    const double x_end = double_from_cli_map(cli, "x_end", 10.0);
    const double t_start = double_from_cli_map(cli, "t_start", 0.0);
    const double t_end = double_from_cli_map(cli, "t_end", 10.0);
    const double x_step = double_from_cli_map(cli, "x_step", 0.1);
    const double t_step = double_from_cli_map(cli, "t_step", 0.1);
    const double speed = double_from_cli_map(cli, "speed", 0.9);
    const std::string output_path = str_from_cli_map(cli, "output", "lab2.txt");
    const bool run_sdl = bool_from_cli_map(cli, "graph", false);

    // Расчет размера сетки
    const double courant = speed * x_step / t_step;
    std::cout << "Courant: " << courant << std::endl;
    size_t x_size = (size_t)((x_end - x_start) / x_step);
    size_t t_size = (size_t)((t_end - t_start) / t_step);
    
    std::fstream output(output_path, std::ios::out);
    std::cout << "Saving output to: " << output_path << std::endl;
    output << "X Step\t" << x_step << "\tT Step\t" << t_step << "\tCourant\t" << courant << std::endl;
    try {
        // Создаем сетку
        mesh_t m(x_size, t_size);

        // Настройка оси абсцисс
        m.set_layer_x(x_step);
        auto abs = m.get_layer_x();
        output << "X:\t";
        for (size_t x = 0; x < x_size; x++)
            output << abs[x] << '\t';
        output << std::endl;

        // Первый слой
        m.set_layer_t0(func1);
        write_line(output, m.get_layer(0), x_size, 0);

        std::cout << "Begin mesh calculations...";
        // Расчет сетки
        for (size_t t = 1; t < t_size; t++) {
            m.calc_layer(t, courant);
            write_line(output, m.get_layer(t), x_size, t);
        }
        std::cout << "done." << std::endl;

        if (run_sdl) {
        try {
            sdl_display disp;
            bool run = true;
            size_t frame = 0;

            while (run) {
                disp.show_frame(m, frame, t_step);
                switch (handle_kbd()) {
                case sdl_events::quit:
                    run = false;
                    break;
                case sdl_events::previous:
                    if (frame > 0) frame--;
                    break;
                case sdl_events::next:
                    if (frame < t_size - 1) frame++;
                    break;
                default: continue;
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "SDL: Error: " << e.what() << std::endl;
        }
    }
    }
    catch (const std::bad_alloc&) { // Слишком большая сетка
        unsigned long long int req_gbytes = sizeof(double) * x_size * t_size / 1024 / 1024 / 1024;
        std::cerr << "mesh: Error: Out of memory. Mesh size [" << x_size << " X " << t_size << "]"
        << " will require " << req_gbytes << " GB RAM" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "mesh: Error: " << e.what() << std::endl;
    }
    output.close();

    return 0;
}
