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

#include "parser_types.h"
#include "model_types.h"

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
    ~sim_output(void) {
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
    ~simulation(void) { delete emitter; }

    // Расчет пути частицы
    // Возвращает false, если все частицы рассчитаны
    bool process_particle(void);

    // Расчет пакета частиц
    // Возвращает false, если все частицы рассчитаны
    bool process_burst();

    void process_heat(void);

    const main_area_t get_main_area(void) const { return main_area; }
    const std::vector<subarea_t>& get_subarea(void) const { return subareas; }
    const emit_point* get_emitter(void) const { return emitter; }
    std::vector<std::string> get_subarea_names(void) const;

    // Вывод данных о симуляции
    sim_output get_tracks(void) const;

    // Вывод статистики
    sim_stats get_stats(void) const;

    std::vector<heatmap_t> get_grids(void) const {
        std::vector<heatmap_t> ret;
        for (auto &i : subareas)
            ret.emplace_back(i.get_grid(), i.get_max_absorbed());
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

    size_t burst_count = 0; // Число частиц для взаимодействия перед расчетом энергии

    // Границы подобластей для простоты расчета столкновений
    struct subarea_borders_t {float start = 0.0f; float end = 0.0f;};
    std::vector<subarea_borders_t> borders;
    size_t b_count = 0;

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
