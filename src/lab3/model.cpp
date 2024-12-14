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
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <random>
#include <cmath>
#include "model.h"
#include "parser.h"

inline constexpr double MATH_PI = 3.14159265358979323846;

particle emit_point::spawn_particle() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution nums_y(90.0f - spread, spread + 90.0f);
    auto angle = nums_y(gen) * MATH_PI / 180.0f; // Перевод в радианы

    float dir_x = (float)sin(angle);
    float dir_y = (float)cos(angle);

    std::uniform_real_distribution en(0.01f, 0.99f);
    float got_prob = en(gen);

    // Выбор энергии
    size_t prob_id = 0;
    float top_prob = 0.0f;
    while (top_prob < got_prob) {
        top_prob += energy[prob_id].prob;
        prob_id++;
    }
    prob_id--;

    return particle(pos.x, pos.y, dir_x, dir_y, energy[prob_id].level);
}

simulation::simulation(const parser_data& conf) {
    // Загрузка материалов
    auto mats = load_materials(conf);
    if (mats.size() == 0)
        throw std::runtime_error("No materials found.");

    subareas = spawn_areas(conf, &main_area, mats);
    // Получаем границы для подобластей
    b_count = subareas.size();
    borders.reserve(b_count);
    for (size_t i = 0; i < b_count; i++)
        borders.push_back({ subareas[i].x_start, subareas[i].x_start + subareas[i].width });

    emitter = spawn_emitter(find_config_line(conf, "source"), find_config_line(conf, "energy"));
    part_count = get_particles_count(find_config_line(conf, "particles"));
    try {
        burst_count = std::stoul(find_config_line(conf, "burst").args[0]);
    }
    catch (const std::exception&) {
        burst_count = part_count;
    }

    // Выделение памяти под треки
    tracks.reserve(part_count);
    for (size_t i = 0; i < part_count; i++) {
        tracks.emplace_back();
        tracks.back().reserve(5);
    }

    // Подготовка статистики
    stat_subarea_energy.reserve(b_count);
    for (size_t i = 0; i < b_count; i++) stat_subarea_energy.push_back(0.0f);
}

// Метод Карлсона для столкновения
void collide_carlson(particle& p, const float xi) {
    const float energy_now = p.get_energy();

    const float eps = 0.511f;
    const float alpha = energy_now / eps;

    const float S = energy_now / (1.0f + 0.5625f * alpha);
    float energy_new = energy_now / (1.0f + S + xi + (2.0f * alpha - S) * powf(xi, 3.0f));

    // Поправки для разных энергий
    if (energy_now > 0.0f && energy_now < 0.5f) {
        const float A = 0.592f;
        const float B = 2.578f;
        const float C = 0.425f;

        energy_new -= (A * alpha * alpha) / (1.0f + 2.0f * alpha);
        energy_new *= (B / (C + alpha) - 1.0f) * xi * xi * (1.0f - xi) * (1.0f - xi);
    }
    else {
        energy_new += 0.5f * (alpha - 4.0f) * xi * xi * (1.0f - xi) * (1.0f - xi);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution new_angle(0.0, 2.0 * MATH_PI);
    auto angle = new_angle(gen);

    float new_cos = (float)cos(angle);
    float new_sin = (float)sin(angle);

    p.set_direction(new_cos, new_sin);
    p.set_energy(energy_new);
}

SDL_FPoint calc_real_end_pos(const SDL_FPoint start_pos, const SDL_FPoint end_pos, const main_area_t& consts) {
    // Уравнение последнего движения частицы
    float A = end_pos.y - start_pos.y;
    float B = start_pos.x - end_pos.x;
    float C = start_pos.y * (end_pos.x - start_pos.x);
    C -= start_pos.x * (end_pos.y - start_pos.y);
    C *= -1.0f;

    float len[4] = {HUGE_VALF, HUGE_VALF, HUGE_VALF, HUGE_VALF};
    SDL_FPoint rets[4];
    rets[0] = { 0.0f, C / B };
    rets[1] = { (C - B * consts.height) / A, consts.height };
    rets[2] = { C / A, 0.0f };
    rets[3] = { consts.length, (C - A * consts.length) / B };

    // Расчет расстояния
    for (int i = 0; i < 4; i++)
        len[i] = sqrtf(powf(rets[i].x - start_pos.x, 2.0f) + powf(rets[i].y - start_pos.y, 2.0f));

    // Выбираем минимальное расстояние
    float min = HUGE_VALF;
    int ind = 0;
    for (int i = 0; i < 4; i++)
        if (len[i] < min) {
            ind = i;
            min = len[i];
        }

    return rets[ind];
}

bool simulation::process_particle(void) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution nums(0.0f, main_area.length);

    auto p = emitter->spawn_particle();

    SDL_FPoint coord_pre, coord_now;
    coord_now = p.get_position();
    tracks[current_part].push_back(coord_now);

    float start_energy = p.get_energy();
    stat_total_energy += start_energy;

    bool is_absorbed = false;
    float optics = NAN; // "Нулевое" сечение взаимодействия
    do { // Движение частицы
        float move = nums(gen);
        if (!isnanf(optics)) move /= optics;

        // Транспортное ядро
        p.move_particle(move);
        coord_pre = coord_now;
        coord_now = p.get_position();
        tracks[current_part].push_back(coord_now);

        // Ядро столкновений
        for (auto &i : subareas) {
            optics = NAN; // Если частица не попала в вещество
            if (i.is_hit(coord_now)) {
                std::uniform_real_distribution xi(0.0f, 1.0f);
                if (xi(gen) > i.consume_prob) {
                    auto pre_consume = p.get_energy();
                    collide_carlson(p, 0.5f);
                    // Поглощение веществом энергии
                    float absorbed = pre_consume - p.get_energy();
                    i.process_hit(coord_now, absorbed);
                    optics = i.optics;
                }
                else {
                    i.process_hit(coord_now, p.get_energy());
                    is_absorbed = true;
                }
            }
        }
    } while (!is_absorbed && is_within_main(p.get_position()));
    float end_energy = p.get_energy();

    if (!is_absorbed) {
        // Частица скорее всего перелетела границы. В какой точке?
        auto end_pos = calc_real_end_pos(coord_pre, coord_now, main_area);
        tracks[current_part].back() = end_pos;

        // На экран?
        if (fabs(end_pos.x - main_area.length) < 0.001 && end_pos.y > 0.0f && end_pos.y < main_area.length) {
            stat_screen_particles++;
            stat_screen_energy += end_energy;
        }
    }

    part_en.push_back({start_energy, end_energy});

    // Отладочный вывод
    std::cout << "Particle " << current_part << ": ";
    for (auto &c : tracks[current_part])
        std::cout << "{" << c.x << " " << c.y << "} ";
    std::cout << "; Energy: " << start_energy << "->" << end_energy << std::endl;
    std::cout << std::endl;

    current_part++;
    return current_part < part_count;
}

bool simulation::process_burst(void) {
    if (current_part >= part_count) return false; // Не рассчитываем частицы, если они не заданы

    for (size_t i = 0; i < burst_count; i++) {
        if (process_particle() == false) return false;
    }

    process_heat();

    std::cout << "Burst processed." << std::endl;
    return current_part < part_count;
}

void simulation::process_heat(void) {
    constexpr size_t passes = 1; // Число проходов

    for (auto &i : subareas) {
        auto heat = i.get_grid();

        const auto Y = heat.get_height();
        const auto X = heat.get_length();

        for (size_t p = 0; p < passes; p++)
            for (size_t y = 0; y < Y; y++)
                for (size_t x = 0; x < X; x++) {
                    // Сколько соседей?
                    float neighbours = 4.0f;
                    if (y == 0 || y == Y - 1) neighbours -= 1.0f;
                    if (x == 0 || x == X - 1) neighbours -= 1.0f;

                    const float onehalf = heat(y, x) / 2.0f;
                    const float spread = onehalf / neighbours;

                    heat(y, x) = onehalf;
                    // Передача тепла
                    if (y > 0) heat(y - 1, x) += spread;
                    if (y < Y - 1) heat(y + 1, x) += spread;
                    if (x > 0) heat(y, x - 1) += spread;
                    if (x < X - 1) heat(y, x + 1) += spread;
                }
    }
}

std::vector<std::string> simulation::get_subarea_names(void) const {
    std::vector<std::string> ret;

    const auto size = subareas.size();
    ret.reserve(size);

    for (size_t i = 0; i < size; i++) ret.push_back(subareas[i].name);
    return ret;
}

bool simulation::is_within_main(const SDL_FPoint p) const {
    if (p.x < 0.0f || p.y < 0.0f) return false;
    if (p.x > main_area.length || p.y > main_area.height) return false;

    return true;
}

sim_output simulation::get_tracks(void) const {
    sim_output ret(part_count);
    for (size_t p = 0; p < part_count; p++) {
        auto len = tracks[p].size();
        ret.track_len[p] = len;
        ret.tracks[p] = new SDL_FPoint[len];

        auto curr = ret.tracks[p];
        for (size_t t = 0; t < len; t++)
            curr[t] = tracks[p][t];

        ret.energies->x = part_en[p].pre;
        ret.energies->y = part_en[p].post;
    }

    return ret;
}

sim_stats simulation::get_stats(void) const {
    sim_stats ret;
    const size_t size = subareas.size();

    ret.subarea_energy.reserve(size);
    ret.subareas_count = size;
    for (auto &i : subareas) {
        ret.subarea_energy.push_back(i.get_total_absorbed());
        ret.subarea_names.push_back(i.name);
    }

    ret.screen_energy = stat_screen_energy;
    ret.screen_particles = stat_screen_particles;

    ret.total_energy = stat_total_energy;
    ret.total_particles = part_count;

    return ret;
}
