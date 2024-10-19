// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <fstream>
#include <iostream>
#include "parser.h"

parser_data parse_task_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.good()) throw std::runtime_error("Failed to read task file");

    std::string line, command, args;
    parser_data ret;
    size_t line_number = 0;
    while (std::getline(file, line)) {
        if (line.length() == 0 || line[0] == '#') continue; // Пропускаем пустые строки и комментарии

        auto split = line.find(' ');
        if (split == line.npos) {
            command = line;
            args.clear();
        }
        else {
            command = line.substr(0, split);
            args = line.substr(split + 1, line.length());
        }

        // Обработка команд
        if (command == "area" || command == "subarea" || command == "source" || command == "particles")
            ret.emplace_back(std::make_pair(command, args));
        else 
            std::cout << "parse: Unknown command: '" << command << "' (line " << line_number << ")" << std::endl;
        line_number++;
    }

    return ret;
}

main_area_t str_to_main_area(const std::string& str) {
    auto pos = str.find(' ');
    return { std::stof(str.substr(0, pos)), std::stof(str.substr(pos + 1, str.length())) };
}

subarea_t str_to_subarea(const std::string& str) {
    auto pos = str.find(' ');
    return { std::stof(str.substr(0, pos)), std::stof(str.substr(pos + 1, str.length())) };
}

bool check_main_area(const main_area_t& test) {
    return test.height > 0.0f && test.width > 0.0f;
}

bool check_subarea(const subarea_t& test, const main_area_t& main) {
    return test.x_start > 0.0f && test.width > 0.0f && test.x_start + test.width < main.width;
}

std::vector<subarea_t> spawn_areas(const parser_data& src, main_area_t* main_area) {
    std::vector<subarea_t> others;

    main_area_t main_copy; // Локальная копия для проверок
    // Ищем main_area
    for (auto it = src.begin(); it != src.end(); ++it)
        if (it->first == "area") {
            main_copy = str_to_main_area(it->second);
            if (!check_main_area(main_copy))
                throw std::runtime_error("loader: Error: Check main area size");
        }
    
    // Ищем остальные
    for (auto it = src.begin(); it != src.end(); ++it)
        if (it->first == "subarea") {
            subarea_t test = str_to_subarea(it->second);
            if (check_subarea(test, main_copy)) others.push_back(test);
            else throw std::runtime_error("loader: Error: Check subarea " + std::to_string(others.size() + 1) + " position and size");
        }
    
    // Сохраняем области
    *main_area = main_copy;
    return others;
}

emit_point* spawn_emitter(const parser_data& src) {
    for (auto it = src.begin(); it != src.end(); ++it)
        if (it->first == "source") {
            std::string local = it->second, type, args;
            auto space = local.find(' ');
            type = local.substr(0, space);
            args = local.substr(space + 1);

            // Выбор типа источника
            if (type == "point") {
                space = args.find(' ');
                auto space2 = args.find(' ', space + 1);
                float x = std::stof(args.substr(0, space));
                float y = std::stof(args.substr(space + 1, space2));
                float angle = std::stof(args.substr(space2 + 1));
                return new emit_point(x, y, angle);
            }
            throw std::runtime_error("loader: Unknown emitter type");
        }

    throw std::runtime_error("loader: Error: No source specified");
}

size_t get_particles_count(const parser_data& src) {
    for (auto it = src.begin(); it != src.end(); ++it)
        if (it->first == "particles")
            return std::stoul(it->second);
    
    std::cout << "loader: No particles required?" << std::endl;
    return 0;
}
