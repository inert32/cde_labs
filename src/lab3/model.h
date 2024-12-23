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

// Подобласть внутри главной (вещество)
struct subarea_t {
    float x_start = 0.0f;
    float width = 0.0f;
    // Оптическая толщина
    float optics = 0.0f;
    // Вероятность поглощения
    float consume_prob = 0.0f;
    std::string name;
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
    sim_output& operator=(const sim_output& other) {
        if (this == &other) return *this;
        particle_count = other.particle_count;
        track_len = new size_t[particle_count]; //-V773
        memcpy(track_len, other.track_len, sizeof(size_t) * particle_count);

        tracks = new SDL_FPoint*[particle_count];
        for (size_t i = 0; i < particle_count; i++) {
            auto len = track_len[i];
            tracks[i] = new SDL_FPoint[len];
            memcpy(tracks[i], other.tracks[i], sizeof(SDL_FPoint) * len);
        }

        energies = new SDL_FPoint[particle_count];
        memcpy(energies, other.energies, sizeof(SDL_FPoint) * particle_count);
        return *this;
    }
    ~sim_output() {
        for (size_t i = 0; i < particle_count; i++) delete[] tracks[i];
        delete[] tracks;
        delete[] track_len;
        delete[] energies;
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
