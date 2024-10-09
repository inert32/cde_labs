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

class sdl_text;
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
    static constexpr int area_x_end = len_x - 1;
    static constexpr int area_x_diap = area_x_end - area_x_start; // Диапазон позиций поля вывода вдоль OX

    static constexpr int area_y_start = (int)(0.1 * len_y);
    static constexpr int area_y_end = (int)(0.9 * len_y);

    sdl_text* text;
};

// Вывод времени и отметок на сетке
class sdl_text {
public:
    sdl_text(SDL_Renderer* renderer);
    ~sdl_text();

    void render_text(const std::string& text, const int x, const int y);
    // Для чисел с плавающей запятой отрезать лишнее число (size) знаков после запятой
    std::string cut_number(const double num, const size_t size) const;
private:
    // Шрифт
    TTF_Font* font = nullptr;
    SDL_Renderer* rend = nullptr;
};

// Простая X-Y сетка для графика
class sdl_grid {
public:
    sdl_grid(const size_t marks_x, const size_t marks_y, const SDL_FRect area, sdl_text* text_writer);
    ~sdl_grid();

    void draw_axes(SDL_Renderer* rend) const;
private:
    // Настройки координатной сетки
    SDL_FPoint* coord_grid = nullptr;
    // Число отметок
    size_t grid_ox_count;
    size_t grid_oy_count;
    // Размер coord_grid (оси и все отметки)
    size_t grid_size;
    sdl_text* text;
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

sdl_events handle_kbd() { return sdl_events::quit; }

#endif /* __ENABLE_GRAPH__ */

#endif /* __DISPLAY_H__ */
