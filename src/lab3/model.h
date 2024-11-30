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
#ifndef __MODEL_H__
#define __MODEL_H__

#include <map>
#include <vector>
#include <string>
#include <cstring>

#ifndef __ENABLE_GRAPH__ // SDL отключен, копируем определение SDL_FPoint
typedef struct SDL_FPoint
{
    float x;
    float y;
} SDL_FPoint;
#else
#include <SDL_rect.h>
#endif /* __ENABLE_GRAPH__ */

#include "parser_types.h"

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

// Подобласть внутри главной (вещество)
class subarea_t {
public:
    subarea_t(void) = delete;
    subarea_t(const float start, const float wid, const float height,
              const float opt, const float prob, const std::string& mat_name) :
    x_start{start}, width{wid}, height{height}, optics{opt}, consume_prob{prob}, name{mat_name} {
        grid = new float*[100];
        for (size_t y = 0; y < 100; y++) {
            grid[y] = new float[100];
            for (size_t x = 0; x < 100; x++)
                grid[y][x] = 0.0f;
        }
    }

    bool is_hit(const particle& p) const {
        auto c = p.get_position();
        return c.x > x_start && c.x < width;
    }
    void process_hit(const SDL_FPoint at, const float energy) {
        if (!(at.x > x_start && at.x < x_start + width)) return; // Эта частица не попала в вещество

        // Переводим [x_0 ; x_0 + width] -> [0 ; 1]
        float x = (at.x - x_start) / width;
        float y = at.y / height;

        // Координаты в сетке
        size_t x_c = (size_t)floorf(x * 100.0f);
        size_t y_c = (size_t)floorf(y * 100.0f);

        grid[y_c][x_c] += energy;
    }

    float** get_grid(void) const { return grid; }

    const float x_start = 0.0f;
    const float width = 0.0f;
    const float height = 0.0f;
    // Оптическая толщина
    const float optics = 0.0f;
    // Вероятность поглощения
    const float consume_prob = 0.0f;
    const std::string name;

private:
    float** grid = nullptr;
};

struct energy_distr_t {
    float level = 0.0f;
    float prob = 0.0;
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

// Выход симуляции - треки частиц
struct sim_output {
    // Массивы треков (посещенные точки в области)
    // Размерность: tracks[particle_count][track_len[i]]
    SDL_FPoint** tracks = nullptr;
    // Число столкновений у частицы
    // Размерность: track_len[particle_count]
    size_t* track_len = nullptr;
    // Энергия частицы до (x) и после (y)
    // Размерность: energies[particle_count]
    SDL_FPoint* energies = nullptr;

    size_t particle_count = 0;
    sim_output(const size_t count) {
        particle_count = count;
        tracks = new SDL_FPoint*[count];
        track_len = new size_t[count];
        energies = new SDL_FPoint[count];
    }
    sim_output(const sim_output& other) {
        _copy_other(other);
    }
    sim_output& operator=(const sim_output& other) {
        if (this == &other) return *this;

        // Удаляем старые данные, если есть
        if (track_len != nullptr || tracks != nullptr || energies != nullptr) {
            if (tracks != nullptr)
                for (size_t i = 0; i < particle_count; i++)
                    delete[] tracks[i];
            delete[] tracks;
            delete[] track_len;
            delete[] energies;
        }
        _copy_other(other);
        return *this;
    }
    ~sim_output() {
        for (size_t i = 0; i < particle_count; i++)
            delete[] tracks[i];
        delete[] tracks;
        delete[] track_len;
        delete[] energies;
    }
    private:
    inline void _copy_other(const sim_output& other) {
        particle_count = other.particle_count;
        track_len = new size_t[particle_count];
        memcpy(track_len, other.track_len, sizeof(size_t) * particle_count);

        tracks = new SDL_FPoint*[particle_count];
        for (size_t i = 0; i < particle_count; i++) {
            auto len = track_len[i];
            tracks[i] = new SDL_FPoint[len];
            memcpy(tracks[i], other.tracks[i], sizeof(SDL_FPoint) * len);
        }

        energies = new SDL_FPoint[particle_count];
        memcpy(energies, other.energies, sizeof(SDL_FPoint) * particle_count);
    }
};

// Статистика симуляции
struct sim_stats {
    // Всего выпущено частиц источником
    size_t total_particles = 0;
    // Число частиц, попавших на экран
    size_t screen_particles = 0;
    // Всего потрачено энергии источником
    float total_energy = 0.0f;
    // Энергия, поглощеная экраном
    float screen_energy = 0.0f;
    // Энергия, поглощеная веществами
    std::vector<float> subarea_energy;

    std::vector<std::string> subarea_names;

    size_t subareas_count = 0;
};

// Обработка частиц
class simulation {
public:
    simulation(const parser_data& conf);
    ~simulation() { delete emitter; }

    // Расчет пути частицы
    // Возвращает false, если все частицы рассчитаны
    bool process_particle();

    const main_area_t get_main_area() const { return main_area; }
    const std::vector<subarea_t>& get_subarea() const { return subareas; }
    const emit_point* get_emitter() const { return emitter; }
    std::vector<std::string> get_subarea_names() const;

    // Вывод данных о симуляции
    sim_output get_tracks() const;

    // Вывод статистики
    sim_stats get_stats() const;

    std::vector<float**> get_grids(void) const {
        std::vector<float**> ret;
        for (auto &i : subareas) ret.push_back(i.get_grid());
        return ret;
    }
private:
    std::vector<std::vector<SDL_FPoint>> tracks; // Треки частиц

    struct energy_track_t { float pre = 0.0f; float post = 0.0f; };
    std::vector<energy_track_t> part_en;

    main_area_t main_area;
    std::vector<subarea_t> subareas;
    emit_point* emitter;
    size_t part_count = 0; // Общее число частиц
    size_t current_part = 0; // Номер текущей частицы

    // Границы подобластей для простоты расчета столкновений
    struct subarea_borders_t {float start = 0.0f; float end = 0.0f;};
    std::vector<subarea_borders_t> borders;
    size_t b_count = 0;

    // Изменяем направление частицы если мы перешли из одной подобласти в другую
    // (get_subarea_index до перемещения != get_subarea_index после)
    size_t get_subarea_index(const SDL_FPoint p) const;

    // Проверка на вылет частицы за пределы main_area
    bool is_within_main(const SDL_FPoint p) const;

    // Статистика

    // Всего потрачено энергии источником
    float stat_total_energy = 0.0f;
    // Число частиц, попавших на экран
    size_t stat_screen_particles = 0;
    // Энергия, поглощеная экраном
    float stat_screen_energy = 0.0f;
    // Энергия, поглощеная веществами
    std::vector<float> stat_subarea_energy;
};

#endif /* __MODEL_H__ */
