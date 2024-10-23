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

void sdl_display::show_frame(const sim_output& tracks) {
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

    // Треки
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    for (size_t p = 0; p < tracks.particle_count; p++) {
        SDL_RenderDrawLinesF(rend, tracks.tracks[p], (int)tracks.track_len[p]);
    }

    // Передаем на экран
    SDL_RenderPresent(rend);
}

sim_output sdl_display::translate_tracks(const sim_output& tracks) const {
    sim_output ret;

    size_t part_count = ret.particle_count = tracks.particle_count;
    ret.tracks = new SDL_FPoint*[part_count];
    ret.track_len = new size_t[part_count];

    for (size_t p = 0; p < part_count; p++) {
        auto len = tracks.track_len[p];
        ret.track_len[p] = len;
        ret.tracks[p] = new SDL_FPoint[len];

        auto from = tracks.tracks[p];
        auto to = ret.tracks[p];

        for (size_t t = 0; t < len; t++)
            to[t] = calc_point_position(from[t]);
    }

    return ret;
}

void sdl_display::setup_consts(const simulation& sim) {
    auto ma = sim.get_main_area();
    main_height = ma.height;
    main_width = ma.length;

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

SDL_FPoint sdl_display::calc_point_position(const float x, const float y) const {
    float ret_x = area_x_diap / main_width * x + area_x_start;
    float ret_y = area_y_diap / main_height * y + area_y_start;

    return {ret_x, ret_y};
}

SDL_FPoint sdl_display::calc_point_position(const SDL_FPoint p) const {
    float ret_x = area_x_diap / main_width * p.x + area_x_start;
    float ret_y = area_y_diap / main_height * p.y + area_y_start;

    return {ret_x, ret_y};
}

#endif /* __ENABLE_GRAPH__ */
