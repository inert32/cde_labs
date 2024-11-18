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

#include "common.h"

// Разбор аргументов командной строки
cli_map parse_cli(int argc, char** argv) {
    cli_map ret;
    for (int i = 1; i < argc; i++) {
        // Разделяем аргумент на "ключ=значение"
        const std::string arg(argv[i]);
        auto pos = arg.find('=');
        if (pos == std::string::npos) continue; // Принимаем только "ключ=значение"

        // Добавляем аргумент в словарь
        ret[arg.substr(0, pos)] = arg.substr(pos + 1, arg.length());
    }

    return ret;
}

double double_from_cli_map(const cli_map& cli, const std::string& key, const double default_value) {
    const auto it = cli.find(key);
    return (it != cli.end()) ? std::stod(it->second) : default_value;
}

size_t sizet_from_cli_map(const cli_map& cli, const std::string& key, const size_t default_value) {
    const auto it = cli.find(key);
    return (it != cli.end()) ? std::stoul(it->second) : default_value;
}

std::string str_from_cli_map(const cli_map& cli, const std::string& key, const std::string& default_value) {
    const auto it = cli.find(key);
    return (it != cli.end()) ? it->second : default_value;
}

bool bool_from_cli_map(const cli_map& cli, const std::string& key, const bool default_value) {
    const auto it = cli.find(key);
    if (it != cli.end()) {
        const auto val = it->second;
        return (val == "On" || val == "on" || val == "True" || val == "true") ? true : false;
    }
    else return default_value;
}
