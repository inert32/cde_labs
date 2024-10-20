// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <fstream>
#include "parser.h"

bool is_known_command(const std::string& cmd) {
    return cmd == "area" || cmd == "subarea" || cmd == "source" || cmd == "particles" || cmd == "energy";
}

// Проверка команд файла на нужное число аргументов
bool checkout_args(const std::string& command, const parser_opts& opts) {
    if (command == "area") return opts.size() == 2;
    if (command == "subarea") return opts.size() == 3;
    if (command == "source") return opts.size() == 4;
    if (command == "particles") return opts.size() == 1;
    if (command == "energy") return opts.size() > 0;

    return false;
}

bool checkout_energy_args(const parser_opts& en) {
    float total_prob = 0.0f;
    for (auto &i : en) {
        // Распределение излучения должно иметь вид <уровень=вероятность>
        auto pos = i.find('=');
        if (pos == std::string::npos) return false;

        // Вероятность должна быть близка к единице (насколько можно)
        total_prob += std::stof(i.substr(pos + 1));
    }
    return (fabs(total_prob - 1.0f) < 0.001f) ? true : false;
}

// Поиск параметра в файле задания
const parser_line& find_config_line(const parser_data& conf, const std::string& line) {
    for (auto &i : conf) {
        if (line == i.first)
            return i;
    }
    throw std::runtime_error("No such line: " + line);
}

parser_data parse_task_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.good()) throw std::runtime_error("Failed to read task file");

    std::string line, command;
    parser_data ret;
    parser_opts args;
    size_t line_number = 0;
    while (std::getline(file, line)) {
        line_number++;
        if (line.length() == 0 || line[0] == '#') continue; // Пропускаем пустые строки и комментарии

        size_t space_pos = 0, space_prev = 0, len = line.length();
        // Чтение команды
        space_pos = line.find(' ', space_pos + 1);
        command = line.substr(space_prev, space_pos - space_prev);

        if (is_known_command(command)) {
            // Чтение аргументов
            while (space_pos < len) {
                space_prev = space_pos + 1;
                space_pos = line.find(' ', space_pos + 1);
                args.emplace_back(line.substr(space_prev, space_pos - space_prev));
            }
            if (!checkout_args(command, args))
                throw std::runtime_error("Not enough arguments for command " + command
                + " at line " + std::to_string(line_number));
            
            if (command == "energy" && !checkout_energy_args(args))
                throw std::runtime_error("Bad energy distribution at line " + std::to_string(line_number));

            ret.emplace_back(std::make_pair(command, args));
            args.clear();
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

std::vector<subarea_t> spawn_areas(const parser_data& src, main_area_t* main_area) {
    std::vector<subarea_t> others;

    // Ищем main_area
    auto ma = find_config_line(src, "area").second;
    main_area_t main_copy; // Локальная копия для проверок
    main_copy.length = std::stof(ma[0]);
    main_copy.height = std::stof(ma[1]);
    if (!check_main_area(main_copy))
        throw std::runtime_error("loader: Error: Check main area size");
    
    // Ищем остальные
    for (auto it = src.begin(); it != src.end(); ++it)
        if (it->first == "subarea") {
            auto& sa_raw = it->second;
            subarea_t sa;
            sa.x_start = std::stof(sa_raw[0]);
            sa.width = std::stof(sa_raw[1]);
            sa.optics = std::stof(sa_raw[2]);
            if (!(sa.optics > 0.0f)) throw std::runtime_error("loader: Error: Check subarea " + std::to_string(others.size() + 1) + " optical density");

            if (check_subarea(sa, main_copy)) others.push_back(sa);
            else throw std::runtime_error("loader: Error: Check subarea " + std::to_string(others.size() + 1) + " position and size");
        }
    
    // Сохраняем области
    *main_area = main_copy;
    return others;
}

emit_point* spawn_emitter(const parser_line& src, const parser_line& energy) {
    auto& args = src.second;
    if (args[0] == "point") {
        float x = std::stof(args[1]);
        float y = std::stof(args[2]);
        float angle = std::stof(args[3]);

        std::vector<energy_distr_t> dist;
        for (auto &i : energy.second) {
            energy_distr_t tmp;
            auto pos = i.find('=');
            tmp.level = std::stof(i.substr(0, pos));
            tmp.prob = std::stof(i.substr(pos + 1, i.length()));
            dist.push_back(tmp);
        }
        return new emit_point(x, y, angle, dist);
    }
    else throw std::runtime_error("loader: Unknown emitter type");
}

size_t get_particles_count(const parser_line& src) {
    return std::stoul(src.second[0]);
}
