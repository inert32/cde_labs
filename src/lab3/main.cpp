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

#define SDL_MAIN_HANDLED

#include <iostream>
#include <iomanip>
#include <cstring>
#include "../build.h"
#include "../common/common.h"
#include "display.h"
#include "parser.h"
#include "model.h"

void print_stats(const sim_stats& stats) {
    const auto prec = (int)std::cout.precision();
    std::cout << "Statistics: " << std::endl;

    auto sp = (float)stats.screen_particles / (float)stats.total_particles * 100.0f;
    std::cout << "Particles got on screen: " << stats.screen_particles << "/" << stats.total_particles <<
    std::setprecision(4) << " (" << sp << "%)" << std::setprecision(prec) << std::endl;

    auto ep = stats.screen_energy / stats.total_energy * 100.0f;
    std::cout << "Screen absorbed " << stats.screen_energy << "/" << stats.total_energy <<
    std::setprecision(4) << " MeV (" << ep << "%)" << std::setprecision(prec) << std::endl;
            
    for (size_t i = 0; i < stats.subareas_count; i++) {
        auto sa = stats.subarea_energy[i] / stats.total_energy * 100.0f;
        std::cout << "Subarea " << i + 1 << " (" << stats.subarea_names[i] <<  ") absorbed: "
        << stats.subarea_energy[i] << std::setprecision(4) << " MeV (" << sa << "%)" << std::setprecision(prec) << std::endl;
    }
}

int main(int argc, char** argv) {
    std::cout << "Lab3 " << build_version << " " << build_git << std::endl;
    auto cli = parse_cli(argc, argv);
    const bool run_sdl = bool_from_cli_map(cli, "graph", true);
    const bool run_stats = bool_from_cli_map(cli, "stat", true);

    // Получаем параметры из файла задания
    parser_data conf;
    try {
        const std::string task_file = str_from_cli_map(cli, "file");
        conf = parse_task_file(task_file);
        std::cout << "Task file parsed." << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    
    try {
        simulation sim(conf);
        std::cout << "Start simulation..." << std::endl;
        while (sim.process_burst());
        std::cout << "Simulation complete." << std::endl;

        if (run_stats) print_stats(sim.get_stats());

        if (run_sdl) {
            try {
                // True - вывод треков, false - вывод тепловой карты
                bool disp_mode = true;
                sdl_display disp(sim);
                auto tracks = sim.get_tracks();
                tracks = disp.translate_tracks(tracks);
                auto heatmap = disp.translate_heatmap(sim.get_grids());
                bool run = true;

                while (run) {
                    (disp_mode) ? disp.show_frame(tracks) : disp.show_heatmap(heatmap);
                    switch (handle_kbd()) {
                    case sdl_events::mode:
                        disp_mode = !disp_mode;
                        break;
                    case sdl_events::quit:
                        run = false;
                        break;
                    default: continue;
                    }
                }
            }
            catch (const std::exception& e) {
                if (!strcmp(e.what(), "SDL disabled"))
                    std::cerr << "SDL: Error: " << e.what() << std::endl;
                else
                    std::cerr << "SDL disabled" << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << "parse: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

#ifdef __ENABLE_GRAPH__

sdl_events handle_kbd() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) return sdl_events::quit;
        if (e.type == SDL_KEYDOWN) {
            switch (SDL_GetKeyFromScancode(e.key.keysym.scancode)) {
            case 27:         return sdl_events::quit;     // Escape
            case 13:         return sdl_events::mode;     // Смена режима вывода

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
#else

sdl_events handle_kbd() {
    return sdl_events::none;
}
#endif /* __ENABLE_GRAPH__ */
