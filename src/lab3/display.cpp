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

#ifdef __ENABLE_GRAPH__

#include <iostream>
#include <random>
#include "display.h"

// Очистка экрана
#define clear_screen() SDL_SetRenderDrawColor(rend, 127, 127, 127, 255); SDL_RenderClear(rend)

sdl_display::sdl_display(const simulation& sim) : sdl_display_base("Lab3") {
    setup_consts(sim);
}

void sdl_display::show_frame(const sim_output& tracks) {
    clear_screen();

    // Рисуем границы графика
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_FPoint area[6] = { {area_x_start, area_y_start}, {area_x_start, area_y_end}, {area_x_end, area_y_end}, {area_x_end, area_y_start}, {area_x_start, area_y_start}, {area_x_start, area_y_end}, };
    if (SDL_RenderDrawLinesF(rend, area, 5) < 0) throw std::runtime_error(SDL_GetError());

    // Источник
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_RenderDrawPointF(rend, emitter_pos.x, emitter_pos.y);

    // Подобласти
    for (size_t i = 0; i < subareas_count; i++) {
        // Отрисовка разных подобластей разными цветами
        auto color = subarea_colors[names[i]];
        SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, 100);
        SDL_RenderFillRectF(rend, &subareas_[i]);

        // Вывод названия материала
        text->render_text(names[i], (int)subareas_[i].x, (int)area_y_end + 10, (int)subareas_[i].w);
    }

    // Треки
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    for (size_t p = 0; p < tracks.particle_count; p++) {
        SDL_RenderDrawLinesF(rend, tracks.tracks[p], (int)tracks.track_len[p]);
    }

    // Передаем на экран
    SDL_RenderPresent(rend);
}

void sdl_display::show_heatmap(const std::vector<heatmap_converted>& hm) {
    clear_screen();

    // Рисуем границы графика
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_FPoint area[6] = { {area_x_start, area_y_start}, {area_x_start, area_y_end}, {area_x_end, area_y_end}, {area_x_end, area_y_start}, {area_x_start, area_y_start}, {area_x_start, area_y_end}, };
    if (SDL_RenderDrawLinesF(rend, area, 5) < 0) throw std::runtime_error(SDL_GetError());

    // Подобласти
    for (auto &i : hm) {
        SDL_SetRenderDrawColor(rend, i.color, 0, 0, 255);
        SDL_RenderFillRectF(rend, &i.pos);
    }

    // Передаем на экран
    SDL_RenderPresent(rend);
}

sim_output sdl_display::translate_tracks(sim_output& tracks) const {
    for (size_t p = 0; p < tracks.particle_count; p++) {
        auto len = tracks.track_len[p];
        auto curr = tracks.tracks[p];
        for (size_t t = 0; t < len; t++)
            curr[t] = calc_point_position(curr[t]);
    }

    return tracks;
}

std::vector<heatmap_converted> sdl_display::translate_heatmap(const std::vector<heatmap_t>& hm) const {
    std::vector<heatmap_converted> ret;
    ret.reserve(hm.size() * sa_grid_x * sa_grid_y);

    // Поиск максимальной температуры во всех подобластях
    float heat_max = 0.0f;
    for (auto &i : hm) heat_max = std::max(heat_max, i.second);

    for (size_t i = 0; i < subareas_count; i++) {
        auto heat_grid = hm[i].first;
        // Отрисовка сетки внутри подобласти
        auto sa = &subareas_[i];
        const float step_x = sa->w / (float)sa_grid_x;
        const float step_y = sa->h / (float)sa_grid_y;

        for (size_t y = 0; y < sa_grid_y; y++)
            for (size_t x = 0; x < sa_grid_x; x++) {
                // Выбор цвета
                Uint8 color = Uint8(255.0f * heat_grid(y, x) / heat_max);

                // Расчет координат текущего участка сетки
                SDL_FRect target;
                target.x = sa->x + (float)(step_x * (float)x);
                target.y = sa->y + (float)(step_y * (float)y);
                target.w = step_x;
                target.h = step_y;

                ret.push_back({target, color});
            }
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
    subareas_.reserve(subareas_count);
    for (size_t i = 0; i < subareas_count; i++) {
        // Переводим координаты
        SDL_FPoint start = calc_point_position(sa[i].x_start, 0.0f);
        SDL_FPoint end = calc_point_position(sa[i].width, main_height);

        // Сохраняем координаты
        SDL_FRect tmp; 
        tmp.x = start.x; tmp.y = start.y;
        tmp.w = end.x - (float)area_x_start;
        tmp.h = (float)area_y_end - (float)area_y_start;
        subareas_.push_back(tmp);
    }
    names = sim.get_subarea_names();

    // Выбор цвета для разных материалов
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution color(0, 255);
    for (auto &i : names) {
        color_t c = {(Uint8)color(gen), (Uint8)color(gen), (Uint8)color(gen)};
        subarea_colors.insert_or_assign(i, c);
    }
}

inline SDL_FPoint sdl_display::calc_point_position(const float x, const float y) const {
    float ret_x = (float)area_x_diap / main_width * x + (float)area_x_start;
    float ret_y = (float)area_y_diap / main_height * y + (float)area_y_start;

    return {ret_x, ret_y};
}

inline SDL_FPoint sdl_display::calc_point_position(const SDL_FPoint p) const {
    return calc_point_position(p.x, p.y);
}

#endif /* __ENABLE_GRAPH__ */
