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
#include "../common/sdl_text.h"

class sdl_grid;

// Отображение графика на экране
class sdl_display {
public:
    sdl_display();
    ~sdl_display();

    // Выбор слоя сетки (curr) для отображения
    void show_frame(const mesh_t& mesh, const size_t curr, const double t_step);

private:
    static constexpr int len_x = 1024, len_y = 768;
    SDL_Window* window = nullptr;
    SDL_Renderer* rend = nullptr;

    SDL_FPoint* scale_graph(const mesh_t& mesh, const size_t curr);

    // Настройки координатной сетки
    sdl_grid* coord_grid = nullptr;

    // Область рисования графика
    static constexpr int area_x_start = (int)(0.1 * len_x);
    static constexpr int area_x_end = (int)(0.9 * len_x);
    static constexpr int area_x_diap = area_x_end - area_x_start; // Диапазон позиций поля вывода вдоль OX

    static constexpr int area_y_start = (int)(0.1 * len_y);
    static constexpr int area_y_end = (int)(0.9 * len_y);

    sdl_text* text;
};

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
