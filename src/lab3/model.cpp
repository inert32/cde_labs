// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "model.h"

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
