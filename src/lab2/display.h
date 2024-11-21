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

#ifdef __ENABLE_GRAPH__

#include <stdexcept>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include "mesh.h"
#include "../common/sdl_display.h"

// Простая X-Y сетка для графика
class sdl_grid {
public:
    sdl_grid(const size_t marks_x, const size_t marks_y, const SDL_FRect area, sdl_text* text);
    ~sdl_grid();

    void draw_axes(SDL_Renderer* rend, const mesh_t& mesh, const size_t curr) const;
private:
    // Настройки координатной сетки
    SDL_FPoint* coord_grid = nullptr;
    // Число отметок
    size_t grid_ox_count;
    size_t grid_oy_count;
    // Размер coord_grid (оси и все отметки)
    size_t grid_size;
    SDL_FRect area_;
    sdl_text* text_;
};

// Отображение графика на экране
class sdl_display : public sdl_display_base {
public:
    sdl_display();
    ~sdl_display();

    // Выбор слоя сетки (curr) для отображения
    void show_frame(const mesh_t& mesh, const size_t curr, const double t_step);

private:
    SDL_FPoint* scale_graph(const mesh_t& mesh, const size_t curr);

    // Настройки координатной сетки
    sdl_grid* coord_grid = nullptr;
};

sdl_events handle_kbd();

#else
#include <iostream>
#include "mesh.h"

class sdl_display {
public:
    sdl_display() { std::cout << "SDL: display disabled. " << std::endl; }
    ~sdl_display() {}

    void show_frame([[maybe_unused]] const mesh_t& mesh, [[maybe_unused]] const size_t curr, [[maybe_unused]] const double t_step) {}
};

sdl_events handle_kbd();

#endif /* __ENABLE_GRAPH__ */

#endif /* __DISPLAY_H__ */
