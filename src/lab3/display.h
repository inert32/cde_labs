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
    end
};

// Обработка нажатий клавиш
sdl_events handle_kbd();

#ifdef __ENABLE_GRAPH__

#include <map>
#include <SDL.h>
#include "model.h"
#include "../common/sdl_text.h"

struct color_t {
    Uint8 r = 0;
    Uint8 g = 0;
    Uint8 b = 0;
};

// Отображение графика на экране
class sdl_display {
public:
    sdl_display(const simulation& sim);
    ~sdl_display();

    // Вывести треки на экран
    // Треки должны быть масштабированы (translate_tracks) перед выводом на экран
    void show_frame(const sim_output& tracks);

    // Масштабирование треков
    sim_output translate_tracks(sim_output& tracks) const;

    // Расчет позиции точки внутри графика
    SDL_FPoint calc_point_position(const float x, const float y) const;
    SDL_FPoint calc_point_position(const SDL_FPoint p) const;
private:
    static constexpr int len_x = 1024, len_y = 768; // Размер окна
    SDL_Window* window = nullptr;
    SDL_Renderer* rend = nullptr;

    // Область рисования графика
    static constexpr int area_x_start = (int)(0.1 * len_x);
    static constexpr int area_x_end = (int)(0.9 * len_x);
    static constexpr int area_x_diap = area_x_end - area_x_start;

    static constexpr int area_y_start = (int)(0.1 * len_y);
    static constexpr int area_y_end = (int)(0.9 * len_y);
    static constexpr int area_y_diap = area_y_end - area_y_start;

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

    sdl_text* text;
};

#else
#include <exception>
#include "model.h"

class sdl_display {
public:
    sdl_display([[maybe_unused]] const simulation& sim)  { throw std::runtime_error("SDL disabled."); }
    ~sdl_display() = default;

    // Вывести треки на экран
    // Треки должны быть масштабированы (translate_tracks) перед выводом на экран
    void show_frame([[maybe_unused]] const sim_output& tracks) {}

    // Масштабирование треков
    sim_output translate_tracks([[maybe_unused]] const sim_output& tracks) const {
        sim_output dummy(0);
        return dummy;
    }

    // Расчет позиции точки внутри графика
    SDL_FPoint calc_point_position([[maybe_unused]] const float x, [[maybe_unused]] const float y) const {
        SDL_FPoint dummy;
        return dummy;
    }
    SDL_FPoint calc_point_position([[maybe_unused]] const SDL_FPoint p) const  {
        SDL_FPoint dummy;
        return dummy;
    }
};

#endif /* __ENABLE_GRAPH__ */

#endif /* __DISPLAY_H__ */
