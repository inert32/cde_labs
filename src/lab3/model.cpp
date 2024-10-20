// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <random>
#include <cmath>
#include "model.h"
#include "parser.h"

particle::particle(const float pos_x, const float pos_y, const float dir_x, const float dir_y) {
    pos = {pos_x, pos_y};
    direction = {dir_x, dir_y};
}

SDL_FPoint particle::get_position() const {
    return pos;
}

void particle::move_particle(const float len) {
    pos.x += len * direction.x;
    pos.y += len * direction.y;
}

void particle::set_direction(const float dir_x, const float dir_y) {
    direction = {dir_x, dir_y};
}

emit_point::emit_point(const float pos_x, const float pos_y, const float angle, const std::vector<energy_distr_t>& dist) : energy(dist) {
    pos.x = pos_x; pos.y = pos_y; spread = angle;
}

particle emit_point::spawn_particle() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution nums_y(90.0f - spread, spread + 90.0f);
    float angle = nums_y(gen) * M_PI / 180.0f; // Перевод в радианы

    float dir_x = sin(angle);
    float dir_y = cos(angle);

    particle ret(pos.x, pos.y, dir_x, dir_y);

    return ret;
}

SDL_FPoint emit_point::get_position() const {
    return pos;
}

simulation::simulation(const parser_data& conf) {
    subareas = spawn_areas(conf, &main_area);
    // Получаем границы для подобластей
    b_count = subareas.size();
    borders = new subarea_borders_t[b_count];
    for (size_t i = 0; i < b_count; i++)
        borders[i] = { subareas[i].x_start, subareas[i].x_start + subareas[i].width };

    emitter = spawn_emitter(find_config_line(conf, "source"), find_config_line(conf, "energy"));
    part_count = get_particles_count(find_config_line(conf, "particles"));

    // Выделение памяти под треки
    tracks.reserve(part_count);
    for (size_t i = 0; i < part_count; i++) {
        tracks.emplace_back();
        tracks.back().reserve(5);
    }
}

bool simulation::process_particle() {
    if (current_part >= part_count) return false; // Не рассчитываем частицы, если они не заданы
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution nums(0.0f, main_area.length);

    // Создать частицу
    // Продвинуть ее на длину пробега
    // Если частица попала в другую среду
    //      Создать новый вектор перемещения
    //      Продвинуть ее на длину пробега
    // И так, пока она не выйдет за границы main_area
    auto p = emitter->spawn_particle();
    size_t sa_prev = 0; // Предыдущая подобласть
                        // Считаем, что sa_* = 0 - вакуум
    tracks[current_part].push_back(p.get_position());

    do { // Движение частицы
        auto sa_now = get_subarea_index(p.get_position());

        float move = nums(gen);
        float opt = subareas[sa_now - 1].optics; // get_subarea_index смещена на +1, компенсируем
        if (sa_now != 0) move /= opt;

        p.move_particle(move);
        tracks[current_part].push_back(p.get_position());

        sa_prev = sa_now;
    } while (is_within_main(p.get_position()));

    // Отладочный вывод
    std::cout << "Particle " << current_part << ": ";
    for (auto &c : tracks[current_part])
        std::cout << "{" << c.x << " " << c.y << "} ";
    std::cout << std::endl;

    current_part++;
    return (current_part < part_count) ? true : false;
}

const main_area_t simulation::get_main_area() const {
    return main_area;
}

const std::vector<subarea_t>& simulation::get_subarea() const {
    return subareas;
}

const emit_point* simulation::get_emitter() const {
    return emitter;
}

size_t simulation::get_subarea_index(const SDL_FPoint p) const {
    for (size_t id = 0; id < b_count; id++) {
        auto bord = borders[id];
        if (p.x > bord.start && p.x < bord.end) return id + 1; // Одна из подобластей
    }
    return 0; // Находимся в главной области
}

bool simulation::is_within_main(const SDL_FPoint p) const {
    if (p.x < 0.0f || p.y < 0.0f) return false;
    if (p.x > main_area.length || p.y > main_area.height) return false;

    return true;
}

sim_output simulation::get_tracks() const {
    sim_output ret;

    ret.particle_count = part_count;
    ret.tracks = new SDL_FPoint*[part_count];
    ret.track_len = new size_t[part_count];

    for (size_t p = 0; p < part_count; p++) {
        auto len = tracks[p].size();
        ret.track_len[p] = len;
        ret.tracks[p] = new SDL_FPoint[len];

        auto curr = ret.tracks[p];
        for (size_t t = 0; t < len; t++)
            curr[t] = tracks[p][t];
    }

    return ret;
}
