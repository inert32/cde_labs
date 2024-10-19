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
    float optics = 0.0f;
};

class particle {
public:
    particle(const float pos_x, const float pos_y, const float dir_x, const float dir_y);

    SDL_FPoint get_position() const;

    void move_particle(const float len);
    void set_direction(const float dir_x, const float dir_y);
private:
    // Координаты частицы
    SDL_FPoint pos;
    // Вектор скорости
    SDL_FPoint direction;
};

// Точечный источник частиц
class emit_point {
public:
    emit_point(const float pos_x, const float pos_y, const float angle);
    emit_point() = default;

    particle spawn_particle();
    SDL_FPoint get_position() const;
private:
    SDL_FPoint pos;
    float spread;
};

// Выход симуляции - треки частиц
struct sim_output {
    // Массивы треков (посещенные точки в области)
    // Размерность: tracks[particle_count][track_len[i]]
    SDL_FPoint** tracks = nullptr;
    size_t* track_len = nullptr; // Число столкновений у частицы
    size_t particle_count = 0;
};

// Обработка частиц
class simulation {
public:
    simulation(const parser_data& conf);
    ~simulation() = default;

    // Расчет пути частицы
    // Возвращает false, если все частицы рассчитаны
    bool process_particle();

    const main_area_t get_main_area() const;
    const std::vector<subarea_t>& get_subarea() const;
    const emit_point* get_emitter() const;

    // Вывод данных о симуляции
    sim_output get_tracks() const;
private:
    std::vector<std::vector<SDL_FPoint>> tracks; // Треки частиц

    main_area_t main_area;
    std::vector<subarea_t> subareas;
    emit_point* emitter;
    size_t part_count = 0; // Общее число частиц
    size_t current_part = 0; // Номер текущей частицы

    // Границы подобластей для простоты расчета столкновений
    struct subarea_borders_t {float start = 0.0f; float end = 0.0f;};
    subarea_borders_t* borders = nullptr;
    size_t b_count = 0;
    // Изменяем направление частицы если мы перешли из одной подобласти в другую
    // (get_subarea_index до перемещения != get_subarea_index после)
    size_t get_subarea_index(const SDL_FPoint p) const;

    bool is_within_main(const SDL_FPoint p) const; // Проверка на вылет частицы за пределы main_area
};

#endif /* __MODEL_H__ */