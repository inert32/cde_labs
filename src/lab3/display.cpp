// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifdef __ENABLE_GRAPH__

#include <stdexcept>
#include <SDL.h>
#include "display.h"

static constexpr Uint8 subareas_colors[3][3] = {
    {0, 255, 255}, {255, 0, 255}, {255, 255, 0}
};

// Очистка экрана
#define clear_screen() SDL_SetRenderDrawColor(rend, 127, 127, 127, 255); SDL_RenderClear(rend)

sdl_display::sdl_display(const simulation& sim) {
    // Запуск SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) throw std::runtime_error(SDL_GetError());

    // Создание окна
    window = SDL_CreateWindow("Lab3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, len_x, len_y, SDL_WINDOW_SHOWN);
    if (window == nullptr) throw std::runtime_error(SDL_GetError());

    // Запуск структуры отрисовки
    rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (rend == nullptr) throw std::runtime_error(SDL_GetError());
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

    setup_consts(sim);

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

    // Подобласти
    for (size_t i = 0; i < subareas_count; i++) {
        // Отрисовка разных подобластей разными цветами
        int color_now = i % 3;
        SDL_SetRenderDrawColor(rend, subareas_colors[color_now][0], 
        subareas_colors[color_now][1], subareas_colors[color_now][2], 100);

        SDL_RenderFillRectF(rend, &subareas_[i]);
    }

    // Передаем на экран
    SDL_RenderPresent(rend);
}

//void sdl_display::setup_consts(const main_area_t& main_area, const std::vector<subarea_t>& subareas, const emit_point* emitter) {
void sdl_display::setup_consts(const simulation& sim) {
    auto ma = sim.get_main_area();
    main_height = ma.height;
    main_width = ma.width;

    // Позиция источника на экране
    const auto em_pos = sim.get_emitter()->get_position();
    emitter_pos = calc_point_position(em_pos.x, em_pos.y);

    // Подобласти
    auto sa = sim.get_subarea();
    subareas_count = sa.size();
    subareas_ = new SDL_FRect[subareas_count];
    for (size_t i = 0; i < subareas_count; i++) {
        // Переводим координаты
        SDL_FPoint start = calc_point_position(sa[i].x_start, 0.0f);
        SDL_FPoint end = calc_point_position(sa[i].width, main_height);

        // Сохраняем координаты
        SDL_FRect tmp; 
        tmp.x = start.x; tmp.y = start.y;
        tmp.w = end.x - area_x_start; tmp.h = area_y_end - area_y_start;
        subareas_[i] = tmp;
    }
}

SDL_FPoint sdl_display::calc_point_position(const float x, const float y) {
    float ret_x = area_x_diap / main_width * x + area_x_start;
    float ret_y = area_y_diap / main_height * y + area_y_start;

    return {ret_x, ret_y};
}

#endif /* __ENABLE_GRAPH__ */
