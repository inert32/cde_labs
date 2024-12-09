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
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

enum class sdl_events {
    none,
    quit,
    previous,
    next,
    start,
    end,
    mode
};

// Обработка нажатий клавиш
sdl_events handle_kbd();

#ifdef __ENABLE_GRAPH__

#include "../common/sdl_display.h"
#include "model.h"

// Цвет подобласти
struct color_t {
    Uint8 r = 0;
    Uint8 g = 0;
    Uint8 b = 0;
};

// Подготовленная для вывода тепловая карта
struct heatmap_converted {
    SDL_FRect pos = {0.0f, 0.0f, 0.0f, 0.0f};
    Uint8 color = 0;
};

// Отображение графика на экране
class sdl_display : public sdl_display_base {
public:
    sdl_display(const simulation& sim);
    ~sdl_display() = default;

    // Вывод треков на экран
    // Треки должны быть масштабированы (translate_tracks) перед выводом на экран
    void show_frame(const sim_output& tracks);
    // Вывод тепловой карты на экран
    void show_heatmap(const std::vector<heatmap_converted>& hm);

    // Масштабирование треков
    sim_output translate_tracks(sim_output& tracks) const;
    // Расчет цвета для всех тепловых карт
    std::vector<heatmap_converted> translate_heatmap(const std::vector<heatmap_t>& hm) const;

    // Расчет позиции точки внутри графика
    inline SDL_FPoint calc_point_position(const float x, const float y) const;
    inline SDL_FPoint calc_point_position(const SDL_FPoint p) const;
private:
    // Размеры главной области (main_area_t)
    float main_width = 0.0f;
    float main_height = 0.0f;

    // Настройка subareas_ и emitter_pos
    void setup_consts(const simulation& sim);

    // Массив подобластей
    std::vector<SDL_FRect> subareas_;
    size_t subareas_count = 0;
    std::vector<std::string> names;
    std::map<std::string, color_t> subarea_colors;

    // Координаты источника
    SDL_FPoint emitter_pos = {0.0f, 0.0f};
};

#else
#include <exception>
#include "../common/sdl_display.h"
#include "model.h"

// Подготовленная для вывода тепловая карта
struct heatmap_converted {
    struct SDL_FRect {float x; float y; float w; float h; }
        pos = {0.0f, 0.0f, 0.0f, 0.0f};
    unsigned char color = 0;
};

class sdl_display {
public:
    sdl_display([[maybe_unused]] const simulation& sim) { throw std::runtime_error("SDL disabled."); }
    ~sdl_display() = default;

    // Вывод треков на экран
    // Треки должны быть масштабированы (translate_tracks) перед выводом на экран
    void show_frame([[maybe_unused]] const sim_output& tracks) {}
    // Вывод тепловой карты на экран
    void show_heatmap([[maybe_unused]] const std::vector<heatmap_converted>& hm) {}

    // Масштабирование треков
    sim_output translate_tracks([[maybe_unused]] sim_output& tracks) const {
        sim_output dummy(0);
        return dummy;
    }
    // Расчет цвета для всех тепловых карт
    std::vector<heatmap_converted> translate_heatmap([[maybe_unused]] const std::vector<heatmap_t>& hm) const {
        std::vector<heatmap_converted> dummy;
        return dummy;
    }

    // Расчет позиции точки внутри графика
    inline SDL_FPoint calc_point_position([[maybe_unused]] const float x, [[maybe_unused]] const float y) const {
        SDL_FPoint dummy;
        return dummy;
    }
    inline SDL_FPoint calc_point_position([[maybe_unused]] const SDL_FPoint p) const  {
        SDL_FPoint dummy;
        return dummy;
    }
};

#endif /* __ENABLE_GRAPH__ */

#endif /* __DISPLAY_H__ */
