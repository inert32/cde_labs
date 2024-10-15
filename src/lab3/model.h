#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <string>

#ifndef __ENABLE_GRAPH__ // SDL отключен, копируем определение SDL_FPoint
typedef struct SDL_FPoint
{
    float x;
    float y;
} SDL_FPoint;
#else
#include <SDL_rect.h>
#endif /* __ENABLE_GRAPH__ */

// Главная область моделирования
struct main_area_t {
    float width = 0.0f;
    float height = 0.0f;
};

// Подобласть моделирования внутри 
struct subarea_t {
    float x_start = 0.0f;
    float width = 0.0f;
};

struct vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

class particle {
public:
    particle(const float pos_x, const float pos_y, const float vel_x, const float vel_y);

    vec2 get_position() const;
    vec2 get_velocity() const;

    void move_particle(const float len);
    void set_velocity(const float vel_x, const float vel_y);
private:
    // Координаты частицы
    vec2 pos;
    // Вектор скорости
    vec2 velocity;
};

// Точечный источник частиц
class emit_point {
public:
    emit_point(const float pos_x, const float pos_y);
    emit_point() = default;

    particle spawn_particle();
    vec2 get_position() const;

private:
    vec2 pos;
    float last_angle = 45.0f;
};

class simulation {
public:
    simulation(const std::vector<std::pair<std::string, std::string>>& conf);
    ~simulation() = default;

    void next_particle();

    const main_area_t get_main_area() const;
    const std::vector<subarea_t>& get_subarea() const;
    const emit_point* get_emitter() const;
private:
    std::vector<std::vector<SDL_FPoint>> tracks;

    main_area_t main_area;
    std::vector<subarea_t> subareas;
    emit_point* emitter;
    size_t part_count = 0;
};

#endif /* __MODEL_H__ */