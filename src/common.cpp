// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "common.h"

// Разбор аргументов командной строки
std::map<std::string, std::string, std::less<>> parse_cli(int argc, char** argv) {
    std::map<std::string, std::string, std::less<>> ret;
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
