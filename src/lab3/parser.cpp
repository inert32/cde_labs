/*
Laboratory work on numerical methods for solving convection-diffusion equations
Copyright (C) 2024 Mikhaylov Alexey (inert32@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <cmath>
#include <fstream>
#include "parser.h"

bool is_known_command(const std::string& cmd) {
    return cmd == "area" || cmd == "subarea" || cmd == "source" ||
    cmd == "particles" || cmd == "energy" || cmd == "material" ||
    cmd == "burst";
}

// Проверка команд файла на нужное число аргументов
bool checkout_args(const std::string& command, const parser_opts& opts) {
    if (command == "area") return opts.size() == 2;
    if (command == "subarea") return opts.size() == 3;
    if (command == "source") return opts.size() == 4;
    if (command == "particles") return opts.size() == 1;
    if (command == "energy") return opts.size() > 0;
    if (command == "material") return opts.size() == 3;
    if (command == "burst") return opts.size() == 1;

    return false;
}

bool checkout_energy_args(const parser_opts& en) {
    float total_prob = 0.0f;
    for (auto &i : en) {
        // Распределение излучения должно иметь вид <уровень=вероятность>
        auto pos = i.find('=');
        if (pos == std::string::npos) return false;

        total_prob += std::stof(i.substr(pos + 1));
    }
    // Вероятность должна быть близка к единице (насколько можно)
    return (fabs(total_prob - 1.0f) < 0.001f) ? true : false;
}

// Поиск параметра в файле задания
const parser_line& find_config_line(const parser_data& conf, const std::string& line) {
    for (auto &i : conf) {
        if (line == i.command)
            return i;
    }
    throw std::runtime_error("parser: Error: No such line: " + line);
}

parser_data parse_task_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.good()) throw std::runtime_error("parser: Error: Failed to read task file");

    std::string line, command;
    parser_data ret;
    size_t line_number = 0;
    while (std::getline(file, line)) {
        line_number++;
        if (line.length() == 0 || line[0] == '#') continue; // Пропускаем пустые строки и комментарии

        size_t space_pos = 0, space_prev = 0, len = line.length();
        // Чтение команды
        space_pos = line.find(' ', space_pos + 1);
        command = line.substr(space_prev, space_pos - space_prev);

        if (is_known_command(command)) {
            parser_opts args;
            // Чтение аргументов
            while (space_pos < len) {
                space_prev = space_pos + 1;
                space_pos = line.find(' ', space_pos + 1);
                args.emplace_back(line.substr(space_prev, space_pos - space_prev));
            }
            if (!checkout_args(command, args))
                throw std::runtime_error("parser: Error: Not enough arguments for command " + command
                + " at line " + std::to_string(line_number));
            
            if (command == "energy" && !checkout_energy_args(args))
                throw std::runtime_error("parser: Error: Bad energy distribution at line " + std::to_string(line_number));

            ret.push_back({command, args});
        }
    }

    return ret;
}

bool check_main_area(const main_area_t& test) {
    return test.height > 0.0f && test.length > 0.0f;
}

bool check_subarea(const subarea_t& test, const main_area_t& main) {
    return test.x_start > 0.0f && test.width > 0.0f && test.x_start + test.width < main.length;
}

std::vector<subarea_t> spawn_areas(const parser_data& src, main_area_t* main_area, const mat_t& materials) {
    std::vector<subarea_t> others;

    // Ищем main_area
    auto ma = find_config_line(src, "area").args;
    main_area_t main_copy; // Локальная копия для проверок
    main_copy.length = std::stof(ma[0]);
    main_copy.height = std::stof(ma[1]);
    if (!check_main_area(main_copy))
        throw std::runtime_error("loader: Error: Check main area size");
    
    // Ищем остальные
    for (auto it = src.begin(); it != src.end(); ++it)
        if (it->command == "subarea") {
            auto& sa_raw = it->args;
            float x_start = std::stof(sa_raw[0]);
            float width = std::stof(sa_raw[1]);
            auto mat_id = sa_raw[2];
            float optics;

            try {
                optics = materials.at(mat_id).sigma;
            }
            catch (const std::exception&) {
                throw std::runtime_error("loader: Error: Unknown material: " + mat_id);
            }
            if (!(optics > 0.0f)) throw std::runtime_error("loader: Error: Check subarea " + std::to_string(others.size() + 1) + " optical density");
            
            float consume_prob = materials.at(mat_id).consume_prob;
            if (!(consume_prob > 0.01f && consume_prob < 1.0f))
                throw std::runtime_error("loader: Error: Check subarea " + std::to_string(others.size() + 1) + " absorbtion probability");

            subarea_t sa(x_start, width, main_copy.height, optics, consume_prob, mat_id);

            if (check_subarea(sa, main_copy)) others.push_back(sa);
            else throw std::runtime_error("loader: Error: Check subarea " + std::to_string(others.size() + 1) + " position and size");
        }
    
    // Сохраняем области
    *main_area = main_copy;
    return others;
}

emit_point* spawn_emitter(const parser_line& src, const parser_line& energy) {
    auto& args = src.args;
    if (args[0] == "point") {
        float x = std::stof(args[1]);
        float y = std::stof(args[2]);
        float angle = std::stof(args[3]);

        std::vector<energy_distr_t> dist;
        for (auto &i : energy.args) {
            energy_distr_t tmp;
            auto pos = i.find('=');
            tmp.level = std::stof(i.substr(0, pos));
            tmp.prob = std::stof(i.substr(pos + 1, i.length()));
            dist.push_back(tmp);
        }
        return new emit_point(x, y, angle, dist);
    }
    else throw std::runtime_error("loader: Error: Unknown emitter type");
}

size_t get_particles_count(const parser_line& src) {
    return std::stoul(src.args[0]);
}

mat_t load_materials(const parser_data& conf) {
    mat_t ret;
    for (auto &i : conf) {
        if (i.command == "material") {
            material_t m;
            auto& args = i.args;
            std::string name = args[0];
            m.sigma = std::stof(args[1]);
            m.consume_prob = std::stof(args[2]);

            ret.insert_or_assign(name, m);
        }
    }
    return ret;
}
