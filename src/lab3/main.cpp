// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#define SDL_MAIN_HANDLED

#include <iostream>
#include <iomanip>
#include <cstring>
#include "../build.h"
#include "../common.h"
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
        while (sim.process_particle());
        std::cout << "Simulation complete." << std::endl;

        if (run_stats) print_stats(sim.get_stats());

        if (run_sdl) {
            try {
                sdl_display disp(sim);
                auto tracks = sim.get_tracks();
                tracks = disp.translate_tracks(tracks);
                bool run = true;

                while (run) {
                    disp.show_frame(tracks);
                    switch (handle_kbd()) {
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
