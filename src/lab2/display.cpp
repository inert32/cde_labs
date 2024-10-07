// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifdef __ENABLE_GRAPH__

#include <iostream>
#include <stdexcept>
#include <SDL.h>
#include "display.h"

#define clear_screen() SDL_SetRenderDrawColor(rend, 127, 127, 127, 255); SDL_RenderClear(rend)

sdl_display::sdl_display() {
    // Запуск SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) throw std::runtime_error(SDL_GetError());

    // Создание окна
    window = SDL_CreateWindow("Lab2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, len_x, len_y, SDL_WINDOW_SHOWN);
    if (window == nullptr) throw std::runtime_error(SDL_GetError());

    rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (rend == nullptr) throw std::runtime_error(SDL_GetError());
    clear_screen();
    SDL_RenderPresent(rend);
}

sdl_display::~sdl_display() {
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdl_display::show_frame(const mesh_t& mesh, const size_t curr) {
    clear_screen();
    // Создаем точки
    auto size = mesh.get_size().x;
    SDL_FPoint* points = new SDL_FPoint[size];

    auto abs = mesh.get_layer_x();
    auto layer = mesh.get_layer(curr);

    // Масштабирование
    auto Xmin = abs[0];
    auto diapX = abs[size - 1] - Xmin;
    auto Ymax = mesh.get_max_on_layer(curr);
    auto diapY = Ymax - mesh.get_min_on_layer(curr);

    // Для графика выделяем следующие части экрана:
    // 0-1 по OX, 0.1-0.9 по OT
    auto graph_x_start = (int)(0 * len_x);
    auto graph_x_end = (int)(1 * len_x) - 1;
    auto diap_xg = graph_x_end - graph_x_start; // Диапазон позиций поля вывода вдоль OX

    auto graph_t_start = (int)(0.1 * len_y);
    auto graph_t_end = (int)(0.9 * len_y);
    auto diap_yg = graph_t_end - graph_t_start; // Диапазон позиций поля вывода вдоль OY

    SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
    SDL_Point area[6] = { {graph_x_start, graph_t_start}, {graph_x_start, graph_t_end}, {graph_x_end, graph_t_end}, {graph_x_end, graph_t_start}, {graph_x_start, graph_t_start}, {graph_x_start, graph_t_end}, };
    if (SDL_RenderDrawLines(rend, area, 5) < 0) throw std::runtime_error(SDL_GetError());

    for (size_t i = 0; i < size; i++) {
        auto x = ((abs[i] - Xmin) * diap_xg / diapX) + graph_x_start;
        auto y = ((Ymax - layer[i]) * diap_yg / diapY) + graph_t_start;
        points[i] = {(float)x, (float)y};
    }
    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
    if (SDL_RenderDrawLinesF(rend, points, (int)size) < 0) throw std::runtime_error(SDL_GetError());
    SDL_RenderPresent(rend);
    delete[] points;
}

sdl_events handle_kbd() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) return sdl_events::quit;
        if (e.type == SDL_KEYDOWN) {
            switch (SDL_GetKeyFromScancode(e.key.keysym.scancode)) {
                case 27:         return sdl_events::quit;     // Escape
                case 1073741903: return sdl_events::next;     // Стрелка вправо
                case 1073741904: return sdl_events::previous; // Стрелка влево
                default: break;
            }
        }
    }
    return sdl_events::none;
}

#endif /* __ENABLE_GRAPH__ */
