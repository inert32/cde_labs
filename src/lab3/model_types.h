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
#ifndef __MODEL_TYPES_H__
#define __MODEL_TYPES_H__

#include <string>
#include <map>
#ifndef __ENABLE_GRAPH__ // SDL отключен, копируем определение SDL_FPoint
typedef struct SDL_FPoint
{
    float x;
    float y;
} SDL_FPoint;
#else
#include <SDL_rect.h>
#endif /* __ENABLE_GRAPH__ */
#include "flatmat.h"

// Главная область моделирования
struct main_area_t {
    float length = 0.0f;
    float height = 0.0f;
};

// Параметры материала.
// Необходима во время настройки simulation
struct material_t {
    // Сечение взаимодействия
    float sigma = 0.0f;
    // Вероятность поглощения
    float consume_prob = 0.0f;
};
typedef std::map<std::string, material_t> mat_t;

class particle {
public:
    particle(const float pos_x, const float pos_y, const float dir_x, const float dir_y, const float en) {
        pos = {pos_x, pos_y}; direction = {dir_x, dir_y}; energy = en;
    }

    SDL_FPoint get_position() const { return pos; }
    float get_energy() const { return energy; }

    void move_particle(const float len) { pos.x += len * direction.x; pos.y += len * direction.y; }
    void set_direction(const float dir_x, const float dir_y) { direction = {dir_x, dir_y}; }
    void set_energy(const float en) { energy = en; }
private:
    // Координаты частицы
    SDL_FPoint pos;
    // Вектор скорости
    SDL_FPoint direction;

    float energy = 0.0f;
};

inline constexpr size_t sa_grid_x = 20;
inline constexpr size_t sa_grid_y = 50;

// Подобласть внутри главной (вещество)
class subarea_t {
public:
    subarea_t(void) = delete;
    subarea_t(const float start, const float wid, const float height,
              const float opt, const float prob, const std::string& mat_name) :
    x_start{start}, width{wid}, height{height}, optics{opt}, 
    consume_prob{prob}, name{mat_name}, grid(sa_grid_y, sa_grid_x) {}

    bool is_hit(const SDL_FPoint p) const {
        return (p.x > x_start && p.x < x_start + width) && // По оси OX
               (p.y > 0.0f && p.y < height); // По оси OY
    }
    void process_hit(const SDL_FPoint at, const float energy) {
        // Переводим [x_0 ; x_0 + width] -> [0 ; 1]
        float x = (at.x - x_start) / width;
        float y = at.y / height;

        // Координаты в сетке
        size_t x_c = (size_t)floorf(x * (float)sa_grid_x);
        size_t y_c = (size_t)floorf(y * (float)sa_grid_y);

        grid(y_c, x_c) += energy;
        total_absorbed += energy;

        if (grid(y_c, x_c) > max_absorbed) max_absorbed = grid(y_c, x_c);
    }
    float get_max_absorbed(void) const { return max_absorbed; }

    flatmat<float> get_grid(void) const { return grid; }

    const float x_start = 0.0f;
    const float width = 0.0f;
    const float height = 0.0f;
    // Оптическая толщина
    const float optics = 0.0f;
    // Вероятность поглощения
    const float consume_prob = 0.0f;
    const std::string name;

private:
    flatmat<float> grid;
    float total_absorbed = 0.0f;
    float max_absorbed = 0.0f;
};

typedef std::pair<flatmat<float>, float> heatmap_t;

struct energy_distr_t {
    float level = 0.0f;
    float prob = 0.0f;
};

// Точечный источник частиц
class emit_point {
public:
    emit_point(const float pos_x, const float pos_y, const float angle, const std::vector<energy_distr_t>& dist) : energy(dist) {
        pos.x = pos_x; pos.y = pos_y; spread = angle;
    }
    emit_point() = default;

    particle spawn_particle();
    SDL_FPoint get_position() const { return pos; }
private:
    SDL_FPoint pos;
    float spread;

    std::vector<energy_distr_t> energy;
};

#endif /* __MODEL_TYPES_H__ */
