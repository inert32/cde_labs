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

#include <vector>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include "model.h"

class sdl_text;
class sdl_grid;

// Отображение графика на экране
class sdl_display {
public:
    sdl_display(const simulation& sim);
    ~sdl_display();

    // Вывести треки на экран
    // Треки должны быть масштабированы (translate_tracks) перед выводом на экран
    void show_frame(const sim_output& tracks);

    // Масштабирование треков
    sim_output translate_tracks(const sim_output& tracks) const;

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
    SDL_FRect* subareas_ = nullptr;
    size_t subareas_count = 0;

    // Координаты источника
    SDL_FPoint emitter_pos = {0.0f, 0.0f};
};

// Обработка нажатий клавиш
sdl_events handle_kbd();

#else
#include <iostream>
#include "mesh.h"

class sdl_display {
public:
    sdl_display() { std::cout << "SDL: display disabled. " << std::endl; }
    ~sdl_display() = default;

    void show_frame() {}
};

sdl_events handle_kbd() { return sdl_events::quit; }

#endif /* __ENABLE_GRAPH__ */

#endif /* __DISPLAY_H__ */
