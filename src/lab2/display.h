#ifndef __DISPLAY_H__
#define __DISPLAY_H__

enum class sdl_events {
    none,
    quit,
    previous,
    next
};

#ifdef __ENABLE_GRAPH__

#include <stdexcept>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include "mesh.h"

class sdl_text;

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
    SDL_FPoint* coord_grid = nullptr;
    static constexpr int grid_ox_count = 10;
    static constexpr int grid_ot_count = 10;
    // Размер coord_grid (оси и все отметки)
    static constexpr int grid_size = 2 * (grid_ox_count + 1) + 2 * (grid_ot_count + 1) + 4;
    void setup_grid();
    void draw_grid();

    // Область рисования графика
    static constexpr int area_x_start = 1;
    static constexpr int area_x_end = len_x - 1;
    static constexpr int area_x_diap = area_x_end - area_x_start; // Диапазон позиций поля вывода вдоль OX

    static constexpr int area_t_start = (int)(0.1 * len_y);
    static constexpr int area_t_end = (int)(0.9 * len_y);

    sdl_text* text;
};

// Вывод времени и отметок на сетке
class sdl_text {
public:
    sdl_text(SDL_Renderer* renderer);
    ~sdl_text();

    void render_text(const std::string& text, const int x, const int y);
private:
    // Шрифт
    TTF_Font* font = nullptr;
    SDL_Renderer* rend = nullptr;
};

sdl_events handle_kbd();

#else
#include <iostream>
#include "mesh.h"

class sdl_display {
public:
    sdl_display() { std::cout << "SDL: display disabled. " << std::endl; }
    ~sdl_display() {}

    void show_frame([[maybe_unused]] const mesh_t& mesh, [[maybe_unused]] const size_t curr) {}
};

sdl_events handle_kbd() { return sdl_events::quit; }

#endif /* __ENABLE_GRAPH__ */

#endif /* __DISPLAY_H__ */
