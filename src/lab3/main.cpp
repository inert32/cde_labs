// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <fstream>
#include <vector>
#include "../build.h"
#include "../common.h"
#include "display.h"

typedef std::vector<std::pair<std::string, std::string>> parser_data;
// Парсер файла задания
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
        if (command == "area" || command == "source") {
            ret.emplace_back(std::make_pair("command", args));
        }
        else std::cout << "parse: Unknown command: '" << command << "' (line " << line_number << ")" << std::endl;
        line_number++;
    }

    return ret;
}

int main(int argc, char** argv) {
    std::cout << "Lab3 " << build_version << " " << build_git << std::endl;
    auto cli = parse_cli(argc, argv);
    const bool run_sdl = bool_from_cli_map(cli, "graph", true);

    // Получаем параметры из файла задания
    parser_data conf;
    try {
        const std::string task_file = str_from_cli_map(cli, "file");
        conf = parse_task_file(task_file);
        std::cout << "Task file parsed." << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "parse: " << e.what() << std::endl;
        return 1;
    }

    try {
        if (run_sdl) {
            try {
                sdl_display disp;
                bool run = true;
                while (run) {
                    disp.show_frame();
                    switch (handle_kbd()) {
                    case sdl_events::quit:
                        run = false;
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
    catch (const std::exception& e) {
        std::cout << "parse: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

sdl_events handle_kbd() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) return sdl_events::quit;
        if (e.type == SDL_KEYDOWN) {
            switch (SDL_GetKeyFromScancode(e.key.keysym.scancode)) {
            case 27:         return sdl_events::quit;     // Escape

            case 1073741904: [[fallthrough]];             // Стрелка влево
            case 1073741916: return sdl_events::previous; // Стрелка влево на числовой клавиатуре

            case 1073741903: [[fallthrough]];             // Стрелка вправо
            case 1073741918: return sdl_events::next;     // Стрелка вправо на числовой клавиатуре

            case 1073741898: [[fallthrough]];             // Home
            case 1073741919: return sdl_events::start;    // Home на числовой клавиатуре

            case 1073741901: [[fallthrough]];             // End
            case 1073741913: return sdl_events::end;      // End на числовой клавиатуре

            default: break;
            }
        }
    }
    return sdl_events::none;
}