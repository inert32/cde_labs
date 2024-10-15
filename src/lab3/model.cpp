// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <random>
#include "model.h"
#include "parser.h"

particle::particle(const float pos_x, const float pos_y, const float vel_x, const float vel_y) {
    pos = {pos_x, pos_y};
    velocity = {vel_x, vel_y};
}

SDL_FPoint particle::get_position() const {
    return pos;
}

SDL_FPoint particle::get_velocity() const {
    return velocity;
}

void particle::move_particle(const float len) {
    pos.x += len * velocity.x;
    pos.y += len * velocity.y;
}

void particle::set_velocity(const float vel_x, const float vel_y) {
    velocity = {vel_x, vel_y};
}

emit_point::emit_point(const float pos_x, const float pos_y) {
    pos.x = pos_x; pos.y = pos_y;
}

particle emit_point::spawn_particle() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution nums_x(0.25f, 0.35f); // Разброс скорости
    std::uniform_real_distribution nums_y(-0.15f, 0.15f); // Разброс угла
    particle ret(pos.x, pos.y, nums_x(gen), nums_y(gen));

    return ret;
}

SDL_FPoint emit_point::get_position() const {
    return pos;
}

simulation::simulation(const std::vector<std::pair<std::string, std::string>>& conf) {
    subareas = spawn_areas(conf, &main_area);
    // Получаем границы для подобластей
    b_count = subareas.size();
    borders = new subarea_borders_t[b_count];
    for (size_t i = 0; i < b_count; i++)
        borders[i] = { subareas[i].x_start, subareas[i].x_start + subareas[i].width };

    emitter = spawn_emitter(conf);
    part_count = get_particles_count(conf);

    // Выделение памяти под треки
    tracks.reserve(part_count);
    for (size_t i = 0; i < part_count; i++) {
        tracks.emplace_back();
        tracks.back().reserve(5);
    }
}

bool simulation::process_particle() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution nums(0.0f, main_area.width);

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
        p.move_particle(nums(gen));
        tracks[current_part].push_back(p.get_position());

        auto sa_now = get_subarea_index(p);
        if (sa_now != sa_prev && sa_now > 0) {
            // Изменить направление
            std::uniform_real_distribution new_x_vel(-1.0f, 1.0f);
            std::uniform_real_distribution new_y_vel(-0.15f, 0.15f);
            p.set_velocity(new_x_vel(gen), new_y_vel(gen));
            sa_prev = sa_now;
        }
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

size_t simulation::get_subarea_index(const particle& p) const {
    auto pos = p.get_position();
    for (size_t id = 0; id < b_count; id++) {
        auto bord = borders[id];
        if (pos.x > bord.start && pos.x < bord.end) return id + 1; // Одна из подобластей
    }
    return 0; // Находимся в главной области
}

bool simulation::is_within_main(const SDL_FPoint p) const {
    if (p.x < 0.0f || p.y < 0.0f) return false;
    if (p.x > main_area.width || p.y > main_area.height) return false;

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
