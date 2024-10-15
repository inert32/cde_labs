// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "model.h"
#include "parser.h"

particle::particle(const float pos_x, const float pos_y, const float vel_x, const float vel_y) {
    pos = {pos_x, pos_y};
    velocity = {vel_x, vel_y};
}

vec2 particle::get_position() const {
    return pos;
}

vec2 particle::get_velocity() const {
    return velocity;
}

void particle::move_particle(const float len) {
    // TODO: Перемещение частицы в зависимости от вектора скорости
}

void particle::set_velocity(const float vel_x, const float vel_y) {
    velocity = {vel_x, vel_y};
}

emit_point::emit_point(const float pos_x, const float pos_y) {
    pos.x = pos_x; pos.y = pos_y;
}

particle emit_point::spawn_particle() {
    particle ret(pos.x, pos.y, 0.0f, 0.0f);

    return ret;
}

vec2 emit_point::get_position() const {
    return pos;
}

simulation::simulation(const std::vector<std::pair<std::string, std::string>>& conf) {
    subareas = spawn_areas(conf, &main_area);
    emitter = spawn_emitter(conf);
    part_count = get_particles_count(conf);

    tracks.reserve(part_count);
    for (auto &i : tracks) i.reserve(5);
}

void simulation::next_particle() {}

const main_area_t simulation::get_main_area() const {
    return main_area;
}
const std::vector<subarea_t>& simulation::get_subarea() const {
    return subareas;
}
const emit_point* simulation::get_emitter() const {
    return emitter;
}
