// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifdef __ENABLE_GRAPH__

#include <stdexcept>
#include <SDL.h>
#include "display.h"

// Очистка экрана
#define clear_screen() SDL_SetRenderDrawColor(rend, 127, 127, 127, 255); SDL_RenderClear(rend)

sdl_display::sdl_display() {
    // Запуск SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) throw std::runtime_error(SDL_GetError());

    // Создание окна
    window = SDL_CreateWindow("Lab3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, len_x, len_y, SDL_WINDOW_SHOWN);
    if (window == nullptr) throw std::runtime_error(SDL_GetError());

    // Запуск структуры отрисовки
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

void sdl_display::show_frame() {
    clear_screen();

    // Рисуем границы графика
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_Point area[6] = { {area_x_start, area_y_start}, {area_x_start, area_y_end}, {area_x_end, area_y_end}, {area_x_end, area_y_start}, {area_x_start, area_y_start}, {area_x_start, area_y_end}, };
    if (SDL_RenderDrawLines(rend, area, 5) < 0) throw std::runtime_error(SDL_GetError());

    // Источник
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_RenderDrawPointF(rend, emitter_pos.x, emitter_pos.y);

    // Передаем на экран
    SDL_RenderPresent(rend);
}

void sdl_display::setup_consts(main_area_t main_area, std::vector<subarea_t> subareas, emit_point* emitter) {
    main_height = main_area.height;
    main_width = main_area.width;

    // Позиция источника на экране
    const auto em_pos = emitter->get_position();
    emitter_pos = calc_point_position(em_pos.x, em_pos.y);
}

SDL_FPoint sdl_display::calc_point_position(const float x, const float y) {
    float ret_x = area_x_diap / main_width * x + area_x_start;
    float ret_y = area_y_diap / main_height * y + area_y_start;

    return {ret_x, ret_y};
}

#endif /* __ENABLE_GRAPH__ */
