// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <random>
#include <cmath>
#include "model.h"
#include "parser.h"

particle::particle(const float pos_x, const float pos_y, const float dir_x, const float dir_y, const float en) {
    pos = {pos_x, pos_y};
    direction = {dir_x, dir_y};
    energy = en;
}

SDL_FPoint particle::get_position() const {
    return pos;
}

float particle::get_energy() const {
    return energy;
}

void particle::move_particle(const float len) {
    pos.x += len * direction.x;
    pos.y += len * direction.y;
}

void particle::set_direction(const float dir_x, const float dir_y) {
    direction = {dir_x, dir_y};
}

void particle::set_energy(const float en) {
    energy = en;
}

emit_point::emit_point(const float pos_x, const float pos_y, const float angle, const std::vector<energy_distr_t>& dist) : energy(dist) {
    pos.x = pos_x; pos.y = pos_y; spread = angle;
}

particle emit_point::spawn_particle() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution nums_y(90.0f - spread, spread + 90.0f);
    float angle = (float)(nums_y(gen) * M_PI / 180.0f); // Перевод в радианы

    float dir_x = sin(angle);
    float dir_y = cos(angle);

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

SDL_FPoint emit_point::get_position() const {
    return pos;
}

simulation::simulation(const parser_data& conf) {
    // Загрузка материалов
    auto mats = load_materials(conf);
    if (mats.size() == 0)
        throw std::runtime_error("No materials found.");

    subareas = spawn_areas(conf, &main_area, mats);
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

    stat_subarea_energy = new float[b_count];
    for (size_t i = 0; i < b_count; i++) stat_subarea_energy[i] = 0.0f;
}

// Метод Карлсона для столкновения
void collide_carlson(particle& p, const float xi) {
    const float energy_now = p.get_energy();

    const float eps = 0.511f;
    const float alpha = energy_now / eps;

    const float S = energy_now / (1.0f + 0.5625f * alpha);
    float energy_new = energy_now / (1.0f + S + xi + (2.0f * alpha - S) * pow(xi, 3.0f));

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
    std::uniform_real_distribution new_angle(0.0, 2.0 * M_PI);
    auto angle = new_angle(gen);

    float new_cos = (float)cos(angle);
    float new_sin = (float)sin(angle);

    p.set_direction(new_cos, new_sin);
    p.set_energy(energy_new);
}

bool simulation::process_particle() {
    if (current_part >= part_count) return false; // Не рассчитываем частицы, если они не заданы
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution nums(0.0f, main_area.length);

    auto p = emitter->spawn_particle();
    tracks[current_part].push_back(p.get_position());
    float start_energy = p.get_energy();
    stat_total_energy += start_energy;

    do { // Движение частицы
        auto sa_now = get_subarea_index(p.get_position()); // Текущая подобласть: вакуум(0) или вещество(>0)

        float move = nums(gen);
        float opt = subareas[sa_now - 1].optics; // get_subarea_index смещена на +1, компенсируем
        if (sa_now != 0) move /= opt;

        // Транспортное ядро
        p.move_particle(move);
        tracks[current_part].push_back(p.get_position());
        sa_now = get_subarea_index(p.get_position());

        // Ядро столкновений
        if (sa_now > 0) {
            std::uniform_real_distribution xi(0.0f, 1.0f);
            if (xi(gen) > subareas[sa_now - 1].consume_prob) 
                collide_carlson(p, 0.5f);
            else {
                stat_subarea_energy[sa_now - 1] += p.get_energy();
                break;
            }
        }
    } while (is_within_main(p.get_position()));
    // На экран?
    auto end_pos = p.get_position();
    float end_energy = p.get_energy();
    if (end_pos.x > main_area.length) {
        stat_screen_particles++;
        stat_screen_energy += end_energy;
    }

    part_en.push_back({start_energy, end_energy});

    // Отладочный вывод
    std::cout << "Particle " << current_part << ": ";
    for (auto &c : tracks[current_part])
        std::cout << "{" << c.x << " " << c.y << "} ";
    std::cout << "; Energy: " << start_energy << "->" << end_energy << std::endl;
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
        auto& bord = borders[id];
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
    ret.energies = new SDL_FPoint[part_count];

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

sim_stats simulation::get_stats() const {
    sim_stats ret;
    const size_t size = subareas.size();

    ret.subarea_energy = new float[size];
    ret.subareas_count = size;
    for (size_t i = 0 ; i < size; i++)
        ret.subarea_energy[i] = stat_subarea_energy[i];

    ret.screen_energy = stat_screen_energy;
    ret.screen_particles = stat_screen_particles;

    ret.total_energy = stat_total_energy;
    ret.total_particles = part_count;

    return ret;
}
