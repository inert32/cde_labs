// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <fstream>
#include "../build.h"
#include "../common.h"

// Парсер файла задания
cli_map parse_task_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.good()) throw std::runtime_error("Failed to read task file");

    std::string line, command, args;
    cli_map ret;
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
        if (command == "area" || command == "source") ret["area"] = args;
        else std::cout << "parse: Unknown command: " << command << std::endl;
    }

    return ret;
}

int main(int argc, char** argv) {
    std::cout << "Lab3 " << build_version << " " << build_git << std::endl;
    auto cli = parse_cli(argc, argv);

    try {
        const std::string task_file = str_from_cli_map(cli, "file");
        auto conf = parse_task_file(task_file);
        std::cout << "Task file parsed." << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "parse: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}