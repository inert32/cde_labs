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
